<?php
/**
 * 
 * Representa un ítem que cuenta como puntaje, ya sea positivo o negativo.
 * @author Julián Mejio
 * @version 1.1
 * 
 * @property int $Id
 * @property UsuarioFacebook $Usuario
 * @property date $Fecha
 * @property string $Descripcion
 * @property int $Cantidad
 * @property boolean $EsSoloLectura
 */
class ItemPuntaje
{
	private $id;
	private $usuario;
	private $fecha;
	private $descripcion;
	private $cantidad;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Id':
				$valor = $this->id;
				break;
			case 'Usuario':
				$valor = $this->usuario;
				break;
			case 'Fecha':
				$valor = $this->fecha;
				break;
			case 'Descripcion':
				$valor = $this->descripcion;
				break;
			case 'Cantidad':
				$valor = $this->cantidad;
				break;
			case 'EsSoloLectura':
				$valor = ($this->Id != null);
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual");
		}
		return $valor;
	}

	function __set($Variable, $Valor)
	{
		if($this->EsSoloLectura)
		throw new Exception('El puntaje registrado no se puede modificar');
		switch($Variable)
		{
			case 'Descripcion':
				$this->descripcion = $Valor;
				break;
			case 'Cantidad':
				$this->cantidad = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	function __construct($Cantidad, $Descripcion)
	{
		$this->Cantidad = $Cantidad;
		$this->Descripcion = $Descripcion;
	}

	function Registrar(UsuarioFacebook $Usuario)
	{
		$fechaRegistro = date("Y-m-d");
		if($Usuario->IdFacebook == null)
		throw new Exception('El usuario no está registrado');
		$puntos = $Usuario->ObtenerPuntosDisponibles();
		if($puntos + $this->Cantidad < 0)
		throw new Exception('No tienes tantos puntos disponibles');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT Puntos (IdUsuario, Fecha, Descripcion, Cantidad) VALUES (%s, %s, %s, %s)",
		$conexionBd->Escapar($Usuario->IdFacebook),
		$conexionBd->Escapar($fechaRegistro),
		$conexionBd->Escapar($this->Descripcion),
		$conexionBd->Escapar($this->Cantidad)
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		$conexionBd->Desconectar();
		$this->usuario = $Usuario;
		$this->fecha = $fechaRegistro;
	}

	/**
	 * 
	 * Obtiene los ítems de puntuación de un usuario.
	 * @param UsuarioFacebook $Usuario
	 * @param int $Limite
	 * @throws Exception Se arroja cuando el usuario no está registrado.
	 */
	static function ObtenerItemsUsuario(UsuarioFacebook $Usuario, $Limite = null)
	{
		$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
		if($usuarioActivo->IdFacebook != $Usuario->IdFacebook && !$usuarioActivo->TienePermiso('VerTransacciones'))
			throw new Exception('No tiene privilegios para ver las transacciones de otros usuarios');
		$items = null;
		if($Usuario->IdFacebook == null)
			throw new Exception('El usuario no está registrado');
		$conexionBd = new ConectorBaseDatos();
		$cadenaLimite = '';
		if($Limite != null && is_int($Limite))
			$cadenaLimite = sprintf(" LIMIT %s",
				$conexionBd->Escapar($Limite)
			);
		$conexionBd->Sentencia = sprintf("SELECT Id, Fecha, Descripcion, Cantidad FROM Puntos WHERE IdUsuario = %s ORDER BY Id DESC$cadenaLimite",
			$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$itemsPuntaje = $conexionBd->EjecutarConsulta();
		if($itemsPuntaje != null)
		{
			$items = array();
			foreach($itemsPuntaje as $item)
			{
				$itemPuntaje = new ItemPuntaje($item['Cantidad'], $item['Descripcion']);
				$itemPuntaje->usuario = $Usuario;
				$itemPuntaje->id = $item['Id'];
				$itemPuntaje->fecha = $item['Fecha'];
				$items[] = $itemPuntaje;
			}
		}
		$conexionBd->Desconectar();
		return $items;
	}

	/**
	 *
	 * Obtiene el puntaje de un usuario.
	 * @param UsuarioFacebook $Usuario
	 * @throws Exception
	 * @return int
	 */
	public static function ObtenerPuntaje(UsuarioFacebook $Usuario)
	{
		$puntaje = null;
		if($Usuario->IdFacebook == null)
		throw new Exception('El usuario no está registrado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Puntos FROM PuntosUsuario WHERE IdUsuario = %s",
		$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$puntaje = $conexionBd->EjecutarEscalar();
		if($puntaje == null)
		$puntaje = 0;
		$conexionBd->Desconectar();
		return intval($puntaje);
	}
}
?>