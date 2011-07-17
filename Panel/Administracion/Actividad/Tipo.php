<?php
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
if(!$usuarioActivo->TienePermiso('CrearTipoActividad') && !$usuarioActivo->TienePermiso('ModTipoActividad'))
{
	if(!headers_sent())
		header('Location: ../../');
	exit;
}
if(
	isset($_POST['Nombre'])
	&& isset($_POST['Descripcion'])
	&& isset($_POST['Puntos'])
	&& isset($_POST['Experiencia']))
{
	if(isset($_POST['Edicion']))
	{
		try
		{
			$tipo = TipoActividad::Obtener($_POST['Edicion']);
			$tipo->Nombre = $_POST['Nombre'];
			$tipo->Descripcion = $_POST['Descripcion'];
			$tipo->Puntos = new BonoPuntos(
				new ItemPuntaje(intval($_POST['Puntos']), "Actividad: {$_POST['Nombre']}"),
				intval($_POST['Experiencia'])
			);
			$tipo->Actualizar();
			$finEdit = true;
		}
		catch(Exception $e)
		{
			$errorRegistro = $e->getMessage();
		}
	}
	else
	{
		$tipoNuevo = new TipoActividad(
			$_POST['Nombre'],
			$_POST['Descripcion'],
			new BonoPuntos(
				new ItemPuntaje(intval($_POST['Puntos']), "Actividad: {$_POST['Nombre']}"),
				intval($_POST['Experiencia'])
			)
		);
		try
		{
			$tipoNuevo->Registrar();
			$registroOk = true;
			$finEdit = true;
		}
		catch(Exception $e)
		{
			$errorRegistro = $e->getMessage();
		}
	}
}
$tipos = TipoActividad::ObtenerTodos();
$tipoEdicion = null;
if(!isset($finEdit) && (isset($_REQUEST['e']) || isset($_POST['Edicion'])))
{
	try
	{
		if(isset($_REQUEST['e']))
			$tipoEdicion = TipoActividad::Obtener($_REQUEST['e']);
		else
			$tipoEdicion = TipoActividad::Obtener($_POST['Edicion']);
	}
	catch(Exception $e) {}
	if(is_a($tipoEdicion, 'TipoActividad'))
		$edicion = true;
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Tipos de actividad - Club Orión de Astronomía</title>
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
<script type="text/javascript" src="../../../API/Jqueryui/js/jquery-ui-1.8.9.custom.min.js"></script>
<script type="text/javascript" src="../../../Estilos/General/Tablas.js"></script>
<script type="text/javascript">
$(document).ready(function(){
	$(".TablaAccion").TablaAccion();
	$("#SliderPuntos div.Slider").slider({
		value: $("#Puntos").val(),
		min: 0,
		max: 255,
		slide: function(event, ui) {
			$("#SliderPuntos div.Numero").text(ui.value);
			$("#Puntos").val(ui.value);
		}
	});
	$("#SliderExperiencia div.Slider").slider({
		value: $("#Experiencia").val(),
		min: 0,
		step: 1,
		max: 1000,
		slide: function(event, ui) {
			$("#SliderExperiencia div.Numero").text(ui.value);
			$("#Experiencia").val(ui.value);
		}
	});
	$("#SliderPuntos div.Numero").text($("#SliderPuntos div.Slider").slider("value"));
	$("#SliderExperiencia div.Numero").text($("#SliderExperiencia div.Slider").slider("value"));
	$("a.AccionEliminar").click(function() {
		if(confirm("¿Está seguro de eliminar este tipo de actividad?")){
			var tAct = $(this);
			$.ajax({
				url: '../../../Administracion/EliminarTipoActividad.json',
				data: {t: $(this).attr("rel")},
				complete: function(datar, estado) {
					if(estado == 'success' || estado == 'notmodified')
					{
						var respuesta = $.parseJSON(datar.responseText);
						if(respuesta.Estado == 'OK')
						{
							tAct.parents("tr").hide('medium', function() {
								$(this).remove();
								$(".TablaAccion").TablaAccion();
							});
						}
						else
						{
							if(respuesta.Mensaje)
							{
								alert(respuesta.Mensaje);
							}
							else
							{
								alert("Ocurrió un error al intentar eliminar el tipo de actividad");
							}
						}
					}
					else
					{
						alert("Ocurrió un error al intentar eliminar el tipo de actividad");
					}
				}
			});
		}
	});
});
</script>
<link href="../../../Estilos/General/Tablas.css" rel="stylesheet" type="text/css" />
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
		<h1>Administrar tipos de actividad</h1>
		<h2>Crear</h2>
<?php
if(isset($ok))
	echo MensajeHtml::JqueryUiInfo('Tipo de actividad registrada');
else if(isset($errorRegistro))
	echo MensajeHtml::JqueryUiError($errorRegistro);
?>
		<form id="fcrear" name="fcrear" method="post" action="">
		  <table class="Formulario">
		    <tr>
		      <th scope="row">Nombre</th>
		      <td><input type="text" name="Nombre" id="Nombre" <?php if(isset($edicion)) echo "value=\"$tipoEdicion->Nombre\"";?>/></td>
	        </tr>
		    <tr>
		      <th scope="row">Descripción</th>
		      <td><textarea name="Descripcion" id="Descripcion" cols="45" rows="5"><?php if(isset($edicion)) echo $tipoEdicion->Descripcion;?></textarea></td>
	        </tr>
		    <tr>
		      <th scope="row">Puntos</th>
		      <td><input type="hidden" name="Puntos" id="Puntos" value="<?php echo (isset($edicion)) ? $tipoEdicion->Puntos->Puntos->Cantidad : '5'; ?>"/><div id="SliderPuntos" class="ControlSlider"><div class="Numero">0</div><div class="Slider"></div></div></td>
	        </tr>
		    <tr>
		      <th scope="row">Experiencia</th>
		      <td><input type="hidden" name="Experiencia" id="Experiencia" value="<?php echo (isset($edicion)) ? $tipoEdicion->Puntos->Experiencia : '5'; ?>"/><div id="SliderExperiencia" class="ControlSlider"><div class="Numero">0</div><div class="Slider"></div></div></td>
	        </tr>
            <tr>
              <td colspan="2"><?php if(isset($edicion)) { ?><input type="hidden" name="Edicion" id="Edicion" value="<?php echo $tipoEdicion->Id; ?>" /><?php }?><input type="submit" name="button" id="button" value="<?php echo (isset($edicion)) ? 'Editar' : 'Registrar'; ?>" /></td>
            </tr>
	      </table>
	    </form>
        <h2>Tipos registrados</h2>
<?php
if(is_array($tipos))
{
?>
		  <table class="TablaAccion">
            <tr>
              <th scope="col">Tipo</th>
              <th scope="col">Puntos</th>
              <th scope="col">Experiencia</th>
            </tr>
<?php
	foreach($tipos as $tipo)
		if(is_a($tipo, 'TipoActividad'))
		{
?>
            <tr>
              <td><strong><?php echo $tipo->Nombre; ?></strong><br /><?php echo $tipo->Descripcion; ?></td>
              <td>Puntos: <?php echo $tipo->Puntos->Puntos->Cantidad; ?><br />Experiencia: <?php echo $tipo->Puntos->Experiencia; ?></td>
              <td><a href="?e=<?php echo $tipo->Id; ?>">Editar</a><br /><a href="javascript:void(0);" class="AccionEliminar" rel="<?php echo $tipo->Id; ?>">Eliminar</a></td>
            </tr>
<?php
		}
?>
          </table>
<?php
}
else
	echo MensajeHtml::JqueryUiInfo('No hay tipos registrados');
?>
		<!-- InstanceEndEditable --></div>
      <div style="clear:both"></div>
    </div>
  </div>
</div>
</body>
<!-- InstanceEnd --></html>