<?php
/**
 *
 * Representa una actividad del club.
 * @version 1.4
 * @author Julián Mejio
 *
 * @property int $Id
 * @property TipoActividad $Tipo
 * @property UsuarioFacebook $Responsable
 * @property string $Descripcion
 * @property date $Fecha
 * @property Acta $ActaAsociada
 * @property bool $EstaCancelada
 * @property bool $EstaRealizada
 * @property bool $EstaRegistrada
 */
class Actividad
{
	private $id;
	private $tipo;
	private $responsable;
	private $descripcion;
	private $fecha;
	private $actaAsociada;
	private $estaCancelada;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Id':
				$valor = $this->id;
				break;
			case 'Tipo':
				$valor = $this->tipo;
				break;
			case 'Responsable':
				$valor = $this->responsable;
				break;
			case 'Fecha':
				$valor = $this->fecha;
				break;
			case 'ActaAsociada':
				$valor = $this->actaAsociada;
				break;
			case 'Descripcion':
				$valor = $this->descripcion;
				break;
			case 'EstaCancelada':
				$valor = $this->estaCancelada;
				break;
			case 'EstaRealizada':
				$valor = ($this->Fecha != null);
				break;
			case 'EstaRegistrada':
				$valor = ($this->Id != null);
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
				if(!is_a($Valor, 'TipoActividad'))
				throw new Exception('Tipo de actividad inválido');
				$this->tipo = $Valor;
				break;
			case 'Responsable':
				if(!is_a($Valor, 'UsuarioFacebook'))
				throw new Exception('Usuario inválido');
				$this->responsable = $Valor;
				break;
			case 'Fecha':
				if($Valor != null && !ExpresionRegular::EsValida($Valor, ExpresionRegular::FechaIso8601))
				throw new Exception('La fecha es inválida');
				$this->fecha = $Valor;
				break;
			case 'Descripcion':
				$this->descripcion = $Valor;
				break;
			case 'EstaCancelada':
				$this->estaCancelada = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea una actividad del club.
	 * @param TipoActividad $Tipo
	 * @param UsuarioFacebook $Responsable
	 * @param date $Fecha
	 */
	function __construct(TipoActividad $Tipo, UsuarioFacebook $Responsable, $Descripcion = null, $Fecha = null)
	{
		$this->Tipo = $Tipo;
		$this->Responsable = $Responsable;
		$this->Descripcion = $Descripcion;
		$this->Fecha = $Fecha;
		$this->EstaCancelada = false;
	}

	/**
	 *
	 * Verifica la integridad de la actividad.
	 * @return boolean
	 */
	private function integridadValida()
	{
		$valida = true;
		if($this->Fecha != null && $this->EstaCancelada)
		$valida = false;
		return $valida;
	}

