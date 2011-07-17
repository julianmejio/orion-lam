<?php
require_once('../API/Orion/class.Config.inc');
require_once('../API/Orion/class.Uri.inc');
require_once('../API/Facebook/src/facebook.php');
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
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- TemplateBeginEditable name="doctitle" -->
<title>Club Orión de Astronomía</title>
<!-- TemplateEndEditable -->
<!-- TemplateBeginEditable name="head" -->
<!-- TemplateEndEditable -->
<link href="../Estilos/General/ItemsFacebook.css" rel="stylesheet" type="text/css" />
<link href="../Estilos/General/Nonometro.css" rel="stylesheet" type="text/css" />
<link href="../Estilos/General/General.css" rel="stylesheet" type="text/css" />
<link href="../API/Jqueryui/css/orion/jquery-ui-1.8.9.custom.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../API/Jquery/jquery-1.6.1.min.js"></script>
<script type="text/javascript" src="../API/Jqueryui/js/jquery-ui-1.8.9.custom.min.js"></script>
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
<!-- TemplateBeginEditable name="posthead" -->
<!-- TemplateEndEditable -->
</head>

<body>
<div id="ContenedorPanel">
  <div id="Panel" class="ui-corner-all">
    <div id="CabeceraPanel">
      <div id="ContenedorImagenLogo"><img src="../Imagenes/LogoPanel.png" width="202" height="100" alt="Logo del CLub Orión de Astronomía" /></div>
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
              <li><a href="../Panel/Album">Álbum</a></li>
            </ul>
          </div>
          <h2><a href="javascript:void(0)">Club</a></h2>
          <div>
            <ul>
              <li><a href="../Panel/Grupo/Lista.php">Grupos</a></li>
<?php
if($usuarioActivo->TienePermiso('RegistrarActas'))
{
?>
              <li><a href="../Panel/Acta/Crear.php">Registrar acta</a></li>
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
          <li><a href="../Panel/Administracion/Grupo/">Grupos</a></li>
<?php
	}
?>
          <li><a href="../Panel/Administracion/Usuario/">Usuarios</a></li>
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
            <li><a href="../Sesion/CerrarSesion.php">Cerrar sesión</a></li>
          </ul>
          <div style="clear:both"></div>
        </div>
        <!-- TemplateBeginEditable name="ContenidoPanel" -->Contenido<!-- TemplateEndEditable --></div>
      <div style="clear:both"></div>
    </div>
  </div>
</div>
</body>
</html>