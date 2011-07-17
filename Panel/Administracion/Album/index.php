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
if(
	isset($_POST['Clasificacion'])
	&& isset($_POST['Nombre'])
	&& isset($_POST['Descripcion'])
	&& isset($_POST['Abundancia'])
	&& isset($_FILES['Imagen']))
{
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
		$lamina = new Lamina($_POST['Nombre'], $_POST['Clasificacion'], $_POST['Descripcion'], $_POST['Abundancia'], "$nombreImagen.jpg");
		$lamina->Registrar();
		$okCreacion = true;
	}
	catch(Exception $e)
	{
		$errorCreacion = $e->getMessage();
	}
}
$laminas = Lamina::ObtenerLaminas();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Administración del álbum - Club Orión de Astronomía</title>
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
<link href="../../../Estilos/General/Formularios.css" rel="stylesheet" type="text/css" />
<link href="../../../Estilos/General/Tablas.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../../Estilos/General/Tablas.js"></script>
<script type="text/javascript">
$(document).ready(function(){
	$(".TablaAccion").TablaAccion();
	$("#Clasificacion").autocomplete({
		source: '../../../Album/ListaClasificacion.json',
		minLength: 2
	});
	$(".botonEliminar").click(function(){
		if(confirm("¿Está seguro de eliminar esta lámina? También se eliminará de cualquier álbum en que esté"))
		{
			var lLink = $(this);
			$.ajax({
				url: '../../../Administracion/EliminarLaminaAlbum.json',
				data: {l: $(this).attr("rel")},
				complete: function(datar, estado) {
					if(estado == 'success' || estado == 'notmodified') {
						var respuesta = $.parseJSON(datar.responseText);
						if(respuesta.Estado == 'OK') {
							lLink.parents("tr").hide('medium', function() {
								$(this).remove();
								$(".TablaAccion").TablaAccion();
							});
						} else {
							if(respuesta.Mensaje) {
								alert(respuesta.Mensaje);
							} else {
								alert("Ocurrió un error al intentar eliminar la lámina");
							}
						}
					} else {
						alert("Ocurrió un error en la conexión cuando se intentaba eliminar la lámina seleccionada");
					}
				}
			});
		}
	});
});
</script>
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
        <h1>Administrar contenido del álbum</h1>
<?php
if($usuarioActivo->TienePermiso('CrearLaminaAlbum'))
{
	if(isset($okCreacion))
		echo "<p>Lámina creada</p>";
	else if(isset($errorCreacion))
		echo MensajeHtml::JqueryUiError($errorCreacion);
?>
        <h2>Crear lámina</h2>
        <form action="" method="post" enctype="multipart/form-data" name="fCrearLamina" id="fCrearLamina">
          <table class="Formulario">
            <tr>
              <th scope="row"><label for="Clasificacion">Clasificación</label></th>
              <td><input type="text" name="Clasificacion" id="Clasificacion" /></td>
            </tr>
            <tr>
              <th scope="row"><label for="Nombre">Nombre</label></th>
              <td><input type="text" name="Nombre" id="Nombre" /></td>
            </tr>
            <tr>
              <th scope="row"><label for="Descripcion">Descripción</label></th>
              <td><textarea name="Descripcion" id="Descripcion" cols="45" rows="5"></textarea></td>
            </tr>
            <tr>
              <th scope="row"><label for="Abundancia">Abundancia</label></th>
              <td><select name="Abundancia" id="Abundancia">
                <option value="1" selected="selected">Muy abundante (1)</option>
                <option value="0.9">Abundante (0.9)</option>
                <option value="0.8">Abundante (0.8)</option>
                <option value="0.7">Escasa (0.7)</option>
                <option value="0.6">Escasa (0.6)</option>
                <option value="0.5">Escasa (0.5)</option>
                <option value="0.4">Muy escasa (0.4)</option>
                <option value="0.3">Muy escasa (0.3)</option>
                <option value="0.2">Única (0.2)</option>
                <option value="0.1">Única (0.1)</option>
                <option value="0">Única (0)</option>
              </select></td>
            </tr>
            <tr>
              <th scope="row"><label for="Imagen">Imagen</label></th>
              <td><input type="file" name="Imagen" id="Imagen" /></td>
            </tr>
            <tr>
              <td colspan="2" scope="row"><input type="submit" name="Enviar" id="Enviar" value="Crear" /></td>
            </tr>
          </table>
        </form>
<?php
}
?>
        <h2>Láminas</h2>
<?php
if(is_array($laminas))
{
?>
        <table class="TablaAccion">
          <tr>
            <th scope="col" style="width: 50%">Nombre</th>
            <th scope="col" style="width: 20%">Clasificación</th>
            <th scope="col" style="width: 10%">Abundancia</th>
            <th scope="col" style="width: 20%">Acción</th>
          </tr>
<?php
	foreach($laminas as $lamina)
		if(is_a($lamina, 'Lamina'))
		{
?>
          <tr>
            <td><?php echo "<strong>$lamina->Nombre</strong>"; if($lamina->Descripcion != null) echo "<br />$lamina->Descripcion"; ?></td>
            <td><?php echo $lamina->Clasificacion; ?></td>
            <td style="text-align:center"><?php echo $lamina->Abundancia; ?></td>
            <td><a class="botonEliminar" href="javascript:void(0)" rel="<?php echo $lamina->Id; ?>">Eliminar</a><br />
            <a href="Editar.php?l=<?php echo $lamina->Id; ?>">Modificar</a><br />
            <a href="javascript:void(0)">Ver imagen</a>
            </td>
          </tr>
<?php
		}
?>
        </table>
<?php
}
else
	echo MensajeHtml::JqueryUiError('No hay lámians registradas en los contenidos del álbum');
?>
        <!-- InstanceEndEditable --></div>
      <div style="clear:both"></div>
    </div>
  </div>
</div>
</body>
<!-- InstanceEnd --></html>