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
if(isset($_REQUEST['c']))
{
	if(!isset($_SESSION))
	session_start();
	$usuario = null;
	$codigoLaminaSeleccionada = $_REQUEST['c'];
	try
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
	}
	catch(Exception $e)
	{
		$respuesta['Mensaje'] = $e->getMessage();
	}
	if(is_a($usuario, 'UsuarioFacebook'))
	{
		$album = $usuario->ObtenerAlbumes();
		if(is_array($album) && count($album) >= 1)
		{
			$album = $album[0];
			try
			{
				$laminaSeleccionada = $album->FinalizarSeleccionCromo($codigoLaminaSeleccionada);
				$objetoLamina = $laminaSeleccionada->Lamina->GenerarItemX();
				$respuesta['Estado'] = 'OK';
				$respuesta['LaminaHtml'] = $objetoLamina->ATexto(true, false);
				$respuesta['MensajeRetorno'] = '¡Has obtenido una lámina!';
				if($laminaSeleccionada->Cantidad == 1)
				{
					$respuesta['Repetida'] = false;
					switch($laminaSeleccionada->Lamina->Abundancia)
					{
						case 0:
						case 0.1:
						case 0.2:
							$respuesta['MensajeRetorno'] = '¡Esta lámina es única! Vaya la suerte que has tenido al conseguirla. Cuídala muy bien';
							break;
						case 0.3:
						case 0.4:
						case 0.5:
						case 0.6:
						case 0.7:
							$respuesta['MensajeRetorno'] = 'Has conseguido una lámina no tan fácil de encontrar. Muy bien';
							break;
						case 0.8:
						case 0.9:
						case 1:
							$respuesta['MensajeRetorno'] = 'Tienes una nueva lámina. Felicidades';
					}
				}
				else
				{
					$respuesta['Repetida'] = true;
					$respuesta['MensajeRetorno'] = 'Obtuviste una lámina, pero ya la habías obtenido anteriormente. Puedes cambiarla si lo deseas por otra que otros usuarios estén dispuestos a hacerlo';
				}
				$respuesta['DatosFbPost'] = array(
					'Nombre' => $laminaSeleccionada->Lamina->Nombre,
					'Clasificacion' => $laminaSeleccionada->Lamina->Clasificacion,
					'Descripcion' => $laminaSeleccionada->Lamina->Descripcion
				);
			}
			catch(Exception $e)
			{
				$respuesta['Mensaje'] = $e->getMessage();
			}
		}
		else
		$respuesta['Mensaje'] = 'No hay álbumes vinculados a esta cuenta';
	}
}
else
$respuesta['Mensaje'] = 'Se debe seleccionar una lámina para poder redimirla';
if($respuesta['Estado'] == 'Error' && isset($respuesta['Mensaje']))
$respuesta['MensajeHtml'] = MensajeHtml::JqueryUiError($respuesta['Mensaje']);
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>