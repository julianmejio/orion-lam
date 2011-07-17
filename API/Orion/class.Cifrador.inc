<?php
/**
 *
 * Provee una interfaz para el cifrado y descifrado de datos.
 * @author Julián Mejio
 * @version 1.0
 */
class Cifrador
{
	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual");
		}
		return $valor;
	}

	function __set($Variable, $Valor)
	{
		switch($Variable)
		{
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Cifra una cadena con la clave proporcionada.
	 * @param string $Datos
	 * @param string $Clave
	 * @return string
	 */
	static function Cifrar($Datos, $Clave = null)
	{
		if($Clave == null)
		$Clave = Config::ClaveSitio;
		$paquete = array('IV' => null, 'Contenido' => null);
		$paquete['IV'] = mcrypt_create_iv(mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_CBC), MCRYPT_RAND);
		$paquete['Contenido'] = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $Clave, $Datos, MCRYPT_MODE_CBC, $paquete['IV']);
		return base64_encode(gzcompress(serialize($paquete)));
	}

	/**
	 *
	 * Descifra una cadena con la clave proporcionada.
	 * @param string $Datos
	 * @param string $Clave
	 * @return string
	 */
	static function Descifrar($Datos, $Clave = null)
	{
		if($Clave == null)
		$Clave = Config::ClaveSitio;
		$paquete = unserialize(gzuncompress(base64_decode($Datos)));
		$datos = rtrim(mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $Clave, $paquete['Contenido'], MCRYPT_MODE_CBC, $paquete['IV']), "\0");
		return $datos;
	}
}
?>