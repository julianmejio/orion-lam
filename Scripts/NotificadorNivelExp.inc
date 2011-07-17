<?php
Script::Cargar('VerificarSesion', true);
$usuarioNotificacion = UsuarioFacebook::ObtenerUsuarioActivo();
if(!isset($_SESSION))
	session_start();
if($usuarioNotificacion->Experiencia->ProcesarBonificacion())
	$_SESSION['notificarNivel'] = true;
else if(isset($_SESSION['notificarNivel']))
{
	$_SESSION['notificarNivel'] = null;
	unset($_SESSION['notificarNivel']);
}
?>