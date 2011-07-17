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
//
if(isset($_REQUEST['Usuario']) && isset($_REQUEST['Grupo']) && isset($_REQUEST['Accion']))
{
	$usuarioActivo = null;
	try
	{
		$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
		$usuarioGrupo = new UsuarioFacebook($_REQUEST['Usuario']);
		$grupo = Grupo::Cargar($_REQUEST['Grupo']);
		switch($_REQUEST['Accion'])
		{
			case 'Pertenencia':
				if($grupo->EsMiembro($usuarioGrupo))
				{
					$grupo->EliminarUsuario($usuarioGrupo);
					$respuesta['ejecucion'] = 'OK';
					$respuesta['EstadoDeseado'] = 'Desactivado';
				}
				else
				{
					$grupo->RegistrarUsuario($usuarioGrupo);
					$respuesta['ejecucion'] = 'OK';
					$repuesta['EstadoDeseado'] = 'Activado';
				}
				break;
			case 'Administracion':
				if($grupo->EsAdministrador($usuarioGrupo))
				{
					$grupo->QuitarAdministracion($usuarioGrupo);
					$respuesta['ejecucion'] = 'OK';
					$respuesta['EstadoDeseado'] = 'Desactivado';
				}
				else
				{
					$grupo->HacerAdministrador($usuarioGrupo);
					$respuesta['ejecucion'] = 'OK';
					$respuesta['EstadoDeseado'] = 'Activado';
				}
				break;
			default:
				$respuesta['MensajeError'] = 'La acción solicitada no es válida';
				break;
		}
	}
	catch(Exception $e)
	{
		$respuesta['MensajeError'] = $e->getMessage();
	}

}
else
$respuesta['MensajeError'] = 'No se proporcionaron datos para la operación';
//
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>