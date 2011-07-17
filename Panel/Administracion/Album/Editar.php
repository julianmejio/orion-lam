<?php
error_reporting(0);
require_once('../../../API/Orion/class.Config.inc');
require_once('../../../API/Orion/class.Uri.inc');
require_once('../../../API/Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
Script::Cargar('VerificarSesion', true);
Script::Cargar('NotificadorNivelExp');
if(!isset($_SESSION))
	session_start();
$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
$albumes = $usuarioActivo->ObtenerAlbumes();
if(!isset($_REQUEST['l']))
{
	if(!headers_sent())
		header("Location: index.php");
	exit;
}
$lamina = null;
try
{
	$lamina = Lamina::ObtenerLamina($_REQUEST['l']);
}
catch(Exception $e)
{
	$error = $e->getMessage();
}
if(!isset($error)
	&& isset($_POST['Clasificacion'])
	&& isset($_POST['Nombre'])
	&& isset($_POST['Descripcion'])
	&& isset($_POST['Abundancia'])
)
{
	try
	{
		$lamina->Clasificacion = $_POST['Clasificacion'];
		$lamina->Nombre = $_POST['Nombre'];
		$lamina->Descripcion = $_POST['Descripcion'];
		$lamina->Abundancia = $_POST['Abundancia'];
		if(isset($_FILES['Imagen']) && file_exists($_FILES['Imagen']['tmp_name']))
			try
			{
				require_once('../../../API/Asido/class.asido.php');
				require_once('../../../API/Asido/class.driver.gd.php');
				require_once('../../../API/Asido/class.driver.gd_hack.php');
				require_once('../../../API/Asido/class.driver.imagick_ext.php');
				require_once('../../../API/Asido/class.driver.imagick_ext_hack.php');
				require_once('../../../API/Asido/class.driver.imagick_shell.php');
				require_once('../../../API/Asido/class.driver.imagick_shell_hack.php');
				require_once('../../../API/Asido/class.driver.magick_wand.php');
				require_once('../../../API/Asido/class.driver.php');
				require_once('../../../API/Asido/class.driver.shell.php');
				require_once('../../../API/Asido/class.image.php');
				require_once('../../../API/Asido/class.imagick.php');
				$nombreImagen = sha1(microtime(true));
				$nombreImagenGrande = "$nombreImagen-o";
				$rutaImagen =  Uri::ObtenerRutaARaiz() . Config::RutaImagenes . '/Laminas/';
				asido::driver('gd');
				$imagen = asido::image($_FILES['Imagen']['tmp_name'], "$rutaImagen$nombreImagen.jpg");
				$imagenGrande = asido::image($_FILES['Imagen']['tmp_name'], "$rutaImagen$nombreImagen-o.jpg");
				asido::resize($imagen, 200, 198, ASIDO_RESIZE_FULLFILL);
				asido::fit($imagenGrande, 920, 550, ASIDO_RESIZE_FULLFILL);
				asido::watermark($imagen, '../../../Imagenes/MarcaAguaLamina.png', ASIDO_WATERMARK_TOP_RIGHT);
				asido::watermark($imagenGrande, '../../../Imagenes/MarcaAguaGrandeLamina.png', ASIDO_WATERMARK_BOTTOM_RIGHT);
				asido::convert($imagen, 'image/jpeg');
				asido::convert($imagenGrande, 'image/jpeg');
				$imagen->save();
				$imagenGrande->save();
				$lamina->NombreImagen = "$nombreImagen.jpg";
			}
			catch(Exception $e)
			{
				$errorSuave = $e->getMessage();
			}
		$lamina->Actualizar();
	}
	catch(Exception $e)
	{
		$errorDuro = $e->getMessage();
	}
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Editar lámina - Club Orión de Astronomía</title>
<!-- InstanceEndEditable -->
<!-- InstanceBeginEditable name="head" -->
<!-- InstanceEndEditable -->
<link href="../../../Estilos/General/ItemsFacebook.css" rel="stylesheet" type="text/css" />
<link href="../../../Estilos/General/Nonometro.css" rel="stylesheet" type="text/css" />
<link href="../../../Estilos/General/General.css" rel="stylesheet" type="text/css" />
<link href="../../../API/Jqueryui/css/orion/jquery-ui-1.8.9.custom.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../../API/Jquery/jquery-1.6.1.min.js"></script>
<script type="text/javascript" src="../../../API/Jqueryui/js/jquery-ui-1.8.9.custom.min.js"></script>
<script type="text/javascript">
$(document).ready(function(){
	$("#Menu").accordion({navigation: true});
<?php
if(isset($_SESSION['notificarNivel']))
{
?>
	alert("¡Felicitaciones! Por ser tan ñoño has recibido una bonificación de láminas para reclamar :D");
<?php
}
?>
});
<?php
if($albumes == null)
{
?>
function obtenerAlbum()
{
	$("#EstadoAlbum").html("<span class=\"InfoCarga\">Estamos creando tu álbum…</span>");	
	$.getJSON('<?php echo Config::RutaBase; ?>/Album/Crear.json', function(data){
		if(data['Estado'] == 'OK')
		{
			$("#EstadoAlbum").html("<a href=\"<?php echo Config::RutaBase; ?>/Panel/Album\">¡Ya tienes tu álbum de láminas, míralo!</a>");
		}
		else
			$("#EstadoAlbum").html("<span class=\"MensajeError\">No se pudo crear tu álbum. Intenta más tarde</a>");
	});
}
<?php
}
?>
</script>
<!-- InstanceBeginEditable name="posthead" -->
<link href="../../../Estilos/General/Album.css" rel="stylesheet" type="text/css" />
<link href="../../../Estilos/General/ItemsAlbum.css" rel="stylesheet" type="text/css" />
<link href="../../../API/JqueryPlugs/JqueryLightbox/css/jquery.lightbox-0.5.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../../Estilos/General/Album.js"></script>
<script type="text/javascript" src="../../../API/JqueryPlugs/JqueryAutoEllipsis/jquery.autoellipsis-1.0.1.min.js"></script>
<script type="text/javascript" src="../../../API/JqueryPlugs/JqueryLightbox/js/jquery.lightbox-0.5.min.js"></script>
<script type="text/javascript" src="../../../API/JqueryTools/jquery.tools.min.js"></script>
<script type="text/javascript">
$(document).ready(function(){
	$("#Clasificacion").autocomplete({
		source: '../../../Album/ListaClasificacion.json',
		minLength: 2
	});
	prepararLamina();
});
</script>
<link href="../../../Estilos/General/AdministradorAlbum.css" rel="stylesheet" type="text/css" />
<link href="../../../Estilos/General/Formularios.css" rel="stylesheet" type="text/css" />
<!-- InstanceEndEditable -->
</head>

<body>
<div id="ContenedorPanel">
  <div id="Panel" class="ui-corner-all">
    <div id="CabeceraPanel">
      <div id="ContenedorImagenLogo"><img src="../../../Imagenes/LogoPanel.png" width="202" height="100" alt="Logo del CLub Orión de Astronomía" /></div>
      <div id="ContenedorNombrePanel"></div>
      <div style="clear:both"></div>
    </div>
    <div id="ControlesPanel">
      <div id="ExtraBSPanel">
        <div id="IdentificadorUsuario"><?php
	  $itemUsuario = $usuarioActivo->GenerarItemX();
	  echo $itemUsuario->ATexto(true, false);
	  ?></div>
        <div id="Menu">
          <h2><a href="javascript:void(0)">Mi cuenta</a></h2>
          <div>
            <ul>
              <li><a href="../../Album">Álbum</a></li>
            </ul>
          </div>
          <h2><a href="javascript:void(0)">Club</a></h2>
          <div>
            <ul>
              <li><a href="../../Grupo/Lista.php">Grupos</a></li>
<?php
if($usuarioActivo->TienePermiso('RegistrarActas'))
{
?>
              <li><a href="../../Acta/Crear.php">Registrar acta</a></li>
<?php
}
?>
            </ul>
          </div>
<?php
if($usuarioActivo->TienePermiso('VerMenuAdmin'))
{
?>
          <h2><a href="javascript:void(0)">Administración</a></h2>
          <div><ul>
          <li>Actividad
<?php
if($usuarioActivo->TienePermiso('CrearTipoActividad') || $usuarioActivo->TienePermiso('ModTipoActividad'))
{
?>
<ul><li style="margin-left:5px; "><a href="<?php echo Config::RutaBase; ?>/Panel/Administracion/Actividad/Tipo.php">Tipos</a></li></ul>
<?php
}
?></li>
<?php
if($usuarioActivo->TienePermiso('CrearLaminaAlbum') || $usuarioActivo->TienePermiso('ModificarLaminaAlbum'))
{
?>
          <li><a href="<?php echo Config::RutaBase; ?>/Panel/Administracion/Album/">Álbum</a></li>
<?php
}
?>
<?php
	if($usuarioActivo->TienePermiso('UsuarioBajoNivel'))
	{
?>
          <li><a href="<?php echo Config::RutaBase; ?>/Panel/Administracion/CodigoArbitrario.php">Código arbitrario</a></li>
<?php
	}
	if($usuarioActivo->TienePermiso('VerGrupos'))
	{
?>
          <li><a href="../Grupo/">Grupos</a></li>
<?php
	}
?>
          <li><a href="../Usuario/">Usuarios</a></li>
          </ul></div>
<?php
}
?>
        </div>
      </div>
      <div id="ContenidoPanel">
        <div id="BarraEstado">
          <ul style="float:right">
<?php
$puntos = null;
try
{
	$puntos = $usuarioActivo->ObtenerPuntosDisponibles();
}
catch(Exception $e){}
if(is_int($puntos))
{
?>
            <li><?php if($puntos >= 1) { ?>Tienes <a href="<?php echo Config::RutaBase; ?>/Panel/Tienda"><span class="NombreEstado"><?php echo $puntos; ?> puntos</span></a><?php } else echo 'Tienes <a href="' . Config::RutaBase . '/Panel/Tienda">0 puntos</a>'; ?></li>
<?php
}
else
{
?>
            <li><?php echo MensajeHtml::InLightError('Tu puntaje no es visible.'); ?></li>
<?php
}
?>
            <li id="EstadoAlbum"><?php
$strAlbum = '<a href="javascript:void(0)" onclick="obtenerAlbum()">¡Obtén tu álbum de láminas!</a>';
if($albumes != null && count($albumes) > 1)
	$strAlbum = '<a href="' . Config::RutaBase . '/Panel/Album/Selector.php">Mira tus álbumes</a>';
else if($albumes != null && count($albumes) == 1)
{
	$album = $albumes[0];
	$strAlbum = '<span class="NombreEstado"><a href="' . Config::RutaBase . '/Panel/Album">Tu álbum</a></span>: ';
	$strAlbum .= count($album->Cromos) . ' láminas';
	if($album->NumeroCromosDisponibles >= 1)
		$strAlbum .= ', <a href="' . Config::RutaBase . '/Panel/Album/Llenar.php">¡Tienes ' . $album->NumeroCromosDisponibles . ' láminas disponibles!</a>';
}
echo $strAlbum;
?></li>
            <li><a href="../../../Sesion/CerrarSesion.php">Cerrar sesión</a></li>
          </ul>
          <div style="clear:both"></div>
        </div>
        <!-- InstanceBeginEditable name="ContenidoPanel" -->        
        <h1>Editar lámina</h1>        
<?php
if(isset($error))
	echo MensajeHtml::JqueryUiError($error);
else
{
	if(isset($errorSuave))
		echo MensajeHtml::JqueryUiInfo('No se actualizó la imagen de la lámina');
	if(isset($errorDuro))
		echo MensajeHtml::JqueryUiError("Ocurrió un error actualizando la lámina: $errorDuro");
?>
<div id="ContenedorLamina">
<?php
$itemLamina = $lamina->GenerarItemX();
echo $itemLamina->ATexto(true, false);
?>
</div>
<div id="EditorLamina">
  <form action="?l=<?php echo $lamina->Id; ?>" method="post" enctype="multipart/form-data" name="DatosLamina" id="DatosLamina">
    <table class="Formulario">
      <tr>
        <th scope="row"><label for="Clasificacion">Clasificación</label></th>
        <td>          <input type="text" name="Clasificacion" id="Clasificacion" value="<?php echo $lamina->Clasificacion; ?>" /></td>
      </tr>
      <tr>
        <th scope="row"><label for="Nombre">Nombre</label></th>
        <td>
          <input type="text" name="Nombre" id="Nombre" value="<?php echo $lamina->Nombre; ?>" /></td>
      </tr>
      <tr>
        <th scope="row"><label for="Descripcion">Descripción</label></th>
        <td>
          <textarea name="Descripcion" id="Descripcion" cols="45" rows="5"><?php echo $lamina->Descripcion; ?></textarea></td>
      </tr>
      <tr>
        <th scope="row"><label for="Abundancia">Abundancia</label></th>
        <td>
          <select name="Abundancia" id="Abundancia">
                <option value="1" <?php if($lamina->Abundancia == 1) echo 'selected="selected"'; ?>>Muy abundante (1)</option>
                <option value="0.9" <?php if($lamina->Abundancia == 0.9) echo 'selected="selected"'; ?>>Abundante (0.9)</option>
                <option value="0.8" <?php if($lamina->Abundancia == 0.8) echo 'selected="selected"'; ?>>Abundante (0.8)</option>
                <option value="0.7" <?php if($lamina->Abundancia == 0.7) echo 'selected="selected"'; ?>>Escasa (0.7)</option>
                <option value="0.6" <?php if($lamina->Abundancia == 0.6) echo 'selected="selected"'; ?>>Escasa (0.6)</option>
                <option value="0.5" <?php if($lamina->Abundancia == 0.5) echo 'selected="selected"'; ?>>Escasa (0.5)</option>
                <option value="0.4" <?php if($lamina->Abundancia == 0.4) echo 'selected="selected"'; ?>>Muy escasa (0.4)</option>
                <option value="0.3" <?php if($lamina->Abundancia == 0.3) echo 'selected="selected"'; ?>>Muy escasa (0.3)</option>
                <option value="0.2" <?php if($lamina->Abundancia == 0.2) echo 'selected="selected"'; ?>>Única (0.2)</option>
                <option value="0.1" <?php if($lamina->Abundancia == 0.1) echo 'selected="selected"'; ?>>Única (0.1)</option>
                <option value="0" <?php if($lamina->Abundancia == 0) echo 'selected="selected"'; ?>>Única (0)</option>
          </select></td>
      </tr>
      <tr>
        <th scope="row"><label for="Imagen">Imagen</label></th>
        <td><?php echo MensajeHtml::JqueryUiInfo('Selecciona una imagen sólo si vas a cambiar la actual');?>
          
          <input type="file" name="Imagen" id="Imagen" /></td>
      </tr>
      <tr>
        <td colspan="2" scope="row"><input type="submit" name="Editar" id="Editar" value="Editar" /></td>
        </tr>
    </table>
  </form>

</div>
<?php
}
$itemD = Lamina::GenerarContenedorDescripcion();
echo $itemD->ATexto(true, false);
?>
        <!-- InstanceEndEditable --></div>
      <div style="clear:both"></div>
    </div>
  </div>
</div>
</body>
<!-- InstanceEnd --></html>