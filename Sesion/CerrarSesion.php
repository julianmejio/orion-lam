<?php
require_once('../API/Orion/class.Config.inc');
require_once('../API/Orion/class.Uri.inc');
require_once('../API/Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
if($usuario != null && !isset($_REQUEST['access_token']))
{
	$fb = new Facebook(FacebookConfig::ObtenerConfiguracionAplicacion());
	$logout = $fb->getLogoutUrl();
	session_unset();
	session_destroy();
	header("Location: $logout");
	exit;
}
else
{
	header("Location: IniciarSesion.php");
	exit;
}
?>