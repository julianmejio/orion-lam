<?php
error_reporting(0);
require_once('../../Facebook/src/facebook.php');
require_once('../../Orion/class.Config.inc');
require_once('../../Orion/class.Uri.inc');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
$respuesta = array('ejecucion' => 'Error', 'MensajeError' => 'Error desconocido');
if(isset($_REQUEST['u']))
{
	try
	{
		$usuario = new UsuarioFacebook($_REQUEST['u']);
		if($usuario->EsSuperUsuario)
		{
			$respuesta['EstadoDeseado'] = 'Activado';
			Permiso::QuitarSuperUsuario($usuario);
			$respuesta['ejecucion'] = 'OK';
			$respuesta['EstadoDeseado'] = 'Desactivado';
		}
		else
		{
			$respuesta['EstadoDeseado'] = 'Desactivado';
			Permiso::OtorgarSuperUsuario($usuario);
			$respuesta['ejecucion'] = 'OK';
			$respuesta['EstadoDeseado'] = 'Activado';
		}
	}
	catch(Exception $e)
	{
		$respuesta['MensajeError'] = $e->getMessage();
	}
}
else
$respuesta['MensajeError'] = 'Usuario no especificado';
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>