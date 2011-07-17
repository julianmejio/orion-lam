<?php
require_once('../../Orion/class.Config.inc');
require_once('../../Orion/class.Uri.inc');
require_once('../../Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once ("../../Orion/class.$Clase.inc");
}
$respuesta = array('Estado' => 'Error', 'Mensaje' => 'Error desconocido');
if(isset($_REQUEST['g']))
{
	try
	{
		$grupo = Grupo::Cargar($_REQUEST['g']);
		$grupo->Eliminar();
		$respuesta['Estado'] = 'OK';
	}
	catch(Exception $e)
	{
		$respuesta['Mensaje'] = $e->getMessage();
	}
}
else
$respuesta['Mensaje'] = 'No se especificó el grupo a eliminar';
header('Content-Type: application/json');
echo json_encode($respuesta);
?>