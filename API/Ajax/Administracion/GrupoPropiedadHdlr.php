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
if(isset($_REQUEST['g']) && isset($_REQUEST['a']))
{
	try
	{
		$grupo = Grupo::Cargar($_REQUEST['g']);
		switch($_REQUEST['a'])
		{
			case 'd':
				if($grupo->PorDefecto)
				{
					$respuesta['EstadoDeseado'] = 'Activado';
					$grupo->QuitarPredeterminado();
					$respuesta['ejecucion'] = 'OK';
					$respuesta['EstadoDeseado'] = 'Desactivado';
				}
				else
				{
					$respuesta['EstadoDeseado'] = 'Desactivado';
					$grupo->HacerPredeterminado();
					$respuesta['ejecucion'] = 'OK';
					$respuesta['EstadoDeseado'] = 'Activado';
				}
			break;
			case 'p':
				if($grupo->EsParticipativo)
				{
					$respuesta['EstadoDeseado'] = 'Activado';
					$grupo->QuitarParticipativo();
					$respuesta['ejecucion'] = 'OK';
					$respuesta['EstadoDeseado'] = 'Desactivado';
				}
				else
				{
					$respuesta['EstadoDeseado'] = 'Desactivado';
					$grupo->HacerPaticipativo();
					$respuesta['ejecucion'] = 'OK';
					$respuesta['EstadoDeseado'] = 'Activado';
				}
			break;
		}
	}
	catch(Exception $e)
	{
		$respuesta['MensajeError'] = $e->getMessage();
	}
}
else
$respuesta['MensajeError'] = 'No se especificó un grupo o una acción';
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>