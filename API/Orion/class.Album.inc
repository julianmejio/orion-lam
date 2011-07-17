<?php
/**
 *
 * Representa un álbum.
 * @author Julián Mejio
 *
 * @property int $Id Id del álbum. Propiedad de sólo lectura.
 * @property UsuarioFacebook $Usuario Propietario del álbum.
 * @property int $NumeroCromosDisponibles Número de cromos disponibles para reclamar.
 * @property date $FechaCreacion Fecha de creación del álbum.
 * @property LaminaAlbum $Cromos Cromos que hay en el álbum.
 */
class Album
{
	private $id;
	private $usuario;
	private $numeroCromosDisponibles;
	private $fechaCreacion;
	private $cromos;

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
			case 'NumeroCromosDisponibles':
				$valor = $this->numeroCromosDisponibles;
				break;
			case 'Cromos':
				$valor = $this->cromos;
				break;
			case 'EstaRegistrado':
				$valor = ($this->Id != null);
				break;
			case 'FechaCreacion':
				$valor = $this->fechaCreacion;
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
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un nuevo álbum.
	 */
	function __construct()
	{
		$this->cromos = array();
		$this->numeroCromosDisponibles = 0;
	}

	/**
	 *
	 * Registra el álbum.
	 * @param UsuarioFacebook $Usuario
	 */
	function Registrar(UsuarioFacebook $Usuario)
	{
		if($this->EstaRegistrado)
		throw new Exception('Este álbum ya se encunetra registrado');
		if($Usuario->IdFacebook == null)
		throw new Exception('El usuario no está registrado');
		$fechaCreacion = date("Y-m-d)");
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT Album (IdUsuario, FechaCreacion) VALUES (%s, %s)",
		$conexionBd->Escapar($Usuario->IdFacebook),
		$conexionBd->Escapar($fechaCreacion)
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		$this->fechaCreacion = $fechaCreacion;
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Agregar una cantidad específica de cromos que pueden ser reclamados.
	 * @param int $Cantidad
	 */
	function GestionarCromosDisponibles($Cantidad)
	{
		if(!$this->EstaRegistrado)
		throw new Exception('El álbum no está registrado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Album SET NumeroCromosDisponibles = NumeroCromosDisponibles + %s WHERE Id = %s LIMIT 1",
		$conexionBd->Escapar($Cantidad),
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->numeroCromosDisponibles += $Cantidad;
	}

	/**
	 *
	 * Inicia el proceso de selección de cromos.
	 * @return array Devuelve un arreglo con cierta cantidad de ítems, y cada uno de estos es una cadena que identifica un ID de lámina.
	 */
	function IniciarSeleccionCromo()
	{
		if(isset($_SESSION['IdSeleccion']))
		throw new Exception('Ya hay otro proceso de selección de cromo activo');
		if($this->NumeroCromosDisponibles <= 0)
		throw new Exception('No hay más cromos disponibles para reclamar');
		$_SESSION['IdSeleccion'] = md5(rand(0, time()) . $this->Id);
		$idsCromos = array();
		$laminas = Lamina::ObtenerLaminasAleatorias();
		foreach($laminas as $lamina)
		$idsCromos[] = Cifrador::Cifrar($lamina->Id, $_SESSION['IdSeleccion']);
		return $idsCromos;
	}

	/**
	 *
	 * Limpia la cookie asociada con el proceso de selección de cromos.
	 */
	function LimpiarSeleccionCromo()
	{
		$_SESSION['IdSeleccion'] = NULL;
		unset($_SESSION['IdSeleccion']);
	}

	/**
	 *
	 * Finaliza el proceso de selección de cromos, agrega el cromo seleccionado y limpia la cookie asociada a la selección.
	 * @param string $IdCromo Debe ser una cadena arrojada en la misma sesión, y por la misma instancia, desde la función Album::IniciarSeleccionCromo()
	 * @return LaminaAlbum
	 */
	function FinalizarSeleccionCromo($IdCromo)
	{
		$lamina = null;
		if(!isset($_SESSION['IdSeleccion']))
		throw new Exception('El proceso de selección de cromo no está activo');
		if($this->NumeroCromosDisponibles <= 0)
		throw new Exception('El proceso de selección de cromos está activo pero no hay más cromos disponibles para este álbum');
		$idCromo = intval(Cifrador::Descifrar($IdCromo, $_SESSION['IdSeleccion']));
		if(!is_int($idCromo))
		throw new Exception('El ID de la lámina es inválido');
		$lamina = Lamina::ObtenerLamina($idCromo);
		if($lamina == null)
		throw new Exception('La lámina seleccionada no es válida');
		$this->AgregarLamina($lamina);
		$this->GestionarCromosDisponibles(-1);
		$this->LimpiarSeleccionCromo();
		$lamina = LaminaAlbum::Obtener($this, $lamina);
		return $lamina;
	}

	/**
	 *
	 * Agrega una lámina al álbum.
	 * @param Lamina $Lamina
	 * @throws Exception
	 */
	public function AgregarLamina(Lamina $Lamina)
	{
		if(!$this->EstaRegistrado)
		throw new Exception('El álbum no está registrado');
		if(!$Lamina->EstaRegistrada)
		throw new Exception('La lámina no está registrada');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT COUNT(*) FROM LaminaAlbum WHERE IdAlbum = %s AND IdLamina = %s",
		$conexionBd->Escapar($this->Id),
		$conexionBd->Escapar($Lamina->Id)
		);
		$existente = $conexionBd->EjecutarEscalar();
		if($existente == 0)
		{
			$conexionBd->Sentencia = sprintf("INSERT LaminaAlbum (IdAlbum, IdLamina, FechaAdquisicion) VALUES (%s, %s, %s)",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($Lamina->Id),
			$conexionBd->Escapar(date("Y-m-d"))
			);
			$this->Usuario->Experiencia->OtorgarExperiencia(Config::ExperienciaLaminaNueva);
			$postFb = new FacebookPost(
			$this->Usuario,
				'¡Obtuve una nueva lámina en mi álbum!',
				'http://mehiocore.com' . Config::RutaBase . '/Panel',
				'http://mehiocore.com' . Config::RutaBase . '/Imagenes/FBImg/Lamina.png',
			$Lamina->Nombre,
			$Lamina->Clasificacion,
			$Lamina->Descripcion,
			array(array('name' => 'Mi Club', 'link' => 'http://www.mehiocore.com' . Config::RutaBase . '/Panel'))
			);
			try
			{
				$postFb->Publicar();
			} catch(Exception $e) {}
		}
		else
		$conexionBd->Sentencia = sprintf("UPDATE LaminaAlbum SET Cantidad = Cantidad + 1 WHERE IdAlbum = %s AND IdLamina = %s",
		$conexionBd->Escapar($this->Id),
		$conexionBd->Escapar($Lamina->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Reitra una lámina del álbum. Sólo se pueden retirar aquellas que están repetidas.
	 * @param Lamina $Lamina
	 * @throws Exception Se arroja cuando se intenta retirar una lámina que no está repetida.
	 */
	public function RetirarLamina(Lamina $Lamina)
	{
		if(!$this->EstaRegistrado)
		throw new Exception('El álbum no está registrado');
		if(!$Lamina->EstaRegistrada)
		throw new Exception('La lámina no está registrada');
		$cantidadLamina = 0;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Cantidad FROM LaminaAlbum WHERE Album = %s AND Lamina = %s LIMIT 1",
		$conexionBd->Escapar($this->Id),
		$conexionBd->Escapar($Lamina->Id)
		);
		$cantidad = $conexionBd->EjecutarEscalar();
		if($cantidad != null && intval($cantidad) > 1)
		{
			$conexionBd->Sentencia = sprintf("UPDATE LaminaAlbum SET Cantidad = Cantidad - 1 WHERE Album = %s AND Lamina = %s",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($Lamina->Id)
			);
			$conexionBd->EjecutarComando();
		}
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Obtiene los álbumes del usuario.
	 * @param UsuarioFacebook $Usuario
	 * @throws Exception
	 * @return Album
	 */
	public static function ObtenerAlbumes(UsuarioFacebook $Usuario)
	{
		$albumes = null;
		if($Usuario->IdFacebook == null)
		throw new Exception('El usuario no está registrado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Id, NumeroCromosDisponibles, FechaCreacion FROM Album WHERE IdUsuario = %s",
		$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$datosAlbumes = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		if($datosAlbumes != null)
		{
			$albumes = array();
			foreach($datosAlbumes as $datos)
			{
				$album = new Album();
				$album->id = $datos['Id'];
				$album->usuario = $Usuario;
				$album->numeroCromosDisponibles = $datos['NumeroCromosDisponibles'];
				$album->fechaCreacion = $datos['FechaCreacion'];
				$album->cromos = $album->ObtenerLaminas();
				$albumes[] = $album;
			}
		}
		return $albumes;
	}

	/**
	 *
	 * Obtiene un álbum específico.
	 * @param int $Id
	 */
	public static function Obtener($Id)
	{
		$album = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT NumeroCromosDisponibles, FechaCreacion, IdUsuario FROM Album WHERE Id = %s",
		$conexionBd->Escapar($Id)
		);
		$datosAlbum = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		if($datosAlbum != null)
		{
			$album = new Album();
			$album->id = $Id;
			$album->usuario = new UsuarioFacebook($datosAlbum[0]['IdUsuario']);
			$album->numeroCromosDisponibles = $datosAlbum[0]['NumeroCromosDisponibles'];
			$album->fechaCreacion = $datos['FechaCreacion'];
			$album->cromos = $album->ObtenerLaminas();
		}
		return $album;
	}

	/**
	 *
	 * Obtiene las láminas del álbum.
	 * @return LaminaAlbum
	 */
	function ObtenerLaminas()
	{
		$laminas = array();
		if(!$this->EstaRegistrado)
		throw new Exception('El álbum no está registrado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT IdLamina FROM LaminaAlbum WHERE IdAlbum = %s",
		$conexionBd->Escapar($this->Id)
		);
		$datosLaminas = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		if($datosLaminas != null)
		foreach($datosLaminas as $datosLamina)
		{
			$lamina = Lamina::ObtenerLamina($datosLamina['IdLamina']);
			$laminas[] = LaminaAlbum::Obtener($this, $lamina);
		}
		return $laminas;
	}

	/**
	 *
	 * Obtiene las láminas repetidas del álbum.
	 * @return LaminaAlbum
	 */
	function ObtenerLaminasRepetidas()
	{
		$laminasRepetidas = null;
		$laminas = $this->ObtenerLaminas();
		foreach($laminas as $lamina)
		if($lamina->Cantidad > 1)
		{
			if(!is_array($laminasRepetidas))
			$laminasRepetidas = array();
			$laminasRepetidas[] = $lamina;
		}
		return $laminasRepetidas;
	}

	/**
	 *
	 * Obtiene las clasificaciones que tienen láminas nuevas.
	 * o sea, clasificaciones que tienen láminas nuevas del día actual.
	 */
	function ObtenerClasificacionLaminasNuevas()
	{
		$clasificaciones = null;
		if(!$this->EstaRegistrado)
		throw new Exception('EL álbum no está registrado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT DISTINCT Clasificacion FROM Lamina WHERE Id IN (SELECT IdLamina FROM LaminaAlbum WHERE IdAlbum = %s AND FechaAdquisicion = DATE(NOW())) ORDER BY Clasificacion",
		$conexionBd->Escapar($this->Id)
		);
		$datos = $conexionBd->EjecutarConsulta();
		if(is_array($datos) && count($datos) >= 1)
		{
			$clasificaciones = array();
			foreach($datos as $dato)
			$clasificaciones[] = $dato['Clasificacion'];
		}
		return $clasificaciones;
	}
}
?>