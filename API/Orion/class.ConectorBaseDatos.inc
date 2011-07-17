<?php
/**
 *
 * Provee métodos de conexión a una base de datos.
 * @author Julián Mejio
 * @version 1.3
 *
 * @property ParametrosDb $Parametros
 * @property int $SentenciasEjecutadas
 * @property string $Sentencia
 * @property boolean $EnTransaccion
 */
class ConectorBaseDatos
{
	/**
	 *
	 * Parámetros de conexión.
	 * @var ParametrosDb
	 */
	private $parametros;
	/**
	 *
	 * Sentencia para ejecutar.
	 * @var string
	 */
	private $sentencia;
	/**
	 *
	 * Número de sentencias que se han  ejecutado.
	 * @var int
	 */
	private $sentenciasEjecutadas;
	/**
	 *
	 * Conexión con la base de datos.
	 * @var mixed
	 */
	private $conexion;
	/**
	 *
	 * Determina si la conexión está en transacción o no.
	 * @var boolean
	 */
	private $enTransaccion;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Parametros':
				$valor = $this->parametros;
				break;
			case 'Sentencia':
				$valor = $this->sentencia;
				break;
			case 'EnTransaccion':
				$valor = $this->enTransaccion;
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
			case 'Parametros':
				$this->parametros = $Valor;
				break;
			case 'Sentencia':
				$this->sentencia = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un nuevo conector a una base de datos.
	 * @param ParametrosDb $Parametros
	 * @param boolean $AutoConectar
	 */
	function __construct(ParametrosDb $Parametros = null, $AutoConectar = true)
	{
		$this->enTransaccion = false;
		$this->conexion = null;
		if($Parametros == null)
		$this->Parametros = ParametrosDb::Cargar(Config::NombreConexionDbGeneral);
		else
		$this->Parametros = $Parametros;
		if(!$this->comprobarCompatiblidadParametros())
		throw new Exception('Los parámetros de la base de datos no son compatibles con el conector');
		if($AutoConectar)
		$this->Conectar();
	}

	/**
	 *
	 * Comprueba los parámetros para verificar si son o no compatibles con el conector.
	 * @return boolean
	 */
	private function comprobarCompatiblidadParametros()
	{
		$compatible = false;
		if($this->Parametros != null && $this->Parametros->Tipo == 'mysql')
		$compatible = true;
		return $compatible;
	}

	/**
	 *
	 * Realiza la conexión a la base de datos.
	 * @param boolean $AutoDesconectar Si el valor es verdadero y hay una conexión activa, se intentará desconectar para realizar la nueva conexión.
	 */
	function Conectar($AutoDesconectar = false)
	{
		if($this->EstaConectado() && $AutoDesconectar)
		$this->Desconectar();
		else if($this->EstaConectado())
		$this->arrojarError('Tiene que cerrar la conexión actual para realizar otra conexión con el mismo conector');
		switch($this->Parametros->Tipo)
		{
			case 'mysql':
				$this->conexion = mysql_pconnect("{$this->Parametros->Servidor}:{$this->Parametros->Puerto}", $this->Parametros->Usuario, $this->Parametros->Clave) or trigger_error(mysql_error(), E_USER_ERROR);
				$this->EjecutarComando(sprintf("SET NAMES '{$this->Parametros->Codificacion}'"));
				mysql_select_db($this->Parametros->BaseDatos);
				break;
			default:
				$this->arrojarError("El tipo de conexión \"{$this->Parametros->Tipo}\" no se admite");
		}
		if($this->conexion == null)
		$this->arrojarError('La conexión no se pudo realizar');
	}

	/**
	 *
	 * Determina si el conector tiene conexión activa con la base de datos.
	 */
	function EstaConectado()
	{
		return ($this->conexion != null);
	}

	/**
	 *
	 * Se desconecta de la base de datos.
	 * @param boolean $ReiniciarContadorSentenciasEjecutadas Si el valor es verdadero, el número de consultas realizadas se establece en 0.
	 */
	function Desconectar($ReiniciarContadorSentenciasEjecutadas = true)
	{
		if(!$this->EstaConectado())
		$this->arrojarError('No se puede realizar la desconexión porque no hay conexión activa');
		mysql_close($this->conexion);
		$this->conexion = null;
		if($ReiniciarContadorSentenciasEjecutadas)
		$this->reestablecerContadorSetneciasRealizadas();
	}

	/**
	 *
	 * Ejecuta una sentencia como comando en la base de datos (no devuelve datos).
	 * @param string $Sentencia Sentencia a ejecutar.
	 * @return integer
	 */
	function EjecutarComando($Sentencia = null)
	{
		$filasAfectadas = 0;
		if(!$this->EstaConectado())
		$this->arrojarError('No hay una conexión activa para ejecutar esta sentencia');
		if($Sentencia == null && $this->Sentencia == null)
		$this->arrojarError('No hay sentencias para ejecutar');
		else if($Sentencia == null)
		$Sentencia = $this->Sentencia;
		$filasAfectadas = mysql_query($Sentencia, $this->conexion) or $this->arrojarError();
		$this->incrementarContadorSentenciasRealizadas();
		if(!is_int($filasAfectadas))
		$filasAfectadas = intval($filasAfectadas);
		return $filasAfectadas;
	}

