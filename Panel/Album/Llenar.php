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
$albumes = $usuarioActivo->ObtenerAlbumes();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Llenar álbum - Club Orión de Astronomía</title>
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
<link href="../../Estilos/General/ItemsAlbum.css" rel="stylesheet" type="text/css" />
<link href="../../API/JqueryPlugs/JqueryLightbox/css/jquery.lightbox-0.5.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../Estilos/General/Album.js"></script>
<script type="text/javascript" src="../../API/JqueryPlugs/JqueryAutoEllipsis/jquery.autoellipsis-1.0.1.min.js"></script>
<script type="text/javascript" src="../../API/JqueryPlugs/JqueryLightbox/js/jquery.lightbox-0.5.min.js"></script>
<script type="text/javascript" src="../../API/JqueryTools/jquery.tools.min.js"></script>
<style type="text/css">
div.Lamina.N {
	cursor: pointer;
}
.DivOculto {
	display: none;
	height: 0;
}
</style>
<script type="text/javascript">
$(document).ready(function() {
	$("div.Lamina.N").addClass("removible");
	$("div.Lamina.N").click(function(){
		var cromoEscogido = $(this);
		$(this).removeClass("removible");
		$("div.Lamina.N").unbind("click");
		$("div.Lamina.N").css("cursor", "auto");
		$(".removible").hide(550);
		$("#ControlSeleccionCromos").animate({width: 230}, 600, function(){
			$(".removible").remove();
			$(cromoEscogido).html('<p class="InfoCarga" style="font-style: italic">…y entre todas, esta es la que escogiste.</p>');
			$.getJSON('<?php echo Config::RutaBase . '/Album/Llenar.json'; ?>',
				{c: $(cromoEscogido).attr('id')},
				function(data)
				{
					$(cromoEscogido).slideUp('slow', function()
					{
						$(cromoEscogido).remove();
						$("#ControlSeleccionCromos").html('<div id="LaminaEscogida" class="DivOculto" style="230"></div><div id="MensajeRetornoLamina" class="DivColuto" style="float:right;width:460px;font-size:1.6em;text-align:center"></div><div style="clear:both"></div>');
						if(data['Estado'] == 'OK')
						{
							$("#LaminaEscogida").html(data['LaminaHtml']);
							$("#LaminaEscogida").slideDown('slow', function(){
								$("#ControlSeleccionCromos").animate({width: 690}, 600, function(){
									$("#MensajeRetornoLamina").text(data['MensajeRetorno']);
									$("#MensajeRetornoLamina").slideDown('slow');
									$("#EstadoAlbum").html('<a href="<?php echo Config::RutaBase . '/Panel/Album'; ?>">¡Mira tu álbum!</a>');
									prepararLamina();
								});
							});
						}
						else
						{
							$("#LaminaEscogida").html(data['MensajeHtml']);
							$("#LaminaEscogida").slideDown();
							$("#EstadoAlbum").html('<a href="<?php echo Config::RutaBase . '/Panel/Album/Llenar.php'; ?>">Intenta reclamar otra lámina</a>');
						}
					});
				});
		});
	});
});
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
        <h1>¡Reclama una lámina para tu álbum!</h1>
        <div id="ContenedorSelectorLaminas">
          <p style="width:690px; margin: 0 auto;text-align:center;font-style:italic">Te daré una de las siguientes láminas, la que escojas. ¡Pero escoge con cuidado! no sea que desperdicies la oportunidad de tener una lámina interesante…</p>
<?php
if(is_array($albumes) && count($albumes) >= 1)
{
	$album = $albumes[0];
	if($album->NumeroCromosDisponibles >= 1)
	{
		try
		{
			$album->LimpiarSeleccionCromo();
			$cromosSeleccion = $album->IniciarSeleccionCromo();
			if(is_array($cromosSeleccion) && count($cromosSeleccion) >= 1)
			{
				$iv = Lamina::GenerarItemVacioX();				
?>
          <div id="ControlSeleccionCromos" style="width:690px;margin: 0 auto;padding:5px;background-color:#FFF;" class="ui-corner-all">
<?php
				echo MensajeHtml::JqueryUiInfo('Si te sale una lámina nueva, se publicará este logro en Facebook. Más adelante podrás cambiar esto.');
				foreach($cromosSeleccion as $cromo)
				{
					$iv->AgregarAtributo(new AtributoXml('id', $cromo), true);
					echo $iv->ATexto(true, false);
				}
?>
            <div style="clear:both"></div>
          </div>
<?php
			}
			else
			{
				$album->LimpiarSeleccionCromo();
				echo MensajeHtml::JqueryUiError('No hay cromos que puedas seleccionar en este momento. Intenta más tarde');
			}
		}
		catch(Exception $e)
		{
			$album->LimpiarSeleccionCromo();
			echo MensajeHtml::JqueryUiError($e->getMessage());
		}
	}
	else
		echo MensajeHtml::JqueryUiError('¡No puedes reclamar cromos! Vuelve cuando tengas cromos que reclamar.');
}
else
	echo MensajeHtml::JqueryUiError('¡Ups! Parece que no tienes ningún álbum. Reclámalo, es gratis.');
?>
        </div>
<?php
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