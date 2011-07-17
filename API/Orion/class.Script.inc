<?php
/**
 *
 * Provee métodos de llamada a scripts en el sitio.
 * @author Julián Mejio
 * @version 1.0
 */
class Script
{
	/**
	 *
	 * Carga un script ya definido.
	 * @param string $Nombre Nombre del script.
	 * @param boolean $EsCritico Determina si el script es crítico o no.
	 * @throws Exception Se arroja cuando no se puede cargar el script, así como en el caso de que el script no se encuentre.
	 */
	static function Cargar($Nombre, $EsCritico = false)
	{
		$nombreCompleto = Uri::ObtenerRutaARaiz() . Config::RutaScripts . "/$Nombre.inc";
		if(file_exists($nombreCompleto))
		require_once($nombreCompleto);
		else if($EsCritico)
		throw new Exception("No se pudo cargar el script $Nombre. Compruebe el nombre del script");
	}
}
?>