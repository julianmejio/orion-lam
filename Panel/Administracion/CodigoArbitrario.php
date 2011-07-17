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
if(!$usuarioActivo->TienePermiso('UsuarioBajoNivel'))
{
	if(!headers_sent())
		header('Location: ' . Config::RutaBase . '/Panel');
	exit;
}
$albumes = $usuarioActivo->ObtenerAlbumes();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Código arbitrario - Club Orión de Astronomía</title>
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
<script type="text/javascript">
$(document).ready(function(){
	$("#bEjecutarCodigo").button();
});
function ejecutarCodigo()
{
	$("#dialogoConfirmacion").dialog({
		resizable: false,
		height: 400,
		width: 550,
		modal: true,
		buttons: {
			"¡Sí! estoy seguro y asumo la responsabilidad" : function()
			{
				ejecutarCodigoArbitrario();
				$(this).dialog("close");
			},
			Cancel: function()
			{
				$(this).dialog("close");
			}
		}
	});
}
var e = null;
function ejecutarCodigoArbitrario()
{
	var codigoArbitrario = $("#CodigoArbitrario").val();
	$.ajax({
		url: '<?php echo Config::RutaBase; ?>/Administracion/EjecutarCodigoArbitrario.json',
		type: 'POST',
		data: {CodigoArbitrario: codigoArbitrario},
		complete: function(Respuesta) {
			if(Respuesta['status'] == 200 || Respuesta['status'] == 304)
			{
				var datos = eval('(' + Respuesta['responseText'] + ')');
				if(datos['Estado'] == 'OK')
				{
					alert("La ejecución del código se completó exitosamente. Esto no quiere decir que el código haya sido ejecutado correctamente.");
					$("#ResultadoEjecucion").text(datos['Mensaje']);
				}
				else
					alert("Error: " + datos['Mensaje']);
			}
			else
			{
				alert("Error: " + Respuesta['statusText']);
			}
		},
		error: function(jqXhr, Estado, Error) {
			alert("Error: " + Estado);
		}
	});
}
</script>
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
              <li><a href="../Acta/Crear.php">Registrar acta</a></li>
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
          <li><a href="Grupo/">Grupos</a></li>
<?php
	}
?>
          <li><a href="Usuario/">Usuarios</a></li>
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
        <div class="ui-widget">
          <div class="ui-state-highlight ui-corner-all">
            <p><span class="ui-icon ui-icon-alert" style="float:left;margin-right:1em"></span>Advertencia: esta página le ofrecerá funcionalidad a bajo nivel sobre <strong>todo el sistema</strong>, por eso ejecutar código arbitrario es considerado una vulnerabilidad sobre el sistema y sólo aquella persona que tenga privilegios podrá hacerlo. Podrá hacer uso del API PHP especial de Orión, incluyendo la capa de conexión a la base de datos. Escriba el código PHP que quiere ejecutar, pero primero asegúrese de que no hayan instrucciones que puedan afectar el sistema. Recuerde que esta debe ser la última opción, en caso de que no hayan funciones manejadas implementadas para lo que está intentando hacer. Adicional se registrará la información de esta ejecución: usuario, fecha y código ejecutado. También es recomendable realizar una copia de seguridad de la base de datos y los archivos. ¡<strong>Sólo ejecute el código una vez esté completamente seguro de lo que hace</strong>!</p>
          </div>
        </div>
        <div>
          <label for="CodigoArbitrario">Código:</label>
          <textarea style="width: 100%; height:20em" name="CodigoArbitrario" id="CodigoArbitrario" cols="45" rows="5"></textarea>
          <input id="bEjecutarCodigo" type="button" value="¡Ejecutar!" onclick="ejecutarCodigo()" />
        </div>
        <div id="dialogoConfirmacion" title="¿Seguro que deseas ejecutar este código?">
          <p><span class="ui-icon ui-icon-alert" style="float:left;margin-right:1em"></span>¿Está seguro de hacer esto? Recuerde que puede ser lo último que haga si todo sale mal. Por favor, rectifique las veces que sea necesario su código arbitrario y luego de que esté seguro, puede continuar.</p>
        </div>
        <div id="Resultado">
        <p>En esta sección encontrará el resultado (si lo arroja) de la ejecución de código arbitrario.</p>
        <pre id="ResultadoEjecucion" style="height:500px; overflow:auto; background-color:#FFF; margin:1em;">
        </pre>
        </div>
        <!-- InstanceEndEditable --></div>
      <div style="clear:both"></div>
    </div>
  </div>
</div>
</body>
<!-- InstanceEnd --></html>