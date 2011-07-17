<?php
//error_reporting(0);
require_once('../../Facebook/src/facebook.php');
require_once('../../Orion/class.Config.inc');
require_once('../../Orion/class.Uri.inc');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
$respuesta = array('Estado' => 'Error', 'Mensaje' => 'Error desconocido');
$errorLogico = false;
if(!isset($_SESSION))
session_start();
try
{
	$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
}
catch (Exception $e)
{
	$respuesta['Mensaje'] = $e->getMessage();
	$errorLogico = true;
}
if($usuarioActivo != null && !$errorLogico)
{
	$seccion = null;
	if(isset($_REQUEST['c']))
	$seccion = $_REQUEST['c'];
	if($seccion != null)
	{
		try
		{
			$albumes = $usuarioActivo->ObtenerAlbumes();
		}
		catch(Exception $e)
		{
			$respuesta['Mensaje'] = $e->getMessage();
			$errorLogico = true;
		}
		if($albumes != null && !$errorLogico)
		{
			$album = $albumes[0];
			try
			{
				$laminas = Lamina::ObtenerLaminasPorClasificacion($seccion);
			}
			catch(Exception $e)
			{
				$respuesta['Mensaje'] = $e->getMessage();
				$errorLogico = true;
			}
			if($laminas != null && !$errorLogico)
			{
				$laminasObtenidas = 0;
				$htmlSeccion = '';
				$laminaVacia = Lamina::GenerarItemVacioX();
				foreach($laminas as $lamina)
				{
					$cantidad = 0;
					$existe = false;
					foreach($album->Cromos as $cromo)
					if($cromo->Lamina->Id == $lamina->Id && $cromo->Cantidad >= 1)
					{
						$cantidad = $cromo->Cantidad;
						$laminasObtenidas++;
						break;
					}
					if($cantidad >= 1)
					{
						$laminaX = $lamina->GenerarItemX();
						if($cantidad >= 2)
						{
							$propiedadCantidad = new EtiquetaXml('div');
							$propiedadCantidad->AgregarAtributo(new AtributoXml('class', 'InformacionCantidadLamina ui-corner-all'));
							$propiedadCantidad->AgregarContenido("× $cantidad");
							$laminaX->AgregarContenido($propiedadCantidad);
						}
						$htmlSeccion .= $laminaX->ATexto(true, false);
					}
					else
					$htmlSeccion .= $laminaVacia->ATexto(true, false);
				}
				$respuesta['Estado'] = 'OK';
				$respuesta['ContenidoHtml'] = $htmlSeccion;
				$respuesta['Mensaje'] = 'Construcción HTML correcta';
				$respuesta['PorcentajeCompletado'] = round(($laminasObtenidas / count($laminas)) * 100, 2);
				$respuesta['TotalLaminasSeccion'] = count($laminas);
				$respuesta['LaminasSeccionObtenidas'] = $laminasObtenidas;
			}
			else
			$respuesta['Mensaje'] = "No hay láminas bajo la sección $seccion";
		}
		else
		$respuesta['Mensaje'] = 'No tienes álbumes. Deberías registrar uno a tu nombre';
	}
	else
	$respuesta['Mensaje'] = 'No se especificó una sección para cargar';
}
else
$respuesta['Mensaje'] = 'No hay usuario activo';
if($respuesta['Estado'] == 'Error' && isset($respuesta['Mensaje']))
$respuesta['MensajeHtml'] = MensajeHtml::JqueryUiError($respuesta['Mensaje']);
header('Content-Type: application/json');
header('Content-Disposition: attachment; filename="respuesta.json"');
echo json_encode($respuesta);
?>