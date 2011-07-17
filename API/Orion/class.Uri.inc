<?php
/**
 * Provee métodos de manipulación de Uris.
 *
 * @author Julián Mejio
 * @version 1.6
 */
class Uri {

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			default:
				throw new Exception("No se puede obtener la información de la propiedad $Variable. Es posible que ésta no exista en el contexto actual");
		}
		return $valor;
	}

	function __set($Variable, $Valor)
	{
		switch($Variable)
		{
			default:
				throw new Exception("No se puede establecer un valor para la propiedad $Variable. Es posible que ésta no exista en el contexto actual");
		}
	}

	/**
	 * Obtiene el separador de ruta específico para el sistema operativo.
	 * @return string
	 */
	static function ObtenerSeparadorRuta()
	{
		$separador = '/';
		$softwareServidor = $_SERVER['SERVER_SOFTWARE'];
		if(preg_match('/Win/', $softwareServidor))
		$separador = '\\';
		return $separador;
	}

	/**
	 * Obtiene la ruta relativa a la raíz del sitio.
	 * @param string $Ruta Ruta desde la cual se localizará la ruta del sitio.
	 * @return string
	 */
	static function ObtenerRutaARaiz($Ruta = null)
	{
		$rutaARaiz = null;
		if(isset($_SERVER['SCRIPT_NAME']))
		{
			$Ruta = ($Ruta == null) ? $_SERVER['SCRIPT_NAME'] : $Ruta;
			$fragmentosRuta = explode('/', $Ruta);
			$conteoFragmentosRelevantes = -2;
			foreach($fragmentosRuta as $fragmentoRuta)
			if($fragmentoRuta != '..')
			$conteoFragmentosRelevantes++;
			if($conteoFragmentosRelevantes <= 0)
			$rutaARaiz = './';
			else
			{
				$rutaARaiz = '';
				while($conteoFragmentosRelevantes > 0)
				{
					$rutaARaiz .= '../';
					$conteoFragmentosRelevantes--;
				}
			}
			$rutaARaiz = realpath($rutaARaiz) . Config::RutaBase;
		}
		else
			$rutaARaiz = Config::RutaRemota;
		return $rutaARaiz;
	}

	/**
	 * Obtiene el nombre del servidor.
	 * @return string
	 */
	static function ObtenerNombreServidor()
	{
		$nombreServidor = '';
		if(isset($_SERVER['SERVER_NAME']))
		$nombreServidor = $_SERVER['SERVER_NAME'];
		else if(isset($_SERVER['HTTP_HOST']))
		$nombreServidor = $_SERVER['HTTP_HOST'];
		else
		$nombreServidor = Config::SYS_HOST_PREDET;
		return $nombreServidor;
	}
}
?>