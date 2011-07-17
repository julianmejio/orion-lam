<?php
/**
 *
 * Representa un tipo de actividad realizada en el club.
 * @author Julián Mejio
 * @version 1.0
 *
 * @property int $Id
 * @property string $Nombre
 * @property string $Descripcion
 * @property boolean $EstaRegistrado
 * @property BonoPuntos $Puntos
 */
class TipoActividad
{
	private $id;
	private $nombre;
	private $descripcion;
	private $puntos;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Id':
				$valor = $this->id;
				break;
			case 'Nombre':
				$valor = $this->nombre;
				break;
			case 'Descripcion':
				$valor = $this->descripcion;
				break;
			case 'EstaRegistrado':
				$valor = ($this->Id != null);
				break;
			case 'Puntos':
				$valor = $this->puntos;
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
			case 'Descripcion':
				$this->descripcion = $Valor;
				break;
			case 'Puntos':
				if(!is_a($Valor, 'BonoPuntos') && $Valor != null)
				throw new Exception('Puntos no válidos para un tipo de actividad');
				$this->puntos = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea una tipo de actividad realizada en el club.
	 * @param string $Nombre
	 * @param string $Descripcion
	 */
	function __construct($Nombre, $Descripcion = null, BonoPuntos $Puntos = null)
	{
		$this->Nombre = $Nombre;
		$this->Descripcion = $Descripcion;
		$this->Puntos = $Puntos;
	}

	/**
	 *
	 * Registra una actividad en la base de datos.
	 */
	function Registrar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('CrearTipoActividad'))
		throw new Exception('No tiene permiso para crear un nuevo tipo de actividad');
		if($this->EstaRegistrado)
		throw new Exception('El tipo de actividad ya está registrado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT TipoActividad (Nombre, Descripcion, ItemPuntos) VALUES (%s, %s, %s)",
		$conexionBd->Escapar($this->Nombre),
		$conexionBd->Escapar($this->Descripcion),
		$conexionBd->Escapar(addslashes(serialize($this->Puntos)))
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Actualiza la información del tipo de actividad con la base de datos.
	 */
	function Actualizar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('ModTipoActividad'))
		throw new Exception('No tiene permiso para modificar tipos de actividad');
		if(!$this->EstaRegistrado)
		throw new Exception('EL tipo de actividad no está registrado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE TipoActividad SET Nombre = %s, Descripcion = %s, ItemPuntos = %s WHERE Id = %s",
		$conexionBd->Escapar($this->Nombre),
		$conexionBd->Escapar($this->Descripcion),
		$conexionBd->Escapar(serialize($this->Puntos)),
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Elimina un tipo de actividad que se haya registrado.
	 */
	function Eliminar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('ElimTipoActividad'))
		throw new Exception('No tiene permiso para eliminar los tipos de actividad');
		if(!$this->EstaRegistrado)
		throw new Exception('Este tipo de actividad aún no está registrado en la base de datos');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("DELETE FROM TipoActividad WHERE Id = %s",
		$conexionBd->Escapar($this->Id)
		);
		try
		{
			$conexionBd->EjecutarComando();
		}
		catch(Exception $e)
		{
			if(preg_match('/cannot delete/i', $e->getMessage()))
			throw new Exception('No puedes eliminar este tipo de actividad porque alguna vez se usó');
		}
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Obtiene el tipo de actividad con el ID especificado.
	 * @param int $Id
	 */
	static function Obtener($Id)
	{
		$tipo = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Nombre, Descripcion, ItemPuntos FROM TipoActividad WHERE Id = %s",
		$conexionBd->Escapar($Id)
		);
		$datosTipo = $conexionBd->EjecutarConsulta();
		if(is_array($datosTipo) && count($datosTipo) == 1)
		{
			$datosTipo = $datosTipo[0];
			$tipo = new TipoActividad(
			stripslashes($datosTipo['Nombre']),
			stripslashes($datosTipo['Descripcion']),
			unserialize(stripslashes($datosTipo['ItemPuntos']))
			);
			$tipo->id = $Id;
		}
		$conexionBd->Desconectar();
		return $tipo;
	}

	/**
	 *
	 * Obtiene todos los tipos de actividad registrados.
	 */
	static function ObtenerTodos()
	{
		$tipos = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT Id, Nombre, Descripcion, ItemPuntos FROM TipoActividad ORDER BY Nombre';
		$datosTipos = $conexionBd->EjecutarConsulta();
		if(is_array($datosTipos) && count($datosTipos) >= 1)
		{
			$tipos = array();
			foreach($datosTipos as $datosTipo)
			{
				$tipo= new TipoActividad(
				stripslashes($datosTipo['Nombre']),
				stripslashes($datosTipo['Descripcion']),
				unserialize(stripslashes($datosTipo['ItemPuntos']))
				);
				$tipo->id = $datosTipo['Id'];
				$tipos[] = $tipo;
			}
		}
		$conexionBd->Desconectar();
		return $tipos;
	}
}
?>