<?php
/**
 *
 * Contiene parámetros para la conexión a una base de datos.
 * @author Julián Mejio
 * @version 1.1
 *
 * @property string $Servidor Nombre o dirección IP del Servidor.
 * @property int $Puerto Puerto de escucha del servidor.
 * @property string $Usuario Usuario.
 * @property string $Clave Clave.
 * @property string $BaseDatos Nombre del "schema" de la base de datos.
 * @property string $Codificacion Codificación utilizada para la transferencia de datos.
 * @property string $Tipo Tipo de motor de base de datos.
 */
class ParametrosDb
{
	private $servidor;
	private $puerto;
	private $usuario;
	private $clave;
	private $baseDatos;
	private $codificacion;
	private $tipo;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Servidor':
				$valor = $this->servidor;
				break;
			case 'Puerto':
				$valor = $this->puerto;
				break;
			case 'Usuario':
				$valor = $this->usuario;
				break;
			case 'Clave':
				$valor = $this->clave;
				break;
			case 'BaseDatos':
				$valor = $this->baseDatos;
				break;
			case 'Codificacion':
				$valor = $this->codificacion;
				break;
			case 'Tipo':
				$valor = $this->tipo;
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
			case 'Servidor':
				$this->servidor = $Valor;
				break;
			case 'Puerto':
				$this->puerto = $Valor;
				break;
			case 'Usuario':
				$this->usuario = $Valor;
				break;
			case 'Clave':
				$this->clave = $Valor;
				break;
			case 'BaseDatos':
				$this->baseDatos = $Valor;
				break;
			case 'Codificacion':
				$this->codificacion = $Valor;
				break;
			case 'Tipo':
				$this->tipo = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un contenedor de parámetros de conexión a una base de datos.
	 * @param string $Servidor Nombre o dirección IP del servidor.
	 * @param string $Usuario Usuario.
	 * @param string $BaseDatos Nombre del "schema" de la base de datos.
	 * @param string $Codificacion Codificación a utilizar para la transferencia de datos.
	 * @param int $Puerto Puerto de escucha del servidor.
	 * @param string $Clave Clave.
	 * @param string $Tipo Tipo de motor de base de datos.
	 */
	function __construct($Servidor, $Usuario, $BaseDatos, $Clave = null, $Tipo = 'mysql', $Codificacion = 'utf8', $Puerto = 3306)
	{
		$this->Servidor = $Servidor;
		$this->Puerto = $Puerto;
		$this->Usuario = $Usuario;
		$this->Clave = $Clave;
		$this->Tipo = $Tipo;
		$this->BaseDatos = $BaseDatos;
		$this->Codificacion = $Codificacion;
	}

	/**
	 *
	 * Guarda los parámetros de conexión de manera persistente.
	 * @param string $Nombre Nombre para la configuración de los parámetros. Sólo se permiten letras y números
	 * @param boolean $Sobreescribir Si el valor es verdadero y hay una conexión con el mimso nombre, se sobreescriben esos parámetros por los actuales.
	 */
	function Guardar($Nombre, $Sobreescribir = false)
	{
		$Nombre = strtolower($Nombre);
		if(!preg_match('/[a-z0-9]+/', $Nombre))
		throw new Exception("El nombre para los parámetros de conexión no es válido: \"$Nombre\"");
		$rutaDestino = Uri::ObtenerRutaARaiz() . Config::RutaConexionesDb . "/$Nombre.dbp";
		if(file_exists($rutaDestino) && $Sobreescribir)
		unlink($rutaDestino);
		else if(file_exists($rutaDestino))
		throw new Exception('Ya existe una configuración guardada con el mismo nombre');
		file_put_contents($rutaDestino, Cifrador::Cifrar(((serialize($this)))));
	}

	/**
	 *
	 * Carga parámetros de conexión previamente guardados.
	 * @param string $Nombre Nombre de los parámetros de conexión.
	 * @throws Exception Se arroja cuando EL nombre de los parámetros no es válido o no se encuentran parámetros guardados con ese nombre.
	 * @return ParametrosDb
	 */
	static function Cargar($Nombre)
	{
		$Nombre = strtolower($Nombre);
		if(!preg_match('/[a-z0-9+]/', $Nombre))
		throw new Exception("El nombre para los parámetros de conexión no es válido: \"$Nombre\"");
		$rutaDestino = Uri::ObtenerRutaARaiz() . Config::RutaConexionesDb . "/$Nombre.dbp";
		if(!file_exists($rutaDestino))
		throw new Exception("No hay nignua configuración de conexión con el nombre \"$Nombre\"");
		return unserialize(Cifrador::Descifrar(((file_get_contents($rutaDestino)))));
	}

	static function CargarArchivo($Archivo)
	{
		if(!file_exists($Archivo))
		throw new Exception("No hay nignua configuración de conexión en la ruta $Archivo");
		return unserialize(Cifrador::Descifrar(((file_get_contents($Archivo)))));
	}
}
?>