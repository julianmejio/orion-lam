<?php
/**
 *
 * Representa la cantidad de puntos en sus diferentes modos que se aplican al realizar dicha acción.
 * @author Julián Mejio
 * @version 1.0
 *
 * @property ItemPuntaje $Puntos
 * @property int $Experiencia
 */
class BonoPuntos
{
	private $puntos;
	private $experiencia;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Puntos':
				$valor = $this->puntos;
				break;
			case 'Experiencia':
				$valor = $this->experiencia;
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
			case 'Puntos':
				if(!is_a($Valor, 'ItemPuntaje'))
				throw new Exception('Puntaje no válido para el bono');
				$this->puntos = $Valor;
				break;
			case 'Experiencia':
				$Valor = intval($Valor);
				if($Valor <= 0)
				throw new Exception('La experiencia no puede ser menor o igual a 0');
				$this->experiencia = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un bono de puntos.
	 * @param ItemPuntaje $Puntos
	 * @param int $Experiencia
	 */
	function __construct(ItemPuntaje $Puntos = null, $Experiencia = 0)
	{
		$this->Puntos = $Puntos;
		$this->Experiencia = $Experiencia;
	}

	/**
	 *
	 * Aplica el bono a un usuario
	 * @param UsuarioFacebook $Usuario
	 */
	function Aplicar(UsuarioFacebook $Usuario)
	{
		$Usuario->GestionarPuntos($this->Puntos);
		$Usuario->Experiencia->OtorgarExperiencia($this->Experiencia);
	}
}
?>