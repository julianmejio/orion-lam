<?php
/**
 *
 * Representa un acta del Club.
 * @author Julián Mejio
 * @version 1.6
 *
 * @property int $Id
 * @property date $Fecha
 * @property Grupo $Grupo
 * @property string $Resumen
 * @property boolean $EstaRegistrada
 * @property UsuarioFacebook $Asistencia
 */
class Acta
{
	private $id;
	private $fecha;
	private $grupo;
	private $resumen;
	private $asistencia;
	private $autor;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Id':
				$valor = $this->id;
				break;
			case 'Fecha':
				$valor = $this->fecha;
				break;
			case 'Grupo':
				$valor = $this->grupo;
				break;
			case 'Resumen':
				$valor = $this->resumen;
				break;
			case 'Autor':
				$valor = $this->autor;
				break;
			case 'EstaRegistrada':
				$valor = ($this->Id != null);
				break;
			case 'Asistencia':
				$valor = $this->asistencia;
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
			case 'Fecha':
				if(!ExpresionRegular::EsValida($Valor, ExpresionRegular::FechaIso8601))
				throw new Exception('Fecha inválida');
				$this->fecha = $Valor;
				break;
			case 'Grupo':
				$this->grupo = $Valor;
				break;
			case 'Resumen':
				$this->resumen = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea una acta.
	 * @param Grupo $Grupo
	 * @param date $Fecha
	 * @param string $Resumen
	 */
	function __construct(Grupo $Grupo, $Fecha, $Resumen)
	{
		$this->Grupo = $Grupo;
		$this->Fecha = $Fecha;
		$this->Resumen = $Resumen;
		$this->asistencia = array();
	}

	/**
	 *
	 * Registra el acta en la base de datos.
	 */
	function Registrar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('RegistrarActas'))
			throw new Exception('No tiene privilegios para registrar actas');
		if($this->EstaRegistrada)
			throw new Exception('Esta acta ya está registrada');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT Acta (Grupo, Fecha, Resumen, Autor) VALUES (%s, %s, %s, %s)",
			$conexionBd->Escapar($this->Grupo->Id),
			$conexionBd->Escapar($this->Fecha),
			$conexionBd->Escapar($this->Resumen),
			$conexionBd->Escapar($usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		foreach($this->Asistencia as $asistencia)
		{
			$conexionBd->EjecutarComando(sprintf("INSERT Asistencia VALUES (%s, %s)",
				$conexionBd->Escapar($asistencia->IdFacebook),
				$conexionBd->Escapar($this->Id)
			));
			if($this->Grupo->EsParticipativo)
				$asistencia->Experiencia->OtorgarExperiencia(Config::ExperienciaAsistencia);
		}
		$conexionBd->Desconectar();
		$this->autor = $usuario;
	}
	
	/**
	 * 
	 * Actualiza la fecha y la descripción del acta.
	 */
	function Actualizar()
	{
		if(!$this->EstaRegistrada)
			throw new Exception('Esta acta no está registrada');
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('ActualizarActas'))
			throw new Exception('No tiene privilegios para actualizar actas');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Acta SET Fecha = %s, Resumen = %s WHERE Id = %s",
			$conexionBd->Escapar($this->Fecha),
			$conexionBd->Escapar($this->Resumen),
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Agrega la asistencia de un usuario al acta.
	 * @param Usuario $Usuario
	 */
	function AgregarAsistencia(UsuarioFacebook $Usuario)
	{
		if(!$Usuario->EsMiembroClubOrion)
			throw new Exception('El usuario no es miembro del Club');
		$existe = false;
		foreach($this->Asistencia as $asistencia)
			if($asistencia->IdFacebook == $Usuario->IdFacebook)
			{
				$existe = true;
				break;
			}
		if(!$existe)
		{
			$this->asistencia[] = $Usuario;
			if($this->EstaRegistrada)
			{
				$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
				if(!$usuario->TienePermiso('ActualizarActas'))
					throw new Exception('No tiene privilegios para actualizar actas');
				$conexionBd = new ConectorBaseDatos();
				$conexionBd->Sentencia = sprintf("INSERT Asistencia VALUES (%s, %s)",
					$conexionBd->Escapar($Usuario->IdFacebook),
					$conexionBd->Escapar($this->Id)
				);
				$conexionBd->EjecutarComando();
				$conexionBd->Desconectar();
			}
		}
	}

	/**
	 *
	 * Elimina la asistencia de un usuario del acta.
	 * @param Usuario $Usuario
	 */
	function QuitarAsistencia(UsuarioFacebook $Usuario)
	{
		if($this->EstaRegistrada)
			throw new Exception('No se puede quitar la asistencia de una acta registrada');
		$nuevaLista = array();
		foreach($this->Asistencia as $asistencia)
			if($asistencia->IdFacebook != $Usuario->IdFacebook)
				$nuevaLista[] = $asistencia;
		$this->asistencia = $nuevaLista;
	}
	
	/**
	 * 
	 * Obtiene las actividades asociadas al acta.
	 * @return Actividad
	 */
	function ObtenerActividades()
	{
		return Actividad::ObtenerActividadesActa($this);
	}
	
	/**
	 * 
	 * Obtiene un acta identificada por un ID.
	 * @param int $Id
	 * @return Acta
	 */
	static function Obtener($Id)
	{
		$acta = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Grupo, Autor, Fecha, Resumen FROM Acta WHERE Id = %s",
			$conexionBd->Escapar($Id)
		);		
		$datosActa = $conexionBd->EjecutarConsulta();
		if(!is_array($datosActa) || count($datosActa) == 0)
			throw new Exception('Ha especificado un acta no válida');
		$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
		$permiso = false;
		if($usuarioActivo->TienePermiso('VerActas'))
			$permiso = true;
		if(!$permiso)
		{
			if($usuarioActivo->IdFacebook == $datosActa[0]['Autor'])
				$permiso = true;
			else
			{
				$gruposUsuario = $usuarioActivo->ObtenerGruposAsociados();
				foreach($gruposUsuario as $grupo)
					if($grupo->Id == $datosActa[0]['Grupo'])
					{
						$permiso = true;
						break;
					}
			}
			if(!$permiso)
				throw new Exception('No tiene permiso para ver esta acta');
		}
		$acta = new Acta(Grupo::Cargar($datosActa[0]['Grupo']), $datosActa[0]['Fecha'], stripslashes($datosActa[0]['Resumen']));
		$acta->autor = new UsuarioFacebook($datosActa[0]['Autor']);
		$datosActa = null;
		$acta->id = $Id;
		$conexionBd->Sentencia = sprintf("SELECT IdUsuario FROM Asistencia WHERE IdActa = %s",
			$conexionBd->Escapar($acta->Id)
		);
		$datosAsistencia = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		foreach($datosAsistencia as $datoAsistencia)
			$acta->asistencia[] = new UsuarioFacebook($datoAsistencia['IdUsuario']);
		$datosAsistencia = null;
		return $acta;
	}
}
?>