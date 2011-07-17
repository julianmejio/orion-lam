<?php
class ExpresionRegular
{
	const FechaIso8601 = '/^([\d]{4})-([\d]{2})-([\d]{2})$/';

	static function EsValida($Cadena, $Patron)
	{
		return preg_match($Patron, $Cadena);
	}
}
?>