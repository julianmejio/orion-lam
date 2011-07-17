<?php
/**
 *
 * Representa un vínculo en HTML.
 * @author Julián Mejio
 * @version 1.0
 *
 * @property string $Id Valor que tomará el atributo ID de <a> en HTML.
 * @property string $Texto Texto descriptivo del vínculo.
 * @property string $Vinculo Vínculo.
 */
class VinculoHtml {

	private $texto;
	private $vinculo;
	private $id;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Id':
				$valor = $this->id;
				break;
			case 'Texto':
				$valor = $this->texto;
				break;
			case 'Vinculo':
				$valor = $this->vinculo;
				break;
			default:
				throw new Exception("La propiedad $Variable no está disponible");
		}
		return $valor;
	}

	function __set($Variable, $Valor)
	{
		switch($Variable)
		{
			case 'Id':
				$this->id = $Valor;
				break;
			case 'Texto':
				if($Valor == null)
				$Valor = $this->Vinculo;
				$this->texto = $Valor;
				break;
			case 'Vinculo':
				if($Valor == null)
				throw new Exception('El vínculo HTML no puede ser nulo');
				$this->vinculo = $Valor;
				break;
			default:
				throw new Exception("La propiedad $Variable no está disponible");
		}
	}

	/**
	 *
	 * Crea un nuevo vínculo HTML.
	 * @param string $Vinculo Vínculo.
	 * @param string $Texto Texto representativo del vínculo.
	 * @param string $Id Id del vínculo.
	 */
	function __construct($Vinculo, $Texto = null, $Id = null)
	{
		$this->Vinculo = $Vinculo;
		$this->Texto = ($Texto != null) ? $Texto : $this->Vinculo;
	}

	/**
	 *
	 * Obtiene el vínculo en formato HTML.
	 * @return string
	 */
	function ObtenerHtml()
	{
		return $vinculo->ATexto();
	}

	/**
	 *
	 * Devuelve un vínculo como EtiquetaXml.
	 * @return EtiquetaXml
	 */
	function AEtiquetaXml()
	{
		$contenido = ($this->Texto != null) ? $this->Texto : $this->Vinculo;
		$vinculo = new EtiquetaXml('a', $contenido);
		$vinculo->AgregarAtributo(new AtributoXml('href', $this->Vinculo));
		if($this->Id != null)
		$vinculo->AgregarAtributo(new AtributoXml('id', $this->Id));
		if($this->Texto != null)
		$vinculo->AgregarAtributo(new AtributoXml('title', $contenido));
		return $vinculo;
	}


	/**
	 *
	 * Renderiza el vínculo dentro de la llamada en HTML.
	 */
	function RenderizarHtml()
	{
		echo $this->ObtenerHtml();
	}

	/**
	 *
	 * Determina si un vínculo está activo.
	 * @param boolean $Exacto Determina si la verificación se hará exacta o no.
	 * @return boolean
	 */
	function EsVinculoActivo($Exacto = false)
	{
		$Exacto = ($Exacto) ? '$' : '';
		$uriActual = strtolower($_SERVER['REQUEST_URI']);
		return preg_match('|^' . strtolower($this->Vinculo) . $Exacto . '|', $uriActual);
	}

	/**
	 *
	 * Devuelve un texto en formato HTML con vínculos activos.
	 * @param string $Texto Texto que contendría los vínculos (iniciados con http://).
	 * @return string
	 */
	static function ActivarVinculosTexto($Texto)
	{
		return preg_replace("|(http://\S*)|i", "<a href=\"$1\">$1</a>", $Texto);
	}
}
?>