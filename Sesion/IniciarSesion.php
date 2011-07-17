<?php
require_once('../API/Orion/class.Config.inc');
require_once('../API/Orion/class.Uri.inc');
require_once('../API/Facebook/src/facebook.php');
require_once('../API/Geoip/geoip.inc');
require_once('../API/Geoip/geoipcity.inc');
require_once('../API/Geoip/geoipregionvars.php');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
$usuario = null;
try
{
	$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
}
catch(Exception $e)
{
	$errorRegistro = $e->getMessage();
}
if($usuario != null)
{
	$usuario->ActualizarDatosFacebook();
	if(isset($_REQUEST['r']))
		header("Location: {$_REQUEST['r']}");
	else
		header("Location: ../Panel");
	exit;
}
else
{
	$fb = new Facebook(FacebookConfig::ObtenerConfiguracionAplicacion());
	$urlLogin = $fb->getLoginUrl(array('scope' => 'user_birthday,user_groups,user_location,user_status,email,publish_stream,offline_access'));
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>Club Orión de Astronomía - Iniciar Sesión</title>
<style type="text/css">
img {
	border: none;
}
body {
	font-family: "Segoe UI", Verdana, Geneva, sans-serif;
	font-size: 1em;
	font-style: normal;
	line-height: normal;
	font-weight: normal;
	font-variant: normal;
	text-transform: none;
	color: #FFF;
	background-attachment: scroll;
	background-color: #000;
	background-image: url(../Imagenes/Fondo.jpg);
	background-repeat: no-repeat;
	background-position: center top;
	margin: 0px;
	padding: 50px;
}
#Cabecera {
	text-align: center;
}
#Controles {
	margin-top: 50px;
}
#Controles #ControlSesion #ControlSesionFacebook {
	width: 177px;
	height: 62px;
	padding: 20px;
	margin-right: auto;
	margin-left: auto;
}
</style>
<link href="../API/Jqueryui/css/orion/jquery-ui-1.8.9.custom.css" rel="stylesheet" type="text/css" />
</head>

<body>
<div id="Cabecera"> <img id="LogoPrincipal" src="../Imagenes/LogoEntrada.png" width="406" height="200" alt="Logo Club Orión de Astronomía" longdesc="http://www.orion-astronomia.info" /> </div>
<div id="Controles">
  <div id="ControlSesion" class="ui-widget">
    <div id="ControlSesionFacebook" class="ui-state-default ui-corner-all">
      <?php
if(isset($errorRegistro))
	echo MensajeHtml::JqueryUiError($errorRegistro);
else
{
?>
      <a href="<?php echo $urlLogin?>"><img src="../Imagenes/BotonLoginFb.png" width="177" height="62" alt="Iniciar sesión en Facebook" /></a>
      <?php
}
?>
    </div>
  </div>
</div>
</body>
</html>