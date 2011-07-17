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
	$usuario = null;
	try
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
	}
	catch(Exception $e) {}
	if($usuario != null)
	{
		try
		{
			$usuarios = UsuarioFacebook::EncontrarUsuarios($_REQUEST['term']);
			foreach($usuarios as $usuario)
			{
				$itemX = $usuario->GenerarItemXInLine();
				$respuesta[] = array('Id' => $usuario->IdFacebook, 'Presentacion' => $itemX->ATexto(true, false));
			}
		}
		catch(Exception $e) {}
	}
}
header('Content-Type: text/plain');
//header('Content-Disposition: attachment; filename="respuesta.json"');
header("Content-Disposition: inline");
echo json_encode($respuesta);
?>