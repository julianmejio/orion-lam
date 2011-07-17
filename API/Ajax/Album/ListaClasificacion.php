<?php
error_reporting(0);
require_once('../../Facebook/src/facebook.php');
require_once('../../Orion/class.Config.inc');
require_once('../../Orion/class.Uri.inc');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
$respuesta = array();
if(isset($_REQUEST['term']))
{
	try
	{
		$respuesta = Lamina::FiltrarClasificaciones($_REQUEST['term']);
	}
	catch(Exception $e) {}
}
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>