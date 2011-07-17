<?php
/**
 *
 * Petición de muro para Facebook.
 * @author Julián Mejio
 * @version 1.6
 *
 * @property UsuarioFacebook $Usuario
 * @property string $Mensaje
 * @property string $Hipervinculo
 * @property string $UrlImagen
 * @property string $NombreVinculo
 * @property string $Titulo
 * @property string $Descripcion
 * @property string $Acciones
 */
class FacebookPost
{
	private $usuario;
	private $mensaje;
	private $hipervinculo;
	private $urlImagen;
	private $nombreVinculo;
	private $titulo;
	private $descripcion;
	private $acciones;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Usuario':
				$valor = $this->usuario;
				break;
			case 'Mensaje':
				$valor = $this->mensaje;
				break;
			case 'Hipervinculo':
				$valor = $this->hipervinculo;
				break;
			case 'UrlImagen':
				$valor = $this->urlImagen;
				break;
			case 'NombreVinculo':
				$valor = $this->nombreVinculo;
				break;
			case 'Titulo':
				$valor = $this->titulo;
				break;
			case 'Descripcion':
				$valor = $this->descripcion;
				break;
			case 'Acciones':
				$valor = $this->acciones;
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
			case 'Usuario':
				if(!is_a($Valor, 'UsuarioFacebook'))
				throw new Exception('Usuario especificado no válido');
				$this->usuario = $Valor;
				break;
			case 'Mensaje':
				$this->mensaje = $Valor;
				break;
			case 'Hipervinculo':
				$this->hipervinculo = $Valor;
				break;
			case 'UrlImagen':
				$this->urlImagen = $Valor;
				break;
			case 'NombreVinculo':
				$this->nombreVinculo = $Valor;
				break;
			case 'Titulo':
				$this->titulo = $Valor;
				break;
			case 'Descripcion':
				$this->descripcion = $Valor;
				break;
			case 'Acciones':
				$this->acciones = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea una entrada que puede ser publicada en el Muro de un usuario en Facebook.
	 * @param UsuarioFacebook $Usuario
	 * @param string $Mensaje
	 * @param string $Hipervinculo
	 * @param string $UrlImagen
	 * @param string $NombreVinculo
	 * @param string $Titulo
	 * @param string $Descripcion
	 * @param array $Acciones
	 */
	function __construct(UsuarioFacebook $Usuario, $Mensaje, $Hipervinculo, $UrlImagen = null, $NombreVinculo = null, $Titulo = null, $Descripcion = null, $Acciones = null)
	{
		$this->Usuario = $Usuario;
		$this->Mensaje = $Mensaje;
		$this->Hipervinculo = $Hipervinculo;
		$this->UrlImagen = $UrlImagen;
		$this->NombreVinculo = $NombreVinculo;
		$this->Titulo = $Titulo;
		$this->Descripcion = $Descripcion;
		$this->Acciones = $Acciones;
	}

	/**
	 *
	 * Publica una entrada en el Muro de Facebook.
	 */
	function Publicar()
	{
		if($this->Mensaje == null || $this->Hipervinculo == null)
		throw new Exception('Es obligatorio un mensaje y un hipervínculo, para poder publicar en el Muro de Facebook');
		$parametros = array(
			'message' => $this->Mensaje,
			'link' => $this->Hipervinculo
		);
		if($this->UrlImagen != null)
			$parametros['picture'] = $this->UrlImagen;
		if($this->NombreVinculo != null)
			$parametros['name'] = $this->NombreVinculo;
		if($this->Titulo != null)
			$parametros['caption'] = $this->Titulo;
		if($this->Descripcion != null)
			$parametros['description'] = $this->Descripcion;
		if(is_array($this->Acciones))
			$parametros['actions'] = $this->Acciones;
		$this->Usuario->ApiAuth(
			"/{$this->Usuario->IdFacebook}/feed",
			'POST',
			$parametros
		);
	}
	
	/**
	 * 
	 * Elimina una publicación de Facebook que está en el muro.
	 * @param UsuarioFacebook $Usuario
	 * @param mixed $IdPost
	 */
	static function Eliminar(UsuarioFacebook $Usuario, $IdPost)
	{
		$Usuario->ApiAuth("/{$Usuario->IdFacebook}_$IdPost", 'DELETE');
	}
}
?>