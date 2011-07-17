<?php
class PropiedadLamina
{
	private $lamina;
	private $tipo;
	private $nombre;
	private $valor;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Lamina':
				$valor = $this->lamina;
				break;
			case 'Tipo':
				$valor = $this->tipo;
				break;
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
			case 'Tipo':
				$this->tipo = $Valor;
				break;
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
	 * Crea una propiedad de lámina.
	 * @param Lamina $Lamina
	 * @param string $Tipo
	 * @param string $Nombre
	 * @param string $Valor
	 */
	function __construct(Lamina $Lamina, $Tipo, $Nombre, $Valor)
	{
		$this->lamina = $Lamina;
		$this->tipo = $Tipo;
		$this->nombre = $Nombre;
		$this->valor = $Valor;
	}
}
?>