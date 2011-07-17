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
if(isset($_REQUEST['g']) && isset($_REQUEST['p']))
{
	try
	{
		$grupo = Grupo::Cargar($_REQUEST['g']);
		if(Permiso::OtorgadoSoloGrupo($grupo, $_REQUEST['p']))
		{
			$respuesta['EstadoDeseado'] = 'Activado';
			Permiso::Quitar($grupo, $_REQUEST['p']);
			$respuesta['ejecucion'] = 'OK';
			$respuesta['EstadoDeseado'] = 'Desactivado';
		}
		else
		{
			$respuesta['EstadoDeseado'] = 'Desactivado';
			Permiso::Otorgar($grupo, $_REQUEST['p']);
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
$respuesta['MensajeError'] = 'No se especificó el grupo o el permiso';
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>