	/**
	 *
	 * Ejecuta una sentencia en la base de datos.
	 * @param string $Sentencia Sentencia.
	 */
	function EjecutarConsulta($Sentencia = null)
	{
		$resultado = null;
		if(!$this->EstaConectado())
		$this->arrojarError('No hay conexión activa para ejecutar esta sentencia');
		if($Sentencia == null && $this->Sentencia == null)
		$this->arrojarError('No hay sentencias para ejecutar');
		else if($Sentencia == null)
		$Sentencia = $this->Sentencia;
		$resultadosSentencia = mysql_query($Sentencia, $this->conexion) or $this->arrojarError();
		$this->incrementarContadorSentenciasRealizadas();
		if(mysql_num_rows($resultadosSentencia) >= 1)
		{
			$resultado = array();
			$resultadoSentencia = mysql_fetch_assoc($resultadosSentencia);
			do
			$resultado[] = $resultadoSentencia;
			while($resultadoSentencia = mysql_fetch_assoc($resultadosSentencia));
		}
		mysql_free_result($resultadosSentencia);
		return $resultado;
	}

	/**
	 *
	 * Ejecuta una sentencia y devuelve el primer valor de la primera fila que retorne.
	 * @param string $Sentencia Sentencia a ejecutar. Debe contener el nombre de la columna a devolver.
	 */
	function EjecutarEscalar($Sentencia = null)
	{
		$resultado = null;
		if(!$this->EstaConectado())
		$this->arrojarError('No hay conexión activa para ejecutar esta sentencia');
		if($Sentencia == null && $this->Sentencia == null)
		$this->arrojarError('No hay sentencias para ejecutar');
		else if($Sentencia == null)
		$Sentencia = $this->Sentencia;
		$resultadoSentencia = mysql_query($Sentencia, $this->conexion) or $this->arrojarError();
		if(mysql_num_rows($resultadoSentencia) >= 1)
		{
			$resultado = mysql_fetch_row($resultadoSentencia);
			$resultado = $resultado[0];
		}
		mysql_free_result($resultadoSentencia);
		return $resultado;
	}

	/**
	 *
	 * Inicia una transacción con la base de datos.
	 */
	function IniciarTransaccion()
	{
		if($this->EnTransaccion)
		$this->arrojarError('No se puede iniciar una transacción porque ya hay otra en ejecución');
		$this->EjecutarComando('START TRANSACTION');
		$this->enTransaccion = true;
	}

	/**
	 *
	 * Confirma una transacción activa.
	 */
	function ConfirmarTransaccion()
	{
		if(!$this->EnTransaccion)
		$this->arrojarError('No hay transacciones activas que se puedan confirmar');
		$this->EjecutarComando('COMMIT');
		$this->enTransaccion = false;
	}

	/**
	 * Cancela una transacción activa.
	 * Enter description here ...
	 */
	function CancelarTransaccion()
	{
		if(!$this->EnTransaccion)
		$this->arrojarError('No hay transacciones activas que se puedan cancelar');
		$this->EjecutarComando('ROLLBACK');
		$this->enTransaccion = false;
	}

	/**
	 *
	 * Incrementa el contador de sentencias realizadas.
	 * @param int $Incremento Número de unidades que se va a incrmentar el contador.
	 */
	private function incrementarContadorSentenciasRealizadas($Incremento = 1)
	{
		$this->sentenciasEjecutadas += $Incremento;
	}

	/**
	 *
	 * Reestablece el contador de sentencias realizadas.
	 */
	private function reestablecerContadorSetneciasRealizadas()
	{
		$this->sentenciasEjecutadas = 0;
	}

	/**
	 *
	 * Escapa una cadena para ser pasada de manera segura como parte de un comando o consulta.
	 * @param string $Cadena Cadena.
	 * @return string
	 */
	function Escapar($Cadena)
	{
		if(!$this->EstaConectado())
		throw new Exception('No hay una conexión activa a la base de datos, por lo que no se pueden escapar cadenas');
		if($Cadena == '0' || !$Cadena)
		$cadenaSegura = 0;
		else if($Cadena == null)
		$cadenaSegura = 'NULL';
		else if(is_int($Cadena))
		$cadenaSegura = mysql_real_escape_string($Cadena, $this->conexion) or $this->arrojarError();
		else
		{
			$cadenaSegura = mysql_real_escape_string($Cadena, $this->conexion) or $this->arrojarError();
			$cadenaSegura = "'$cadenaSegura'";
		}
		return $cadenaSegura;
	}

	/**
	 *
	 * Arroja un error de base de datos.
	 * @param string $MensajeError Mensaje de error.
	 * @param boolean $Desconectar Permite desconectar o no la conexión activa a la base de datos después de producirse el error.
	 */
	private function arrojarError($MensajeError = null, $Desconectar = true)
	{
		if($this->EnTransaccion)
		$this->CancelarTransaccion();
		//if($_SERVER['REMOTE_ADDR'] != Config::DireccionIpPruebas)
		//	$MensajeError = 'Error general en la base de datos';
		if($MensajeError == null)
		$MensajeError = @mysql_error($this->conexion);
		if($Desconectar)
		$this->Desconectar();
		throw new Exception("Error en el conector de la base de datos: $MensajeError");
	}
}
?>