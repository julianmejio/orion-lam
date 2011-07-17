<?php
/**
 *
 * Representa una etiqueta XML.
 * @author Julián Mejio
 * @version 1.0
 *
 * @property string $Nombre Nombre de la etiqueta.
 * @property AtributoXml $Atributos Atributos de la etiqueta.
 * @property mixed $Contenido Contenido del nodo de la etiqueta. Puede ser otro elemento EtiquetaXml.
 */
class EtiquetaXml
{
	private $nombre;
	private $atributos;
	private $contenido;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Nombre':
				$valor = $this->nombre;
				break;
			case 'Atributos':
				$valor = $this->atributos;
				break;
			case 'Contenido':
				$valor = $this->contenido;
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
			case 'Contenido':
				$this->contenido = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea una etiqueta HTML.
	 * @param string $Nombre Nombre.
	 * @param string $Contenido Contenido.
	 */
	function __construct($Nombre, $Contenido = null)
	{
		$this->Nombre = $Nombre;
		$this->Contenido = $Contenido;
		$this->atributos = array();
	}

	/**
	 *
	 * Agrega un atributo XML a la etiqueta XML.
	 * @param AtributoXml $Atributo Atributo.
	 */
	function AgregarAtributo(AtributoXml $Atributo, $Sobreescribir = false)
	{
		$existe = false;
		$i = null;
		for($i = 0; $i < count($this->atributos); $i++)
		if(strtolower($this->atributos[$i]->Nombre) == strtolower($Atributo->Nombre))
		{
			if($Sobreescribir)
			unset($this->atributos[$i]);
			else
			$existe = true;
			break;
		}
		if($existe)
		throw new Exception("El atributo $Atributo->Nombre ya está definido en esta etiqueta XML");
		if($i == count($this->atributos) - 1)
		$i++;
		$this->atributos[$i] = $Atributo;
	}

	/**
	 *
	 * Obtiene una versión representativa en texto de la etiqueta en XML.
	 * @param boolean $ContenidoSeguro Convierte caracteres especiales de HTML a versiones literales si su valor es verdadero.
	 * @param boolean $Simplificado Cierra (si es posible) el contenido de manera simplificada.
	 * @return string
	 */
	function ATexto($ContenidoSeguro = true, $Simplificado = true)
	{
		$etiqueta = null;
		$etiqueta = "<$this->Nombre";
		foreach($this->Atributos as $atributo)
		$etiqueta .= ' ' . $atributo->ATexto();
		if(is_array($this->Contenido))
		{
			$etiqueta .= '>';
			foreach($this->Contenido as $elemento)
			if(is_a($elemento, 'EtiquetaXml'))
			$etiqueta .= $elemento->ATexto($ContenidoSeguro, $Simplificado);
			else if(is_string($elemento))
			$etiqueta .= $elemento;
			$etiqueta .= "</$this->Nombre>";
		}
		else if(is_a($this->Contenido, 'EtiquetaXml'))
		$etiqueta .= ">" . $this->Contenido->ATexto($ContenidoSeguro) . "</$this->Nombre>";
		else if($this->Contenido != null)
		if($ContenidoSeguro)
		$etiqueta .= ">" . htmlspecialchars($this->Contenido) . "</$this->Nombre>";
		else
		$etiqueta .= ">$this->Contenido</$this->Nombre>";
		else if($Simplificado)
		$etiqueta .= '/>';
		else
		$etiqueta .= "></$this->Nombre>";
		return $etiqueta;
	}

	/**
	 *
	 * Agrega contenido a la etiqueta XML.
	 * @param mixed $Contenido Contenido. Si el contenido actual de la etiqueta no es un arreglo, se reemplazará por un arreglo y se añadirá el contenido a agregar.
	 * @param boolean $InlcuirContenidoInicial Incluye el contenido actual en caso tal que el contenido no sea un arreglo y que el contenido actual no sea vacío.
	 */
	function AgregarContenido($Contenido, $InlcuirContenidoInicial = true)
	{
		if(!is_array($this->Contenido) && $InlcuirContenidoInicial && $this->Contenido != null)
		$this->AgregarContenido($this->Contenido, false);
		else if(!is_array($this->Contenido))
		$this->Contenido = array();
		if(is_a($Contenido, 'EtiquetaXml'))
		$this->contenido[] = $Contenido;
		else
		$this->contenido[] = strval($Contenido);
	}
}
?>