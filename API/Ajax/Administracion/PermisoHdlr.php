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
if(isset($_REQUEST['p']) && isset($_REQUEST['u']))
{
	try
	{
		$usuario = new UsuarioFacebook($_REQUEST['u']);
		if(!$usuario->EsSuperUsuario)
		{
			if($usuario->TienePermiso($_REQUEST['p']))
			{
				$respuesta['EstadoDeseado'] = 'Activado';
				if(Permiso::OtorgadoSoloUsuario($usuario, $_REQUEST ['p']))
				{
					Permiso::Quitar($usuario, $_REQUEST['p']);
					$respuesta['EstadoDeseado'] = 'Desactivado';
					$respuesta['ejecucion'] = 'OK';
				}
				else
				$respuesta['MensajeError'] = 'Este permiso es dado a través de un grupo. Para retirar este permiso de este usuario debe eliminarlo del grupo o quitarle el permiso al grupo';
			}
			else
			{
				$respuesta['EstadoDeseado'] = 'Desactivado';
				Permiso::Otorgar($usuario, $_REQUEST['p']);
				$respuesta['EstadoDeseado'] = 'Activado';
				$respuesta['ejecucion'] = 'OK';
			}
		}
		else
		{
			$respuesta['EstadoDeseado'] = 'Activado';
			$respuesta['MensajeError'] = 'A un súper usuario esta gestión no le afecta';
		}
	}
	catch(Exception $e)
	{
		$respuesta['MensajeError'] = $e->getMessage();
	}
}
else
$respuesta['MensajeError'] = 'Usuario o permiso no especificado';
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>