<?php
/**
 *
 * Representa un atributo de XML.
 * @author Julián Mejio
 * @version 1.0
 *
 * @property string $Nombre
 * @property string $Valor
 */
class AtributoXml
{
	/**
	 *
	 * Nombre del atributo.
	 * @var string
	 */
	private $nombre;
	/**
	 *
	 * Valor del atributo.
	 * @var string
	 */
	private $valor;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Nombre':
				$valor = $this->nombre;
				break;
			case 'Valor':
				$valor = $this->valor;
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
			case 'Nombre':
				$this->nombre = $Valor;
				break;
			case 'Valor':
				$this->valor = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un atributo XML.
	 * @param string $Nombre
	 * @param string $Valor
	 */
	function __construct($Nombre, $Valor)
	{
		$this->Nombre = $Nombre;
		$this->Valor = $Valor;
	}

	/**
	 *
	 * Obtiene una versión compatible de texto del atributo.
	 * @return string
	 */
	function ATexto()
	{
		return "$this->Nombre=\"" . htmlspecialchars($this->Valor) . "\"";
	}
}
?>