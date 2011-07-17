<?php
class DocumentoXml
{
	private $encabezado;

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
}
?>