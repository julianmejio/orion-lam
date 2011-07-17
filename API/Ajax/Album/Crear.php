<?php
error_reporting(0);
require_once('../../Facebook/src/facebook.php');
require_once('../../Orion/class.Config.inc');
require_once('../../Orion/class.Uri.inc');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
$respuesta = array('Estado' => 'Error', 'Mensaje' => 'Error desconocido');
try
{
	Script::Cargar('VerificarSesion', true);
	$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
	$albumes = $usuarioActivo->ObtenerAlbumes();
	if($albumes == null || count($albumes) == 0)
	{
		$album = new Album();
		$album->Registrar($usuarioActivo);
		$respuesta['Estado'] = 'OK';
	}
	else
	$respuesta['Mensaje'] = 'Este usuario ya tiene registrado un álbum';
}
catch(Exception $e)
{
	$respuesta['Mensaje'] = $e->getMessage();
}
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>