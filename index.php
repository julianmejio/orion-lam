<?php
require_once("API/Orion/class.Config.inc");
require_once("API/Orion/class.Uri.inc");
require_once("API/Facebook/src/facebook.php");
function __autoload($Clase)
{
	require_once("API/Orion/class.$Clase.inc");
}
if(!isset($_SESSION))
	session_start();
$usuario = null;
try
{
	$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
}
catch(Exception $e)
{
	header('Content-Type: text/plain');
	echo 'Error: ' . $e->getMessage();
	exit;
}
if($usuario != null)
{
	if(!headers_sent())
		header('Location: ' . Config::RutaBase . '/Panel');
	exit;
}
else
{
	if(!headers_sent())
		header('Location: ' . Config::RutaBase . '/Sesion/IniciarSesion.php');
	exit;
}
?>