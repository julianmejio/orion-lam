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

$seccionesAlbum = Lamina::ObtenerClasificaciones();
$seccionesLaminasNuevas = (is_array($albumes) && count($albumes) >= 1) ? $albumes[0]->ObtenerClasificacionLaminasNuevas() : null;
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Mi álbum - Club Orión de Astronomía</title>
<!-- InstanceEndEditable -->
<!-- InstanceBeginEditable name="head" -->
<link href="../../Estilos/General/Album.css" rel="stylesheet" type="text/css" />
<link href="../../Estilos/General/ItemsAlbum.css" rel="stylesheet" type="text/css" />
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
<link href="../../API/JqueryPlugs/JqueryLightbox/css/jquery.lightbox-0.5.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../Estilos/General/Album.js"></script>
<script type="text/javascript" src="../../API/JqueryPlugs/JqueryAutoEllipsis/jquery.autoellipsis-1.0.1.min.js"></script>
<script type="text/javascript" src="../../API/JqueryPlugs/JqueryLightbox/js/jquery.lightbox-0.5.min.js"></script>
<script type="text/javascript" src="../../API/JqueryTools/jquery.tools.min.js"></script>
<script type="text/javascript">
var contenedorMenu = null;
$(document).ready(function(){
	$(".WidgetAlbum .MenuAlbum .Menu").accordion({
		icons: {'header': 'ui-icon-bullet', 'headerSelected': 'ui-icon-radio-on'},
		autoHeight: false,
		change: function(event, ui) {
			contenedorMenu = ui.newContent;
			if($.support.leadingWhitespace)
			{
				cargarSeccion(ui.newHeader.context.childNodes[2].firstChild.data);
			}
			else
			{
				cargarSeccion(ui.newHeader.text());
			}
		}
	});
	cargarSeccion('<?php echo $seccionesAlbum[0]['Clasificacion']; ?>');
});

function cargarSeccion(Nombre)
{
	$(".barraS").progressbar({value : 0});
	$(".infoS").html("<span class=\"InfoCarga\">Cargando datos…</span>");
	$(".ContenidoAlbum .Contenido").html("<p style=\"text-align:center\"><span class=\"InfoCarga\">Espera mientras se carga la sección " + Nombre + "…</span></p>");
	$(".WidgetAlbum .MenuAlbum .Menu").accordion("disable");
	$.getJSON('<?php echo Config::RutaBase . '/Album/CargarSeccion.json'; ?>',
		{c: Nombre},
		function(data){
			if(data['Estado'] == 'OK')
			{
				$(".ContenidoAlbum .Contenido").html(data['ContenidoHtml']);
				prepararLamina();
				$(".barraS").progressbar({value: data['PorcentajeCompletado']});
				$(".infoS").html("Tienes " + data['LaminasSeccionObtenidas'] + " de " + data['TotalLaminasSeccion'] + " láminas.");
			}
			else
			{
				$(".ContenidoAlbum .Contenido").html(data['MensajeHtml']);
				$(".infoS").html("¡Error!");
			}
			$(".WidgetAlbum .MenuAlbum .Menu").accordion("enable");
		});
}

var menu = true;
function toggMenuAlbum()
{
	if(menu == true)
	{
		$(".WidgetAlbum .MenuAlbum").hide('slow',function(){
			$(".WidgetAlbum .ContenidoAlbum").animate({width: '98%'}, 1000);
			$("#toggMenu").html("» Menú");
		});
		menu = false;
	}
	else
	{
		$(".WidgetAlbum .ContenidoAlbum").animate({width: '77%'}, 1000, function(){
			$(".WidgetAlbum .MenuAlbum").show('slow');
			$("#toggMenu").html("« Menú");
		});
		menu = true;
	}
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
        <div class="WidgetAlbum">
          <div class="MenuAlbum"><?php
if($seccionesAlbum != null && is_array($seccionesAlbum))
{
	$hayLaminasNuevas = (is_array($seccionesLaminasNuevas));
	$aplClase = null;
?>
            <div class="Menu"><?php
foreach($seccionesAlbum as $seccion)
{
?>
              <h3>
<?php
if($hayLaminasNuevas && in_array($seccion['Clasificacion'], $seccionesLaminasNuevas))
	$aplClase = 'LaminaNueva';
?>
              <a href="javascript:void(0)" <?php if($aplClase != null) echo "class=\"$aplClase\" "; $aplClase = null; ?>><?php echo $seccion['Clasificacion']; ?></a>
              </h3>
              <div><div class="barraS" style="height: 0.5em"></div><div class="infoS" style="font-size:0.8em">&nbsp;</div></div>
<?php
} 
?>            </div><?php
}
else
	echo MensajeHtml::JqueryUiError('No hay secciones de álbum disponibles');
?>
          </div>
          <div class="ContenidoAlbum ui-corner-all">
            <div class="Acciones"><a id="toggMenu" href="javascript:void(0)" onclick="toggMenuAlbum()">« Menú</a></div>
            <div class="Contenido"></div>
          </div>
          <div style="clear:both"></div>
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