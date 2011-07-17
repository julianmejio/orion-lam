<?php
require_once('../../../Facebook/src/base_facebook.php'); 
require_once('../../../Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once("../../../Orion/class.$Clase.inc");
}
function comparar($Cadena1, $Cadena2)
{
	return (obtenerComparacion($Cadena1, $Cadena2) + obtenerComparacion($Cadena2, $Cadena1)) / 2;
}
function compararBatch($Vinculo, $Vinculos)
{
	$similitud = 0;
	foreach($Vinculos as $vinculo)
	{
		$similar = comparar($Vinculo, $vinculo);
		if($similar > $similitud)
			$similitud = $similar;
	}
	return $similitud;
}
function obtenerComparacion($Cadena1, $Cadena2)
{
	$similitud = 0;
	$numeroComparaciones = 0;
	$sumatoriaComparaciones = 0;
	for($i = 0; $i < strlen($Cadena1) - 1; $i++)
		for($j = $i + 1; $j <= strlen($Cadena1); $j++)
		{
			$numeroComparaciones++;
			$cadenaComparable = substr($Cadena1, $i, $j - $i);
			if(strpos($Cadena2, $cadenaComparable) !== false)
				$sumatoriaComparaciones++;
		}
	if($numeroComparaciones >= 1)
		$similitud = $sumatoriaComparaciones / $numeroComparaciones;
	return $similitud;
}
function obtenerUrlEstatica($Url)
{
	$coincidencias = null;
	preg_match('/^https?:\/\/([^#]+)/i', $Url, $coincidencias);
	return $coincidencias[1];
}
function fechaATexto($Fecha)
{
	$fechaS = explode('-', $Fecha);
	$diaStr = intval($fechaS[2]);
	$mesStr = intval($fechaS[1]);
	switch($fechaS[1])
	{
		case 1:
			$mesStr = 'enero';
			break;
		case 2:
			$mesStr = 'febrero';
			break;
		case 3:
			$mesStr = 'marzo';
			break;
		case 4:
			$mesStr = 'abril';
			break;
		case 5:
			$mesStr = 'mayo';
			break;
		case 6:
			$mesStr = 'junio';
			break;
		case 7:
			$mesStr = 'julio';
			break;
		case 8:
			$mesStr = 'agosto';
			break;
		case 9:
			$mesStr = 'septiembre';
			break;
		case 10:
			$mesStr = 'octubre';
			break;
		case 11:
			$mesStr = 'noviembre';
			break;
		case 12:
			$mesStr = 'diciembre';
			break;
	}
	return "$diaStr de $mesStr";
}
$accessToken = '132461856821657|d3afcad244dd4b915e777f58.1-622814201|VmYp5lR12X6s1GBvfpmSn_DR3UQ';
$puntos = 1; // Puntos por buena publicación
$historial = time() - 15552000; // Punto límite de historia
$umbralMaximo = 0.7; // Similitud máxima permitida
$vinculos = array(); // Init
$vinculos2 = array(); // URL de seis meses
$peval = array(); // ID de posts ya evaluados
$idsreg = array(); // ID de usuarios que ya ganaron punto en el día actual
$usuarios = array(); // ID de usuarios registrados
$nIntegrantes = array(); // Integrantes que ganaron puntos.
$parametrosBd = ParametrosDb::CargarArchivo('../../../../Sistema/Datos/ConexionesDb/general.dbp');
$conexionBd = new ConectorBaseDatos($parametrosBd);
$conexionBd->Sentencia = 'SELECT DISTINCT IdUsuario FROM x_pub WHERE DAY(Fecha) = DAY(NOW())';
$did = $conexionBd->EjecutarConsulta();
if(is_array($did) && count($did) >= 1)
	foreach($did as $di)
		$idsreg[] = $di['IdUsuario'];
$conexionBd->Sentencia = sprintf("SELECT DISTINCT Comp FROM x_pub WHERE TStamp >= %s",
	$conexionBd->Escapar($historial)
);
$did = $conexionBd->EjecutarConsulta();
if(is_array($did) && count($did) >= 1)
	foreach($did as $di)
		$vinculos2[] = $di['Comp'];
$conexionBd->Sentencia = sprintf("SELECT IdF FROM x_pub WHERE TStamp >= %s",
	$conexionBd->Escapar($historial)
);
$did = $conexionBd->EjecutarConsulta();
if(is_array($did) && count($did) >= 1)
	foreach($did as $di)
		$peval[] = $di['IdF'];
$conexionBd->Sentencia = 'SELECT IdFacebook FROM UsuarioFacebook';
$did = $conexionBd->EjecutarConsulta();
if(is_array($did) && count($did) >= 1)
	foreach($did as $di)
		$usuarios[] = $di['IdFacebook'];
