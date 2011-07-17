<?php
$usuarioActivo = null;
try
{
	$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
}
catch(Exception $e) {}
if($usuarioActivo == null)
{
	if(!headers_sent())
		header('Location: ' . Config::RutaBase . '/Sesion/IniciarSesion.php?r=' . $_SERVER['REQUEST_URI']);
	exit;
}
?>