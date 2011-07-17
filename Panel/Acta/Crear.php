<?php
require_once('../../API/Orion/class.Config.inc');
require_once('../../API/Orion/class.Uri.inc');
require_once('../../API/Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
Script::Cargar('VerificarSesion', true);
Script::Cargar('NotificadorNivelExp');
if(!isset($_SESSION))
	session_start();
$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
if(!$usuarioActivo->TienePermiso('RegistrarActas'))
{
	if(!headers_sent())
		header("Location: ../");
	exit;
}
$acta = null;
if(
	(isset($_POST['Grupo']) || isset($_POST['Editar']))
	&& isset($_POST['iFecha'])
	&& isset($_POST['iDetalleActa']))
{
	try
	{
		$actividades = array();
		if(isset($_POST['Editar']))
		{
			$acta = Acta::Obtener($_POST['Editar']);
			$acta->Fecha = $_POST['iFecha'];
			$acta->Resumen = $_POST['iDetalleActa'];
		}
		else
			$acta = new Acta(Grupo::Cargar($_POST['Grupo']), $_POST['iFecha'], $_POST['iDetalleActa']);
		foreach($_POST as $d => $v)
		{
			$dat = null;
			if(preg_match('/^a_([\d]+)/', $d, $dat))
				$acta->AgregarAsistencia(new UsuarioFacebook($dat[1]));
			else if($d == 'Actividad' && is_array($v))
				for($i = 0; $i < count($v); $i++)
					if($_POST['Integrante'][$i] != 0)
						$actividades[] = new Actividad(TipoActividad::Obtener($_POST['Actividad'][$i]), new UsuarioFacebook($_POST['Integrante'][$i]), $_POST['DescripcionActividad'][$i]);
		}
		if(isset($_POST['Editar']))
			$acta->Actualizar();
		else
			$acta->Registrar();
		foreach($actividades as $actividad)
		{
			try
			{
				$actividad->Registrar();
				$actividad->AsociarActa($acta, $acta->Fecha);
			}
			catch(Exception $e) {$errorRegAct = $e->getMessage(); }
		}
		if(!isset($errorRegAct))
			$ok = true;
	}
	catch(Exception $e)
	{
		$errorRegistro = $e->getMessage();
	}
}
$albumes = $usuarioActivo->ObtenerAlbumes();
try
{
	$actividades = TipoActividad::ObtenerTodos();
	$miembros = UsuarioFacebook::ObtenerMiembros();
}
catch(Exception $e)
{
	$errorSub = $e->getMessage();
}
if(isset($_REQUEST['e']))
{
	try
	{
		$actaEdicion = Acta::Obtener($_REQUEST['e']);
	}
	catch(Exception $e)
	{
		$errorFatal = $e->getMessage();
	}
}
if(!isset($errorSub) && count($miembros) >= 1 && count($actividades) >= 1)
{
	$widgetActividad = '<div class="DialogoActividadActa"><table class="Formulario"><tr><th scope="row">Integrante</th><td><select name="Integrante[]"><option value="0">Selecciona alguno…</option>';
	foreach($miembros as $miembro)
		$widgetActividad .= "<option value=\"$miembro->IdFacebook\">{$miembro->DatosFacebook['name']}</option>";
	$widgetActividad .= '</select></td></tr><tr><th scope="row">Actividad</th><td><select name="Actividad[]">';
	foreach($actividades as $actividad)
		$widgetActividad .= "<option value=\"$actividad->Id\">$actividad->Nombre</option>";
	$widgetActividad .= '</select></td></tr><tr><th scope="row">Descripción</th><td><textarea name="DescripcionActividad[]" cols="45" rows="5"></textarea></td></tr><tr><td colspan="2" scope="row"><a href="javascript:void(0)" class="QuitarActividad">Quitar actividad</a> <a href="javascript:void(0);" class="AgregarActividad">Otra actividad</a></td></tr></table></div>';
}
$grupos = null;
if($usuarioActivo->TienePermiso('VerGrupos'))
	$grupos = Grupo::ObtenerTodos();
else
	$grupos = $usuarioActivo->ObtenerGruposAsociados();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Club Orión de Astronomía</title>
<!-- InstanceEndEditable -->
<!-- InstanceBeginEditable name="head" -->
<!-- InstanceEndEditable -->
<link href="../../Estilos/General/ItemsFacebook.css" rel="stylesheet" type="text/css" />
<link href="../../Estilos/General/Nonometro.css" rel="stylesheet" type="text/css" />
<link href="../../Estilos/General/General.css" rel="stylesheet" type="text/css" />
<link href="../../API/Jqueryui/css/orion/jquery-ui-1.8.9.custom.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../API/Jquery/jquery-1.6.1.min.js"></script>
<script type="text/javascript" src="../../API/Jqueryui/js/jquery-ui-1.8.9.custom.min.js"></script>
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
<script type="text/javascript" src="../../API/Tinymce/Jquery/jscripts/tiny_mce/tiny_mce.js"></script>
<script type="text/javascript" src="../../API/Tinymce/Jquery/jscripts/tiny_mce/jquery.tinymce.js"></script>
<script type="text/javascript">
$(document).ready(function() {
	$('div.DialogoActividadActa:first a.QuitarActividad').hide();
	$("a.AgregarActividad").click(function() {
		$(this).parents('div.DialogoActividadActa').each(function(){
			$("#ContenedorActividadesRealizadas").append($(this).clone(true));
		});
		$('div.DialogoActividadActa a.QuitarActividad').show();
		$('div.DialogoActividadActa:first a.QuitarActividad').hide();
		$('div.DialogoActividadActa a.AgregarActividad').hide();
		$('div.DialogoActividadActa:last a.AgregarActividad').show();
	});
	$("a.QuitarActividad").click(function() {
		$(this).parents('div.DialogoActividadActa').each(function(){
			$(this).remove();
		});
		$('div.DialogoActividadActa:last a.AgregarActividad').show();
	});
	$('textarea#iDetalleActa').tinymce({
		script_url: '../../API/Tinymce/Compressor/tiny_mce_gzip.php',
		theme: 'advanced',
		plugins : "pagebreak,style,layer,table,save,advhr,advimage,advlink,emotions,iespell,inlinepopups,insertdatetime,preview,media,searchreplace,print,contextmenu,paste,directionality,fullscreen,noneditable,visualchars,nonbreaking,xhtmlxtras,template",
        theme_advanced_buttons1 : "newdocument,|,bold,italic,underline,strikethrough,|,justifyleft,justifycenter,justifyright,justifyfull,styleselect,formatselect",
		theme_advanced_buttons2 : "cut,copy,paste,pastetext,pasteword,|,search,replace,|,bullist,numlist,|,outdent,indent,blockquote,|,undo,redo,|,link,unlink,cleanup,|,insertdate,inserttime,preview,|,forecolor,backcolor",
        theme_advanced_buttons3 : "tablecontrols,|,hr,removeformat,visualaid,|,sub,sup,|,charmap,iespell,media,|,fullscreen",
        theme_advanced_buttons4 : "cite,abbr,acronym,|,visualchars,|,code",
        theme_advanced_toolbar_location : "top",
        theme_advanced_toolbar_align : "center",
        theme_advanced_statusbar_location : "bottom",
        theme_advanced_resizing : false,
		content_css: '../../Estilos/General/ActaContenidoEdicion.css',
		language: 'es',
		entity_encoding: "raw"
	});
	$("#iFecha").datepicker({dateFormat: 'yy-mm-dd'});
});
</script>
<link href="../../Estilos/General/Acta.css" rel="stylesheet" type="text/css" />
<link href="../../Estilos/General/Formularios.css" rel="stylesheet" type="text/css" />
<!-- InstanceEndEditable -->
</head>

<body>
<div id="ContenedorPanel">
  <div id="Panel" class="ui-corner-all">
    <div id="CabeceraPanel">
      <div id="ContenedorImagenLogo"><img src="../../Imagenes/LogoPanel.png" width="202" height="100" alt="Logo del CLub Orión de Astronomía" /></div>
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
              <li><a href="../Album">Álbum</a></li>
            </ul>
          </div>
          <h2><a href="javascript:void(0)">Club</a></h2>
          <div>
            <ul>
              <li><a href="../Grupo/Lista.php">Grupos</a></li>
<?php
if($usuarioActivo->TienePermiso('RegistrarActas'))
{
?>
              <li><a href="Crear.php">Registrar acta</a></li>
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
          <li><a href="../Administracion/Grupo/">Grupos</a></li>
<?php
	}
?>
          <li><a href="../Administracion/Usuario/">Usuarios</a></li>
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
            <li><a href="../../Sesion/CerrarSesion.php">Cerrar sesión</a></li>
          </ul>
          <div style="clear:both"></div>
        </div>
        <!-- InstanceBeginEditable name="ContenidoPanel" -->
<?php
if(isset($errorFatal))
	echo MensajeHtml::JqueryUiError($errorFatal);
else
{
	if(isset($errorRegistro))
		echo MensajeHtml::JqueryUiError($errorRegistro);
	else if(isset($errorRegAct))
		echo MensajeHtml::JqueryUiError("Se registró el acta con errores: $errorRegAct. <a href=\"../Grupo/Acta.php?a=$acta->Id\">Visualizar</a>");
	else if(isset($ok))
		echo MensajeHtml::JqueryUiInfo("Acta guardada. <a href=\"../Grupo/Acta.php?a=$acta->Id\">Visualizar</a>.");
?>
        <h1>Registrar acta</h1>
<?php
if(isset($actaEdicion))
	echo MensajeHtml::JqueryUiInfo('Estás actualizando un acta. Sólo puedes editar la fecha, editar el resumen, agregar actividades y agregar asistencia.');
?>
        <form action="" method="post" name="acta">
        <div id="ControlActa" class="ui-corner-all">
          <div id="ContenedorDetalleActa">
            <h2>Detalles</h2>
            <table class="Formulario">
              <tr>
                <th scope="row"><label for="Grupo">Grupo reunido:</label></th>
                <td><select name="Grupo" id="Grupo" <?php if(isset($actaEdicion)) echo 'disabled="disabled"'; ?>>
<?php
$grupoSel = null;
if(isset($actaEdicion))
	$grupoSel = $actaEdicion->Grupo;
foreach($grupos as $grupo)
{
?>
                  <option value="<?php echo $grupo->Id; ?>" <?php if(is_a($grupoSel, 'Grupo') && $grupoSel->Id == $grupo->Id) echo 'selected="selected"'; ?>><?php echo $grupo->Nombre; ?></option>
<?php
}
?>
                </select></td>
              </tr>
              <tr>
                <th scope="row"><label for="iFecha">Fecha:</label></th>
                <td><input type="text" name="iFecha" id="iFecha" value="<?php echo (isset($actaEdicion)) ? $actaEdicion->Fecha : date("Y-m-d"); ?>" /></td>
              </tr>
              <tr>
                <th colspan="2" scope="row">Detalle</th>
              </tr>
              <tr>
                <td colspan="2" scope="row"><label for="iDetalleActa"></label>
                  <textarea name="iDetalleActa" id="iDetalleActa" cols="45" rows="5" style="height:25em"><?php if(isset($actaEdicion)) echo $actaEdicion->Resumen; ?></textarea></td>
              </tr>
            </table>
          </div>
          <div id="ContenedorDetalleActividades">
            <div id="ContenedorActividadesRealizadas">
              <h2>Actividades realizadas</h2>
<?php
$actividades = null;
if(isset($actaEdicion))
{
	$actividades = $actaEdicion->ObtenerActividades();
	if($actividades != null)
	{
		echo '<ul>';
		foreach($actividades as $actividad)
			echo "<li><strong>{$actividad->Responsable->DatosFacebook['name']}</strong> en <strong>{$actividad->Tipo->Nombre}</strong></li>";
		echo '</ul>';
	}
}
echo $widgetActividad;
?>
            </div>
            <div id="ContenedorAsistencia">
              <h2>Asistencia</h2>
<?php
if(is_array($miembros) && count($miembros) >= 1)
{
?>
              <table>
<?php
	$asistentes = (isset($actaEdicion)) ? $actaEdicion->Asistencia : null;
	foreach($miembros as $miembro)
	{
		$pre = 'a';
		$registrado = false;
		if($asistentes != null)
			foreach($asistentes as $asistente)
				if($asistente->IdFacebook == $miembro->IdFacebook)
				{
					$pre = 'b';
					$registrado = true;
					break;
				}
?>
                <tr>
                  <td><input type="checkbox" name="<?php echo "{$pre}_$miembro->IdFacebook"; ?>" id="<?php echo "{$pre}_$miembro->IdFacebook"; ?>" <?php if($registrado) echo 'disabled="disabled" checked="checked"'; ?> /></td>
                  <td><label for="<?php echo "{$pre}_$miembro->IdFacebook"; ?>" <?php
                  ?>><?php echo $miembro->DatosFacebook['name']; ?></label></td>
                </tr>
<?php
	}
?>
              </table>
<?php
}
else if(count($miembros) == 0)
	echo MensajeHtml::JqueryUiError('¡Ups! No hay miembros del club');
else
{
	$mensaje = '¡No pude hacer la lista de los miembros del Club!';
	if(isset($error))
		$mensaje = "¡No pude hace la lista de los miembros! Al parecer esto pasó: $error";
	echo MensajeHtml::JqueryUiError($mensaje);	
}
?>
            </div>
            <div style="clear:both"></div>
          </div>
<?php
echo MensajeHtml::JqueryUiInfo('Antes de registrar esta acta, verifica nuevamente todos los datos, ya que no es posible, después de haberla registrado, modificar la lista de asistencia, la fecha o el grupo al que ha sido asociado el acta');
if(isset($actaEdicion))
{
?>
<input type="hidden" id="Editar" name="Editar" value="<?php echo $actaEdicion->Id; ?>" />
<?php
}
?>
          <input type="submit" name="button" id="button" value="<?php if(isset($actaEdicion)) echo 'Actualizar'; else echo 'Registrar'; ?>" />
        </div>
        </form>
<?php } ?>
        <!-- InstanceEndEditable --></div>
      <div style="clear:both"></div>
    </div>
  </div>
</div>
</body>
<!-- InstanceEnd --></html>