	/**
	 *
	 * Registra una actividad en la base de datos.
	 */
	function Registrar()
	{
		if($this->EstaRegistrada)
			throw new Exception('Esta actividad ya se encuentra registrada');
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('RegistrarActividades'))
			throw new Exception('No tiene privilegios para registrar actividades');
		if(!$this->integridadValida())
			throw new Exception('La integridad de esta actividad ha sido violada: es posible que haya sido cancelada y sin embargo tenga fecha de realización, entre otras fallas de integridad');
		if(!$this->Responsable->EsMiembroClubOrion)
			throw new Exception('El responsable de la actividad debe ser un miembro validado del Club Orión');
		if(!$this->Tipo->EstaRegistrado)
			throw new Exception('El tipo de actividad no se encuentra registrado');
		if(is_a($this->ActaAsociada, 'Acta') && !$this->ActaAsociada->EstaRegistrada)
			throw new Exception('El acta asociada no está registrada');
		$idActa = (is_a($this->ActaAsociada, 'Acta')) ? $this->ActaAsociada->Id : NULL;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT Actividad (Tipo, Responsable, Descripcion, Fecha, Cancelada) VALUES (%s, %s, %s, %s, %s)",
			$conexionBd->Escapar($this->Tipo->Id),
			$conexionBd->Escapar($this->Responsable->IdFacebook),
			$conexionBd->Escapar($this->Descripcion),
			$conexionBd->Escapar($this->Fecha),
			$conexionBd->Escapar($this->EstaCancelada)
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		if($idActa != null)
			$this->AsociarActa($this->ActaAsociada);
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Actualiza los datos de una actividad.
	 */
	function Actualizar()
	{
		if(!$this->EstaRegistrada)
		throw new Exception('La actividad no está registrada');
		if(!$this->integridadValida())
		throw new Exception('La integridad de esta actividad ha sido violada: es posible que haya sido cancelada y sin embargo tenga fecha de realización, entre otras fallas de integridad');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Actividad SET Tipo = %s, Responsable = %s, Descripcion = %s, Fecha = %s, Cancelada = %s WHERE Id = %s",
		$conexionBd->Escapar($this->Tipo->Id),
		$conexionBd->Escapar($this->Responsable->IdFacebook),
		$conexionBd->Escapar($this->Descripcion),
		$conexionBd->Escapar($this->Fecha),
		$conexionBd->Escapar($this->EstaCancelada),
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Registra la actividad como completada y establece la fecha en la que se realizó.
	 * @param date $Fecha
	 */
	function Completar($Fecha)
	{
		$this->Fecha = $Fecha;
		if(is_a($this->ActaAsociada, 'Acta') && $this->ActaAsociada->Grupo->EsParticipativo)
		{
			$this->Tipo->Puntos->Aplicar($this->Responsable);
			$this->Responsable->Experiencia->OtorgarExperiencia($this->Tipo->Puntos->Experiencia);
		}
		$this->Actualizar();
	}

	/**
	 *
	 * Asocia la actividad a una acta.
	 * @param Acta $Acta
	 */
	function AsociarActa(Acta $Acta, $Fecha = null)
	{
		if(!$this->EstaRegistrada)
			throw new Exception('La actividad no está registrada');
		if(!$Acta->EstaRegistrada)
			throw new Exception('El acta no está registrada');
		if(is_a($this->ActaAsociada, 'Acta'))
			throw new Exception('Esta actividad ya está asociada a un acta');
		if(!$this->EstaRealizada && $Fecha == null)
			throw new Exception('La actividad debe estar realizada antes de asociarla a una acta, o debe especificar la fecha en que se realizó');
		if($this->EstaCancelada)
			throw new Exception('Esta actividad ya ha sido cancelada');
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if($usuario->IdFacebook != $Acta->Autor->IdFacebook && !$usuario->TienePermiso('ActualizarActas'))
			throw new Exception('Sólo el autor del acta o alguien con los privilegios necesarios puede asociar actividades a esta');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Actividad SET ActaAsociada = %s WHERE Id = %s",
			$conexionBd->Escapar($Acta->Id),
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->actaAsociada = $Acta;
		if(!$this->EstaRealizada)
			$this->Completar($Fecha);
	}
	
	/**
	 * 
	 * Obtiene las actividades que están asociadas a un acta
	 * @param Acta $Acta
	 */
	static function ObtenerActividadesActa(Acta $Acta)
	{
		$actividades = null;
		if(!$Acta->EstaRegistrada)
			throw new Exception('El acta no está registrada');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Id, Tipo, Responsable, Descripcion, Fecha, Cancelada FROM Actividad WHERE ActaAsociada = %s",
			$conexionBd->Escapar($Acta->Id)
		);
		$datosActividades = $conexionBd->EjecutarConsulta();
		if(is_array($datosActividades) && count($datosActividades) >= 1)
		{
			$actividades = array();
			foreach($datosActividades as $datosActividad)
			{
				$actividad = new Actividad(TipoActividad::Obtener($datosActividad['Tipo']), new UsuarioFacebook($datosActividad['Responsable']), $datosActividad['Descripcion'], $datosActividad['Fecha']);
				$actividad->actaAsociada = $Acta;
				$actividad->estaCancelada = $datosActividad['Cancelada'];
				$actividad->id = $datosActividad['Id'];
				$actividades[] = $actividad;
			}
		}
		$conexionBd->Desconectar();
		return $actividades;
	}
}
?>