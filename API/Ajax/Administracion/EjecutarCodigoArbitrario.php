<?php
//error_reporting(0);
require_once('../../Orion/class.Config.inc');
require_once('../../Orion/class.Uri.inc');
require_once('../../Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once ("../../Orion/class.$Clase.inc");
}
function flog($Mensaje)
{
	echo "$Mensaje\r\n";
}
$respuesta = array('Estado' => 'Error', 'Mensaje' => 'Error desconocido');
//if(isset($_SERVER['x-requested-with']) && $_SERVER['x-requested-with'] == 'XMLHttpRequest')
//{
$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
if($usuarioActivo != null)
{
	if($usuarioActivo->TienePermiso('UsuarioBajoNivel'))
	{
		$codigoArbitrario = null;
		if(isset($_POST['CodigoArbitrario']))
		$codigoArbitrario = $_POST['CodigoArbitrario'];
		if($codigoArbitrario != null)
		{
			$prRespuesta = null;
			ob_start();
			flog('CONSOLA DE BAJO NIVEL');
			flog('=====================');
			flog('Bienvenido a la consola de ejecución.');
			flog('Hora del servidor: ' . date("Y-m-d H:i:s"));
			flog("Usuario: {$usuarioActivo->DatosFacebook['name']}");
			flog('Nota: todo el proceso será guardado, incluyendo la respuesta');
			flog('      Utilice flog() para imprimir mensaje en vez de print() o echo()');
			flog('');
			flog('*** INICIO DE EJECUCIÓN DE CÓDIGO ARBITRARIO ***');
			flog('');
			try
			{
				eval($codigoArbitrario);
			}
			catch(Exception $e)
			{
				flog('Error: ' . $e->getMessage());
			}
			flog('');
			flog('*** FIN DE EJECUCIÓN DE CÓDIGO ARBITRARIO ***');
			flog('Hora de finalización: ' . date("Y-m-d H:i:s"));
			$prRespuesta = ob_get_contents();
			ob_end_clean();
			$respuesta['Estado'] = 'OK';
			$respuesta['Mensaje'] = $prRespuesta;
		}
		else
		$respuesta['Mensaje'] = 'No hay código arbitrario para ejecutar';
	}
	else
	$respuesta['Mensaje'] = 'No tiene privilegios para ejecutar código arbitrario';
}
else
$respuesta['Mensaje'] = 'No hay usuario activo';
//}
//else
//	$respuesta['Mensaje'] = 'Sólo se puede llamar a esta rutina por medio de invocaciones AJAX';
header('Content-Type: application/json');
echo json_encode($respuesta);
?>