$datos = json_decode(file_get_contents("https://graph.facebook.com/376416784920/feed?access_token=$accessToken&limit=10"));
if(isset($datos->error) || $datos == null)
{
	$intentos = 4;
	$until = time() - 3600; 
	$datos = json_decode(file_get_contents("https://graph.facebook.com/376416784920/feed?access_token=$accessToken&limit=10&until=$until"));
	while((!isset($datos->data) || count($datos->data) <= 0) && $intentos > 0)
	{
		$intentos--;
		$until -= 3600;
		$datos = json_decode(file_get_contents("https://graph.facebook.com/376416784920/feed?access_token=$accessToken&limit=10&until=$until"));
	}
	if($intentos == 0 && ($data == null || !isset($datos->data) || count($datos->data) <= 0))
	{
		$limite = 9;
		$nuevosDatos = json_decode(file_get_contents("https://graph.facebook.com/376416784920/feed?access_token=$accessToken&limit=$limite"));
		while($limite > 0 && (!isset($datos->data) || count($datos->data) <= 0))
		{
			if(isset($nuevosDatos->data) && count($nuevosDatos->data) >= 1)
				$datos = $nuevosDatos;
			else
			{
				$limite--;
				$nuevosDatos = json_decode(file_get_contents("https://graph.facebook.com/376416784920/feed?access_token=$accessToken&limit=$limite"));				
			}
		}
	}	
}
if(isset($datos->data) && count($datos->data) >= 1)
{
	foreach($datos->data as $dato)
		if(
			isset($dato->link)
			&& in_array($dato->from->id, $usuarios)
			&& !in_array($dato->id, $peval)
			&& !preg_match('/mehiocore.com\/orionapp/', $dato->link)
			&& !in_array($dato->from->id, $idsreg)
		){
			$vinculos[] = array('Post' => $dato->id, 'Vinculo' => $dato->link, 'Titulo' => $dato->name, 'Integrante' => $dato->from, 'Comparable' => obtenerUrlEstatica(strtolower($dato->link)));
			$idsreg[] = $dato->from->id;
		}
}
for($i = count($vinculos) - 1; $i >= 0; $i--)
	if(compararBatch($vinculos[$i]['Comparable'], $vinculos2) <= $umbralMaximo)
	{
		$vinculos2[] = $vinculos[$i]['Comparable'];
		try
		{
			$conexionBd->Sentencia = sprintf("INSERT x_pub VALUES (%s, %s, %s, %s, %s)",
				$conexionBd->Escapar($vinculos[$i]['Post']),
				$conexionBd->Escapar($vinculos[$i]['Integrante']->id),
				$conexionBd->Escapar($vinculos[$i]['Comparable']),
				$conexionBd->Escapar(date("Y-m-d")),
				$conexionBd->Escapar(time())
			);
			$conexionBd->EjecutarComando();
			$usuario = new UsuarioFacebook($vinculos[$i]['Integrante']->id);
			$usuario->GestionarPuntos(new ItemPuntaje($puntos, 'Participación en el grupo público del Club Orión'));
			$nIntegrantes[] = $vinculos[$i]['Integrante'];
		}
		catch(Exception $e) {echo $e->getMessage();}
	}
$fechaBase = date("Y-m-d");
$participantes = false;
if(file_exists('participantes.dat'))
	$participantes = @unserialize(@file_get_contents('participantes.dat'));
if($participantes == false)
	$participantes = array(
		'Dia' => date("Y-m-d"),
		'Participantes' => array()
	);
if($participantes['Dia'] != $fechaBase && is_array($participantes['Participantes']) && count($participantes['Participantes']) >= 1)
{
	$usuario = new UsuarioFacebook('622814201');
	$post = array();
	$post['to'] = array();
	$post['to']['data'] = array();
	if(count($participantes['Participantes']) > 1)
	{
		$post['message'] = 'Los integrantes que ganaron puntos el ' . fechaATexto($participantes['Dia']) . ' fueron: ';
		for($i = 0; $i < count($participantes['Participantes']); $i++)
		{
			$post['message'] .= $participantes['Participantes'][$i]->name;
			$cc = '. ';
			if($i == count($participantes['Participantes']) - 2)
				$cc = ' y ';
			else if($i < count($participantes['Participantes']) - 2)
				$cc = ', ';
			$post['message'] .= $cc;
			$post['to']['data'][] = array('name' => $participantes['Participantes'][$i]->name, 'id' => $participantes['Participantes'][$i]->id);
		}
		$post['message'] .= 'Felicitaciones a ellos.';
	}
	else
	{
		$post['message'] = 'El integrante que ganó puntos el ' . fechaATexto($participantes['Dia']) . ' por haber participado en este grupo fue ' . $participantes['Participantes'][0]->name . '; Felicitaciones :)';
		$post['to']['data'][] = array('name' => $participantes['Participantes'][0]->name, 'id' => $participantes['Participantes'][0]->id);
	}
	$post['picture'] = 'http://mehiocore.com/orionapp/Imagenes/LogoPanel.png';
	$post['name'] = '¿Quieres participar en el Club Orión de Astronomía?';
	$post['caption'] = 'Aquí te enseñamos cómo:';
	$post['description'] = 'Ingresa a este vínculo y aprende cómo puedes participar tú también, ganar láminas astronómicas e intercambiar con otras personas que les gusta la astronomía, tus conocimientos… y tus láminas :)';
	$post['link'] = 'http://mehiocore.com/orionapp/Boletin/01-Participacion.php';
	$usuario->ApiAuth('/376416784920/feed', 'POST', $post);
	$participantes['Participantes'] = null;
	$participantes['Participantes'] = array();
	$participantes['Dia'] = $fechaBase;
}
else if(!is_array($participantes['Participantes']))
	$participantes['Participantes'] = array();
if(is_array($nIntegrantes) && count($nIntegrantes) >= 1)
	foreach($nIntegrantes as $nInt)
		$participantes['Participantes'][] = $nInt;
file_put_contents('participantes.dat', serialize($participantes));
?>