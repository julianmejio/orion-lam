<?php
/**
 *
 * Contiene métodos y propiedades que permiten generar y descifrar códigos
 * numéricos.
 * @author Julián Mejio
 * @version 1.1
 *
 * @property int $Clave
 * @property int $Serial
 * @property int $Puntos
 */
class CifradorCodigo
{
	const LongitudBitsClaveMinimo  =  0x0;
	const LongitudBitsClaveMaximo  = 0x20;
	const LongitudBitsSerialMinimo =  0x0;
	const LongitudBitsSerialMaximo = 0x10;
	const LongitudBitsPuntos       =  0xC;
	const LongitudBitsCPuntos      =  0x4;
	const LongitudResumen          =  0x8;

	private $clave;
	private $serial;
	private $puntos;
	private $longitudClave;
	private $longitudSerial;
	private $claveDerivada;
	private $matrizPermutacion;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Clave':
				$valor = $this->clave;
				break;
			case 'LongitudClave':
				$valor = $this->longitudClave;
				break;
			case 'LongitudSerial':
				$valor = $this->longitudSerial;
				break;
			case 'Serial':
				$valor = $this->serial;
				break;
			case 'Puntos':
				$valor = $this->ObtenerPuntos();
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual");
		}
		return $valor;
	}

	function __set($Variable, $Valor)
	{
		switch($Variable)
		{
			case 'Clave':
				if(intval($Valor) == 0)
				throw new Exception('La clave es inválida');
				$longitudClave = CifradorCodigo::obtenerLongitudBits($Valor);
				if($longitudClave < CifradorCodigo::LongitudBitsClaveMinimo
				|| $longitudClave > CifradorCodigo::LongitudBitsClaveMaximo
				)
				throw new Exception('La clave no tiene la longitud requerida');
				$this->longitudClave = $longitudClave;
				$this->clave = $Valor;
				$this->derivarClave();
				break;
			case 'Serial':
				if(intval($Valor) == 0)
				throw new Exception('El serial es inválido');
				$longitudSerial = CifradorCodigo::obtenerLongitudBits($Valor);
				if($longitudSerial < CifradorCodigo::LongitudBitsSerialMinimo
				|| $longitudSerial > CifradorCodigo::LongitudBitsSerialMaximo
				)
				throw new Exception('El serial no tiene la longitud requerida');
				$this->longitudSerial = $longitudSerial;
				$this->serial = $Valor;
				break;
			case 'Puntos':
				if(intval($Valor) == 0)
				throw new Exception('Los puntos no son válidos');
				$longitudPuntos = CifradorCodigo::obtenerLongitudBits($Valor);
				if($Valor < 0
				|| $longitudPuntos > CifradorCodigo::LongitudBitsPuntos)
				throw new Exception('La cantidad de puntos no es válida');
				$this->puntos = CifradorCodigo::fijarPuntos($Valor);
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un cifrado de código de puntos.
	 * @param int $Clave
	 * @param int $Serial
	 * @param string $Puntos
	 */
	function __construct($Puntos, $Clave = null, $Serial = null)
	{
		if($Clave == null || !intval($Clave))
		$this->GenerarClave();
		else
		$this->Clave = intval($Clave);
		if($Serial == null || !intval($Serial))
		$this->GenerarSerial();
		else
		$this->Serial = intval($Serial);
		$this->Puntos = $Puntos;
		$this->llenarMatrizPermutacion();
		$this->derivarClave();
	}

	/**
	 *
	 * Llena la matriz de permutación.
	 */
	private function llenarMatrizPermutacion()
	{
		// Límite de redundancia estática: 5 ciclos.
		$this->matrizPermutacion = array(
		array('0101', '1010', '0111', '0110'),
		array('1011', '0000', '0011', '0010'),
		array('1101', '1100', '0001', '0100'),
		array('1001', '1000', '1111', '1110')
		);
	}

	/**
	 *
	 * Deriva cuatro cadenas de bits a a partir de la clave proporcionada.
	 */
	private function derivarClave()
	{
		$this->claveDerivada = array();
		$bitsClave = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsClaveMaximo, decbin($this->Clave));
		$xorClave = bindec(substr($bitsClave, 0, CifradorCodigo::LongitudBitsClaveMaximo / 2)) ^ bindec(substr($bitsClave, CifradorCodigo::LongitudBitsClaveMaximo / 2, CifradorCodigo::LongitudBitsClaveMaximo / 2));
		$xorClaveOriginal = $xorClave;
		for($i = 1; $i <= 6; $i++)
		{
			$clave = bindec($this->permutacionMultiple(CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsClaveMaximo / 2, decbin($xorClave >> 2))));
			$this->claveDerivada[] = $clave;
			$xorClave = $clave ^ $xorClave;
		}
	}

	/**
	 *
	 * Obtiene la permutación del serial.
	 */
	function PermutarSerial()
	{
		return $this->permutarDatos($this->Serial);
	}

	/**
	 *
	 * Obtiene la permutación de los puntos.
	 */
	function PermutarPuntos()
	{
		$puntosPermutados = $this->permutarDatos($this->puntos);
		$xorPuntos = $this->Serial ^ $puntosPermutados;
		return $xorPuntos;
	}

	/**
	 *
	 * Permuta una serie de datos con la longitud máxima del vector de inicialización.
	 * @param int $Datos
	 */
	private function permutarDatos($Datos)
	{
		$datosPermutados = null;
		$xorDatos = $Datos ^ $this->claveDerivada[0];
		$bitsXorDatos = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo, decbin($xorDatos));
		$hilosDatos = array();
		$hilosDatos[] = substr($bitsXorDatos, 0, CifradorCodigo::LongitudBitsSerialMaximo / 2);
		$hilosDatos[] = substr($bitsXorDatos, CifradorCodigo::LongitudBitsSerialMaximo / 2,
		CifradorCodigo::LongitudBitsSerialMaximo / 2);
		for($i = 1; $i <= 4; $i++)
		{
			if($i > 1)
			{
				$hilosDatos[0] = CifradorCodigo::desplazarBits($hilosDatos[0], 2, 'ltr');
				$hilosDatos[1] = CifradorCodigo::desplazarBits($hilosDatos[1], 2, 'ltr');
				if($i % 2 == 0)
				{
					$hilosDatos[0] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2,
					decbin(bindec($hilosDatos[0]) ^ bindec($hilosDatos[1])));
					$hilosDatos[1] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2,
					decbin(bindec($hilosDatos[1]) ^ bindec($this->claveDerivada[$i])));
				}
				else
				{
					$hilosDatos[1] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2,
					decbin(bindec($hilosDatos[0]) ^ bindec($hilosDatos[1])));
					$hilosDatos[0] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2,
					decbin(bindec($hilosDatos[0]) ^ bindec($this->claveDerivada[$i])));
				}
			}
			$hilosDatos[0] = $this->permutacionMultiple($hilosDatos[0]);
			$hilosDatos[1] = $this->permutacionMultiple($hilosDatos[1]);
		}
		$datosPermutados = "{$hilosDatos[0]}{$hilosDatos[1]}";
		return bindec($datosPermutados);
	}

	/**
	 *
	 * Invierte la permutación de datos.
	 * @param int $DatosPermutados
	 * @param int $Clave
	 */
	static private function invertirPermutacionDatos($DatosPermutados, $Clave)
	{
		$c = new CifradorCodigo(1, $Clave, null);
		$datosOriginales = null;
		$bitsDatos = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo, decbin($DatosPermutados));
		$hilosDatos = array(
		substr($bitsDatos, 0, CifradorCodigo::LongitudBitsSerialMaximo / 2),
		substr($bitsDatos, CifradorCodigo::LongitudBitsSerialMaximo / 2, CifradorCodigo::LongitudBitsSerialMaximo / 2)
		);
		for($i = 4; $i >= 1; $i--)
		{
			$hilosDatos[1] = $c->invertirPermutacionMultiple($hilosDatos [1]);
			$hilosDatos[0] = $c->invertirPermutacionMultiple($hilosDatos [0]);
			if($i > 1)
			{
				if($i % 2 == 0)
				{
					$hilosDatos[1] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2, decbin(bindec($hilosDatos[1]) ^ bindec($c->claveDerivada[$i])));
					$hilosDatos[0] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2, decbin(bindec($hilosDatos[0]) ^ bindec($hilosDatos[1])));
				}
				else
				{
					$hilosDatos[0] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2, decbin(bindec($hilosDatos[0]) ^ bindec($c->claveDerivada[$i])));
					$hilosDatos[1] = CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo / 2, decbin(bindec($hilosDatos[0]) ^ bindec($hilosDatos[1])));
				}
				$hilosDatos[1] = CifradorCodigo::desplazarBits($hilosDatos[1], 2, 'rtl');
				$hilosDatos[0] = CifradorCodigo::desplazarBits($hilosDatos[0], 2, 'rtl');
			}
		}
		$datosOriginales = bindec("{$hilosDatos[0]}{$hilosDatos[1]}");
		$rXorDatos = $datosOriginales ^ $c->claveDerivada[0];
		return $rXorDatos;
	}

	static function LeerCodigo($Codigo, $Clave)
	{
		if(!preg_match('/^[0-9A-F]{4}-[0-9A-F]{4}-[0-9A-F]{4}$/', $Codigo))
		throw new Exception('El código tiene un formato incorrecto');
		$codigo = explode('-', $Codigo);
		$serial = CifradorCodigo::invertirPermutacionDatos(hexdec($codigo[0]), $Clave);
		$puntos = CifradorCodigo::invertirPermutacionDatos((hexdec($codigo[1]) ^ $serial), $Clave);
		$c = new CifradorCodigo(1);
		$c->Clave = $Clave;
		$c->Serial = $serial;
		$c->puntos = $puntos;
		if($c->generarResumen() != hexdec($codigo[2]))
		throw new Exception('El código es inválido');
		return $c->ObtenerPuntos();
	}

	/**
	 *
	 * Genera un resumen de datos.
	 */
	function GenerarResumen()
	{
		$xorSerial = $this->Serial ^ $this->claveDerivada[0];
		$xorPuntos = $this->puntos ^ $this->claveDerivada[0];
		$datos = $this->permutarDatos($xorSerial);
		$datos = $datos ^ $this->permutarDatos($xorPuntos);
		return $datos;
	}

	function GenerarCodigo()
	{
		$bytesSerial = CifradorCodigo::convertirBytes(CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo, decbin($this->permutarSerial())));
		$bytesPuntos = CifradorCodigo::convertirBytes(CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo, decbin($this->permutarPuntos())));
		$bytesResumen = CifradorCodigo::convertirBytes(CifradorCodigo::fijarLongitud(CifradorCodigo::LongitudBitsSerialMaximo, decbin($this->generarResumen())));
		return "$bytesSerial[0]$bytesSerial[1]-$bytesPuntos[0]$bytesPuntos[1]-$bytesResumen[0]$bytesResumen[1]";
	}

	private static function convertirBytes($Bits)
	{
		if(!preg_match('/[01]+/', $Bits))
		throw new Exception('Los bits no son válidos');
		if(strlen($Bits) % 8 != 0)
		throw new Exception('La longitud de la cadena de bytes no es adecuada: ' . strlen($Bits));
		$bytes = array();
		for($i = 0; $i < strlen($Bits) / 8; $i++)
		$bytes[] = strtoupper(CifradorCodigo::fijarLongitud(2, dechex(bindec(substr($Bits, $i * 8, 8)))));
		return $bytes;
	}

	/**
	 *
	 * Realiza las permutaciones necesarias a un conjunto de bits.
	 * @param string $Bits
	 * @throws Exception Se arroja cuando no se cumple con la regla de permutación múltiple.
	 * @return string
	 */
	private function permutacionMultiple($Bits)
	{
		$resultado = '';
		if(strlen($Bits) % 4 != 0)
		throw new Exception('No se puede realizar la permutación');
		$secs = strlen($Bits) / 4;
		for($i = 0; $i < $secs; $i++)
		$resultado .= $this->permutar(substr($Bits, $i * 4, 4));
		return $resultado;
	}

	/**
	 *
	 * Realiza las permutaciones invertidas necesarias a un conjunto de bits.
	 * @param string $Bits
	 * @throws Exception Se arroja cuando no se cumple con la regla de permutación múltiple.
	 * @return string
	 */
	private function invertirPermutacionMultiple($Bits)
	{
		$resultado = '';
		if(strlen($Bits) % 4 != 0)
		throw new Exception('No se puede realizar la permutación');
		$secs = strlen($Bits) / 4;
		for($i = 0; $i < $secs; $i++)
		$resultado .= $this->invertirPermutacion(substr($Bits, $i * 4, 4));
		return $resultado;
	}

	/**
	 *
	 * Obtiene la permutación de dos pares de bytes.
	 * @param string $Bits
	 * @return string
	 */
	private function permutar($Bits)
	{
		if(strlen($Bits) != 4)
		throw new Exception('La permutación sólo puede realizar con cuatro bits');
		$f = bindec(substr($Bits, 2, 2));
		$c = bindec(substr($Bits, 0, 2));
		return $this->matrizPermutacion[$f][$c];
	}

	/**
	 *
	 * Invierte la permutación
	 * @param string $Permutacion
	 */
	private function invertirPermutacion($Permutacion)
	{
		$permutacion = null;
		for($f = 0; $f < count($this->matrizPermutacion); $f++)
		for($c = 0; $c < count($this->matrizPermutacion[$f]); $c++)
		if($this->matrizPermutacion[$f][$c] == $Permutacion)
		{
			$permutacion = CifradorCodigo::fijarLongitud(2, decbin($c)) . CifradorCodigo::fijarLongitud(2, decbin($f));
			break;
		}
		return $permutacion;
	}

	/**
	 *
	 * Genera un número aleatorio con cierta cantidad de bits
	 * @param int $CantidadBits
	 * @return int
	 */
	private static function generarNumero($CantidadBits)
	{
		$numeroGenerado = '';
		while(strlen($numeroGenerado) < $CantidadBits)
		$numeroGenerado .= rand(0, 1);
		return bindec($numeroGenerado);
	}

	/**
	 *
	 * Genera una clave y la almacena en el objeto.
	 */
	function GenerarClave()
	{
		$this->Clave = CifradorCodigo::generarNumero(
		CifradorCodigo::LongitudBitsClaveMaximo
		);
	}

	/**
	 *
	 * Genera un serial y lo almacena en el objeto.
	 */
	function GenerarSerial()
	{
		$this->Serial = CifradorCodigo::generarNumero(
		CifradorCodigo::LongitudBitsSerialMaximo
		);
	}

	/**
	 *
	 * Obtiene la longitus en bits de un número.
	 * @param int $Numero
	 */
	private static function obtenerLongitudBits($Numero)
	{
		return strlen(decbin($Numero));
	}

	/**
	 *
	 * Fija los puntos en un número que tiene la longitud en bits requerida.
	 * @param int $Puntos
	 */
	private static function fijarPuntos($Puntos)
	{
		$longitudPuntosBits = CifradorCodigo::obtenerLongitudBits($Puntos);
		$puntosBin = decbin($Puntos);
		while(strlen($puntosBin) < CifradorCodigo::LongitudBitsPuntos)
		$puntosBin = rand(0, 1) . $puntosBin;
		$puntosBin = decbin($longitudPuntosBits) . $puntosBin;
		return bindec($puntosBin);
	}
	
	/**
	 * 
	 * Redime los puntos de un código de puntos válido a un usuario.
	 * @param UsuarioFacebook $Usuario
	 * @param string $Codigo
	 * @param int $Clave
	 * @throws Exception Se arroja cuando el códgio es incorrecto o ya fue usado.
	 */
	static function RedimirPuntos(UsuarioFacebook $Usuario, $Codigo, $Clave = null)
	{
		$hashCodigosReclamados = array();
		$ConexionBd = new ConectorBaseDatos();
		$datosCodigos = $ConexionBd->EjecutarConsulta('SELECT HashCodigo FROM CodigoReclamado');
		if(is_array($datosCodigos) && count($datosCodigos) >= 1)
			foreach($datosCodigos as $datosCodigo)
				$hashCodigosReclamados[] = $datosCodigo['HashCodigo'];
		$datosCodigos = null;
		$hashCodigo = sha1($Codigo);
		if(in_array($hashCodigo, $hashCodigosReclamados))
			throw new Exception('Este código ya ha sido reclamado');
		if($Clave == null)
			$Clave = Config::ClaveCifradorCodigo;
		$puntos = CifradorCodigo::LeerCodigo($Codigo, $Clave);
		$ConexionBd->Sentencia = sprintf("INSERT CodigoReclamado VALUES (%s, %s, %s, %s)",
			$ConexionBd->Escapar($Usuario->IdFacebook),
			$ConexionBd->Escapar($hashCodigo),
			$ConexionBd->Escapar(date("Y-m-d", time())),
			$ConexionBd->Escapar($puntos)
		);
		$ConexionBd->EjecutarComando();
		$ConexionBd->Desconectar();
		$Usuario->GestionarPuntos(new ItemPuntaje($puntos, 'Redención de código de puntos'));
	}

	/**
	 *
	 * Obtiene los puntos que han sido almacenados en el objeto.
	 * @return int
	 */
	function ObtenerPuntos()
	{
		$puntosBin = CifradorCodigo::fijarLongitud(
		CifradorCodigo::LongitudBitsPuntos
		+ CifradorCodigo::LongitudBitsCPuntos
		,decbin($this->puntos)
		);
		$longitudPuntos = bindec(substr($puntosBin, 0, 4));
		$puntos = bindec(
		substr(
		$puntosBin,
		CifradorCodigo::LongitudBitsPuntos
		+ CifradorCodigo::LongitudBitsCPuntos
		- $longitudPuntos,
		$longitudPuntos
		));
		return intval($puntos);
	}

	/**
	 *
	 * Fija la longitud de un valor con un relleno.
	 * @param int $Cantidad
	 * @param mixed $Valor
	 * @param string $Relleno
	 */
	private static function fijarLongitud($Cantidad, $Valor, $Relleno = '0')
	{
		while(strlen($Valor) < $Cantidad)
		$Valor = "$Relleno$Valor";
		return $Valor;
	}

	/**
	 *
	 * Desplaza una cadena de bits {@param $Posicion} posiciones en una dirección {@param $Direccion}.
	 * @param string $Bits
	 * @param int $Posicion
	 * @param string $Direccion
	 * @throws Exception Se arroja cuando la cadena de bits en inválida o no se puede desplazar la cadena de bits.
	 * @return string
	 */
	private static function desplazarBits($Bits, $Posicion, $Direccion)
	{
		$bitsDesplazados = null;
		if(!preg_match('/[01]+/', $Bits))
		throw new Exception('La cadena de bits no es válida');
		$posicionFija = $Posicion % strlen($Bits);
		if($Direccion == 'ltr')
		$posicionFija = strlen($Bits) - $posicionFija;
		$bitsDesplazados = substr($Bits, $posicionFija) . substr($Bits, 0, $posicionFija);
		if($bitsDesplazados == null)
		throw new Exception('No se pudo despalzar la cadena de bits');
		return $bitsDesplazados;
	}
}
/*
 set_time_limit(600);
 $clave = 56917;
 $codGen = new CifradorCodigo(1, 56917);
 $puntos = array(
 array(
 'Puntos' => 3,
 'Valor' => 0,
 'Cantidad' => 10
 ),
 array(
 'Puntos' => 5,
 'Valor' => 0,
 'Cantidad' => 10
 ),
 array(
 'Puntos' => 10,
 'Valor' => 0,
 'Cantidad' => 1
 ),
 array(
 'Puntos' => 5,
 'Valor' => 500,
 'Cantidad' => 20
 ),
 array(
 'Puntos' => 10,
 'Valor' => 100 * 10 * 0.95,
 'Cantidad' => 10
 ),
 array(
 'Puntos' => 15,
 'Valor' => 100 * 15 * 0.9,
 'Cantidad' => 10
 ),
 array(
 'Puntos' => 20,
 'Valor' => 100 * 20 * 0.85,
 'Cantidad' => 5
 ),
 array(
 'Puntos' => 50,
 'Valor' => 100 * 50 * 0.8,
 'Cantidad' => 5
 ),
 array(
 'Puntos' => 100,
 'Valor' => 100 * 100 * 0.75,
 'Cantidad' => 2
 ),
 array(
 'Puntos' => 500,
 'Valor' => 100 * 500 * 0.7,
 'Cantidad' => 2
 ),
 array(
 'Puntos' => 1000,
 'Valor' => 100 * 1000 * 0.65,
 'Cantidad' => 1
 ),
 array(
 'Puntos' => 2000,
 'Valor' => 100 * 2000 * 0.6,
 'Cantidad' => 0
 ),
 array(
 'Puntos' => 4000,
 'Valor' => 100 * 4000 * 0.55,
 'Cantidad' => 1
 )
 );
 $lPuntos = 0;
 $lValor = 0;
 $lCodigo = 0;
 $arrCtnd = array();
 $a = fopen("Codigos.xml", "w");
 fwrite($a, '<?xml version="1.0" encoding="utf-8"?>');
 fwrite($a, '<codigos>');
 foreach($puntos as $punto)
 for($i = 1; $i <= $punto['Cantidad']; $i++)
 {
 fwrite($a, '<codigo>');
 $codGen->Puntos = $punto['Puntos'];
 $codGen->GenerarSerial();
 fwrite($a, "<puntos>{$punto['Puntos']}</puntos>");
 fwrite($a, "<valor>{$punto['Valor']}</valor>");
 fwrite($a, "<codigox>" . $codGen->generarCodigo() . "</codigox>");
 fwrite($a, '</codigo>');
 }
 $arrP = array();
 fwrite($a, "</codigos>");
 fclose($a);
 do
 {
 echo "Codigo: ";
 $i = fopen("php://stdin", "r");
 $cod = fgets($i);
 if($cod != "")
 {
 try
 {
 if(in_array($cod, $arrP))
 throw new Exception('Código ya reclamado :(');
 $puntos = CifradorCodigo::leerCodigo($cod, $clave);
 echo "Redimiste $puntos puntos :)\r\n";
 $arrP[] = $cod;
 }
 catch(Exception $e)
 {
 echo 'Error: ' . $e->getMessage() . "\r\n";
 }
 }
 echo "\r\n";
 }while($cod != "");
 */
?>