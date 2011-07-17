<?php
/**
 *
 * Representa una lámina de álbum.
 * @author Julián Mejio
 * @version 1.0-fx1
 *
 * @property int $Id
 * @property string $Nombre
 * @property string $Clasificacion
 * @property string $Descripcion
 * @property float $Abundancia
 * @property date $FechaRegistro
 * @property PropiedadLamina $Propiedades
 * @property bool $EstaRegistrada
 * @property string $NombreImagen
 */
class Lamina
{
	private $id;
	private $nombre;
	private $clasificacion;
	private $descripcion;
	private $abundancia;
	private $fechaRegistro;
	private $propiedades;
	private $nombreImagen;

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
			case 'Clasificacion':
				$valor = $this->clasificacion;
				break;
			case 'Descripcion':
				$valor = $this->descripcion;
				break;
			case 'Abundancia':
				$valor = $this->abundancia;
				break;
			case 'FechaRegistro':
				$valor = $this->fechaRegistro;
				break;
			case 'Propiedades':
				$valor = $this->propiedades;
				break;
			case 'EstaRegistrada':
				$valor = ($this->id != null);
				break;
			case 'NombreImagen':
				$valor = $this->nombreImagen;
				break;
			case 'NombreImagenGrande':
				if($this->NombreImagen != null)
				{
					$esImg = preg_replace('/\.jpg$/', '', $this->NombreImagen);
					$valor = "$esImg-o.jpg";
				}
				else
				$valor = null;
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
			case 'Clasificacion':
				$this->clasificacion = $Valor;
				break;
			case 'Descripcion':
				$this->descripcion = $Valor;
				break;
			case 'Abundancia':
				$this->abundancia = $Valor;
				break;
			case 'Propiedades':
				$this->propiedades = $Valor;
				break;
			case 'NombreImagen':
				$this->nombreImagen = $Valor;
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea una lámina
	 * @param string $Nombre
	 * @param string $Clasificacion
	 * @param string $Descripcion
	 * @param float $Abundancia
	 */
	function __construct($Nombre, $Clasificacion, $Descripcion, $Abundancia, $NombreImagen = null)
	{
		$this->Nombre = $Nombre;
		$this->Clasificacion = $Clasificacion;
		$this->Descripcion = $Descripcion;
		$this->Abundancia = $Abundancia;
		$this->Propiedades = array();
		$this->NombreImagen = $NombreImagen;
	}

	/**
	 *
	 * Registra la lámina en la base de datos.
	 * @throws Exception
	 */
	public function Registrar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('CrearLaminaAlbum'))
		throw new Exception('No tiene permiso de crear nuevas láminas');
		if($this->EstaRegistrada)
		throw new Exception('La lámina ya está registrada');
		$fechaRegistro = date("Y-m-d");
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT Lamina (Nombre, Clasificacion, Descripcion, Abundancia, FechaRegistro, NombreImagen) VALUES (%s, %s, %s, %s, %s, %s)",
		$conexionBd->Escapar($this->Nombre),
		$conexionBd->Escapar($this->Clasificacion),
		$conexionBd->Escapar($this->Descripcion),
		$conexionBd->Escapar($this->Abundancia),
		$conexionBd->Escapar($fechaRegistro),
		$conexionBd->Escapar($this->NombreImagen)
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		$conexionBd->Desconectar();
		$this->actualizarPropiedades();
		$this->fechaRegistro = $fechaRegistro;
	}

	/**
	 *
	 * Actualiza la lámina con los datos recién cambiados.
	 * @throws Exception
	 */
	public function Actualizar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('ModificarLaminaAlbum'))
		throw new Exception('No tiene permiso para modificar las láminas del álbum');
		if(!$this->EstaRegistrada)
		throw new Exception('La lámina no está registrada');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Lamina SET Nombre = %s, Clasificacion = %s, Descripcion = %s, Abundancia = %s, NombreImagen = %s WHERE Id = %s LIMIT 1",
		$conexionBd->Escapar($this->Nombre),
		$conexionBd->Escapar($this->Clasificacion),
		$conexionBd->Escapar($this->Descripcion),
		$conexionBd->Escapar($this->Abundancia),
		$conexionBd->Escapar($this->NombreImagen),
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$this->actualizarPropiedades();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Actualiza las propiedades de la lámina.
	 * @throws Exception
	 */
	private function actualizarPropiedades()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('ModificarLaminaAlbum'))
		throw new Exception('No tiene permiso para modificar las láminas del álbum');
		if(!$this->EstaRegistrada)
		throw new Exception('La lámina no está registrada');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Tipo, Nombre, Valor FROM PropiedadLamina WHERE IdLamina = %u",
		$conexionBd->Escapar($this->Id)
		);
		$propiedadesRegistradas = $conexionBd->EjecutarConsulta();
		$procesadas = array();
		if($propiedadesRegistradas != null)
		foreach($propiedadesRegistradas as $propiedadBd)
		{
			$existe = false;
			foreach($this->Propiedades as $propiedad)
			if($propiedad->Tipo == $propiedadBd['Tipo'] && $propiedad->Nombre == $propiedadBd['Nombre'])
			{
				$existe = true;
				if($propiedad->Valor != $propiedadBd['Valor'])
				$conexionBd->EjecutarComando(sprintf("UPDATE PropiedadLamina SET Valor = '%s' WHERE IdLamina = %u AND Tipo = '%s' AND Nombre = '%s'",
				$conexionBd->Escapar($propiedad->Valor),
				$conexionBd->Escapar($this->Id),
				$conexionBd->Escapar($propiedad->Tipo),
				$conexionBd->Escapar($propiedad->Nombre)
				));
				$procesadas[] = $propiedad;
				break;
			}
			if(!$existe)
			$conexionBd->EjecutarComando(sprintf("DELETE FROM PropiedadLamina WHERE IdLamina = %u AND Tipo = %s AND Nombre = %s",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($propiedad['Tipo']),
			$conexionBd->Escapar($propiedad['Nombre'])
			));
		}
		if(count($procesadas) < count($this->Propiedades))
		foreach($this->Propiedades as $propiedad)
		if(!in_array($propiedad, $procesadas))
		$conexionBd->EjecutarComando(sprintf("INSERT PropiedadLamina (IdLamina, Tipo, Nombre, Valor) VALUES (%s, %s, %s, %s)",
		$conexionBd->Escapar($this->Id),
		$conexionBd->Escapar($propiedad->Tipo),
		$conexionBd->Escapar($propiedad->Nombre),
		$conexionBd->Escapar($propiedad->Valor)
		));
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Obtiene las propiedades de la lámina.
	 */
	private function obtenerPropiedades()
	{
		$propiedades = array();
		/*
		 $conexionBd = new ConectorBaseDatos();
		 $conexionBd->Sentencia = sprintf("SELECT Tipo, Nombre, Valor FROM PropiedadLamina WHERE IdLamina = %u ORDER BY Tipo",
			$conexionBd->Escapar($this->Id)
			);
			$datosPropiedades = $conexionBd->EjecutarConsulta();
			if($datosPropiedades != null)
			foreach($datosPropiedades as $datos)
			$propiedades[] = new PropiedadLamina($this, $datos['Tipo'], $datos['Nombre'], $datos['Valor']);
			*/
		return $propiedades;
	}

	/**
	 *
	 * Obtiene una lámina por su ID.
	 * @param int $Id
	 * @return Lamina
	 */
	public static function ObtenerLamina($Id)
	{
		$lamina = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Nombre, Clasificacion, Descripcion, Abundancia, FechaRegistro, NombreImagen FROM Lamina WHERE Id = %s",
		$conexionBd->Escapar($Id)
		);
		$datosLamina = $conexionBd->EjecutarConsulta();
		if($datosLamina != null)
		{
			$datosLamina = $datosLamina[0];
			$lamina = new Lamina(stripslashes($datosLamina['Nombre']), stripslashes($datosLamina['Clasificacion']), stripslashes($datosLamina['Descripcion']), stripslashes($datosLamina['Abundancia']), stripslashes($datosLamina['NombreImagen']));
			$lamina->fechaRegistro = stripslashes($datosLamina['FechaRegistro']);
			$lamina->id = stripslashes($Id);
			$lamina->Propiedades = $lamina->obtenerPropiedades();
		}
		return $lamina;
	}

	/**
	 *
	 * Obtiene el nombre de las clasificaciones y la cantidad de láminas que tiene.
	 * @return array
	 */
	public static function ObtenerClasificaciones()
	{
		$clasificaciones = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT Clasificacion, COUNT(*) AS Cantidad FROM Lamina GROUP BY Clasificacion ORDER BY Clasificacion';
		$clasificaciones = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		return $clasificaciones;
	}

	/**
	 *
	 * Obtiene un arreglo de clasificaciones filtrados por una parte
	 * @param string $Parte
	 */
	public static function FiltrarClasificaciones($Parte)
	{
		$clasificaciones = array();
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT DISTINCT Clasificacion FROM Lamina WHERE Clasificacion REGEXP %s",
		$conexionBd->Escapar($Parte)
		);
		$datos = $conexionBd->EjecutarConsulta();
		if(is_array($datos) && count($datos) >= 1)
		foreach($datos as $dato)
		$clasificaciones[] = stripslashes($dato['Clasificacion']);
		$conexionBd->Desconectar();
		return $clasificaciones;
	}

	/**
	 *
	 * Obtiene las láminas de acuerdo a su clasificación.
	 * @param string $Clasificacion
	 * @return Lamina $Laminas
	 */
	public static function ObtenerLaminasPorClasificacion($Clasificacion)
	{
		$laminas = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Id FROM Lamina WHERE Clasificacion = %s ORDER BY Orden, Id",
		$conexionBd->Escapar($Clasificacion)
		);
		$ids = $conexionBd->EjecutarConsulta();
		if($ids != null)
		{
			$laminas = array();
			foreach($ids as $id)
			{
				$lamina = Lamina::ObtenerLamina($id['Id']);
				$laminas[] = $lamina;
			}
		}
		return $laminas;
	}

	/**
	 *
	 * Obtiene las láminas que están registradas en el álbum, ordenadas por clasificación.
	 */
	public static function ObtenerLaminas()
	{
		$laminas = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT Id FROM Lamina ORDER BY Orden, Clasificacion, Abundancia';
		$ids = $conexionBd->EjecutarConsulta();
		if($ids != null)
		{
			$laminas = array();
			foreach($ids as $id)
			{
				$lamina = Lamina::ObtenerLamina($id['Id']);
				$laminas[] = $lamina;
			}
		}
		return $laminas;
	}

	/**
	 *
	 * Obtiene una cantidad N de láminas aleatoriamente.
	 * @param int $Cantidad
	 * @return Lamina
	 */
	public static function ObtenerLaminasAleatorias($Cantidad = null)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!is_int($Cantidad))
			$Cantidad = Config::NumeroLaminasAleatoriasPredeterminadas;
		$laminas = array();
		$abundancia = Lamina::convertirProbabilidad(rand(1, 100));
		$conexionBd = new ConectorBaseDatos();
		if($usuario->EsMiembroClubOrion)
			$conexionBd->Sentencia = sprintf("SELECT Id FROM Lamina WHERE Abundancia >= %s",
				$conexionBd->Escapar($abundancia)
			);
		else
			$conexionBd->Sentencia = sprintf("SELECT Id FROM Lamina WHERE Abundancia >= %s AND Clasificacion <> 'Hall Of Fame'",
				$conexionBd->Escapar($abundancia)
			);
		$datosLaminas = $conexionBd->EjecutarConsulta();
		if($datosLaminas != null)
			for($i = 1; $i <= $Cantidad; $i++)
				$laminas[] = Lamina::ObtenerLamina($datosLaminas[rand(0, count($datosLaminas) - 1)]['Id']);
		return $laminas;
	}

	/**
	 *
	 * Obtiene el número de láminas que hay regitradas.
	 * @return int
	 */
	public static function ObtenerNumeroLaminasRegistradas()
	{
		$numero = 0;
		$conexionBd = new ConectorBaseDatos();
		$numero = $conexionBd->EjecutarEscalar('SELECT COUNT(*) FROM Lamina');
		$conexionBd->Desconectar();
		return $numero;
	}

	public function ObtenerRutaHttpImagen()
	{
		if(!isset($_SESSION['ilguid']))
		$_SESSION['ilguid'] = microtime(true);
		$nombreImagen = $this->NombreImagen;
		if($nombreImagen == null)
		$nombreImagen = Config::NombreImagenLaminaPredet;
		$idImagenUsuario = sha1($_SESSION['ilguid'] . $nombreImagen);
		$_SESSION[$idImagenUsuario] = $nombreImagen;
		$ruta = Config::RutaBase . "/lamina/$idImagenUsuario.jpg";
		return $ruta;
	}

	public function ObtenerRutaHttpImagenGrande()
	{
		if(!isset($_SESSION['ilguid']))
		$_SESSION['ilguid'] = microtime(true);
		$nombreImagen = $this->NombreImagenGrande;
		if($nombreImagen == null)
		$nombreImagen = Config::NombreImagenLaminaPredet;
		$idImagenUsuario = sha1($_SESSION['ilguid'] . $nombreImagen);
		$_SESSION[$idImagenUsuario] = $nombreImagen;
		$ruta = Config::RutaBase . "/lamina/$idImagenUsuario.jpg";
		return $ruta;
	}

	/**
	 *
	 * Convierte un número porcentual en probabilidad.
	 * @param byte $Porcentaje
	 * @throws Exception
	 */
	private static function convertirProbabilidad($Porcentaje)
	{
		if($Porcentaje <= 0 || $Porcentaje > 100)
		throw new Exception('El porcentaje de probabilidad tiene un valor inválido');
		return round(log($Porcentaje) / 4.6, 1);
	}

	/**
	 *
	 * Genera un ítem de lámina vacío que puede renderizarse en formato HTML.
	 * @return EtiquetaXml
	 */
	public static function GenerarItemVacioX()
	{
		$item = new EtiquetaXml('div');
		$item->AgregarAtributo(new AtributoXml('class', 'Lamina N ui-corner-all'));
		return $item;
	}

	/**
	 *
	 * Genera un ítem que sirve de información gráfica para los intercambios.
	 * @return EtiquetaXml
	 */
	public function GenerarItemIntX()
	{
		$item = new EtiquetaXml('div');
		$tipoLamina = 'MA';
		switch ($this->Abundancia)
		{
			case '0':
			case '0.1':
			case '0.2':
				$tipoLamina = 'U';
				break;
			case '0.3':
			case '0.4':
				$tipoLamina = 'ME';
				break;
			case '0.5':
			case '0.6':
			case '0.7':
				$tipoLamina = 'E';
				break;
			case '0.8':
			case '0.9':
				$tipoLamina = 'A';
				break;
			case '1':
			case '1.0':
			default:
				$tipoLamina = 'MA';
				break;
		}
		$item->AgregarAtributo(new AtributoXml('class', "LaminaIntercambio $tipoLamina ui-corner-all"));
		$item->AgregarContenido($this->Nombre);
		return $item;
	}

	/**
	 *
	 * Genera un ítem que puede renderizarse en formato HTML.
	 * @return EtiquetaXml
	 */
	public function GenerarItemX()
	{
		$item = null;
		$laminaX = new EtiquetaXml('div');
		$tipoLamina = 'MA';
		switch ($this->Abundancia)
		{
			case '0':
			case '0.1':
			case '0.2':
				$tipoLamina = 'U';
				break;
			case '0.3':
			case '0.4':
				$tipoLamina = 'ME';
				break;
			case '0.5':
			case '0.6':
			case '0.7':
				$tipoLamina = 'E';
				break;
			case '0.8':
			case '0.9':
				$tipoLamina = 'A';
				break;
			case '1':
			case '1.0':
			default:
				$tipoLamina = 'MA';
				break;
		}
		$laminaX->AgregarAtributo(new AtributoXml('class', "Lamina $tipoLamina ui-corner-all NoVacia"));
		$cabecera = new EtiquetaXml('div');
		$cabecera->AgregarAtributo(new AtributoXml('class', 'Cabecera'));
		$nombreLamina = new EtiquetaXml('div');
		$nombreLamina->AgregarAtributo(new AtributoXml('class', 'Nombre ui-corner-all'));
		$nombreLamina->AgregarContenido(new EtiquetaXml('span', $this->Nombre));
		$cabecera->AgregarContenido($nombreLamina);
		$imagen = new EtiquetaXml('div');
		$imagen->AgregarAtributo(new AtributoXml('class', 'Imagen ui-corner-all'));
		$imagen->AgregarAtributo(new AtributoXml('style', 'background: #000 url(' . $this->ObtenerRutaHttpImagen() . ') no-repeat scroll center top'));
		$vinculoImagen = new EtiquetaXml('a');
		$vinculoImagen->AgregarAtributo(new AtributoXml('href', $this->ObtenerRutaHttpImagenGrande()));
		$vinculoImagen->AgregarAtributo(new AtributoXml('rel', 'rtimg'));
		$brilloImagen = new EtiquetaXml('div');
		$brilloImagen->AgregarAtributo(new AtributoXml('class', 'BrilloLamina ui-corner-all'));
		$vinculoImagen->AgregarContenido($brilloImagen);
		$imagen->AgregarContenido($vinculoImagen);
		$cabecera->AgregarContenido($imagen);
		$laminaX->AgregarContenido($cabecera);
		$propiedades = new EtiquetaXml('div');
		$propiedades->AgregarAtributo(new AtributoXml('class', 'Propiedades ui-corner-all'));
		$propiedades->AgregarContenido(new EtiquetaXml('span', $this->Descripcion));
		/*
		 $descripcion = new EtiquetaXml('div');
		 $descripcion->AgregarAtributo(new AtributoXml('class', 'Descripcion'));
		 $descripcion->AgregarContenido($this->Descripcion);
		 $propiedades->AgregarContenido($descripcion);
		 if($this->Propiedades != null || count($this->Propiedades) > 0)
		 {
			$tipoPropiedades = null;
			$tablaPropiedades = null;
			$cuerpoTabla = null;
			$tipoActual = '';
			foreach($this->Propiedades as $propiedad)
			{
			if($tipoActual != $propiedad->Tipo)
			{
			$tipoActual = $propiedad->Tipo;
			$tipoPropiedades = new EtiquetaXml('div', $tipoActual);
			$tipoPropiedades->AgregarAtributo(new AtributoXml('class', 'NombrePropiedad'));
			$tablaPropiedades = new EtiquetaXml('table');
			$cuerpoTabla = new EtiquetaXml('tbody');
			$tablaPropiedades->AgregarContenido($cuerpoTabla);
			$propiedades->AgregarContenido($tipoPropiedades);
			$propiedades->AgregarContenido($tablaPropiedades);
			}
			$itemPropiedad = new EtiquetaXml('tr');
			$nombrePropiedad = new EtiquetaXml('td', $propiedad->Nombre);
			$valorPropiedad = new EtiquetaXml('td', $propiedad->Valor);
			$itemPropiedad->AgregarContenido($nombrePropiedad);
			$itemPropiedad->AgregarContenido($valorPropiedad);
			$cuerpoTabla->AgregarContenido($itemPropiedad);
			}
			}
			*/
		$laminaX->AgregarContenido($propiedades);
		$item = $laminaX;
		return $item;
	}

	/**
	 *
	 * Genera el contenedor de descripción larga
	 * @return EtiquetaXml
	 */
	static function GenerarContenedorDescripcion()
	{
		$contenedor = new EtiquetaXml('div');
		$contenedor->AgregarAtributo(new AtributoXml('id', 'InformacionLamina'));
		$contenedor->AgregarAtributo(new AtributoXml('class', 'LaminaDescripcionCompleta'));
		$titulo = new EtiquetaXml('p');
		$titulo->AgregarAtributo(new AtributoXml('class', 'TituloLamina'));
		$texto = new EtiquetaXml('div');
		$texto->AgregarAtributo(new AtributoXml('class', 'Texto'));
		$contenedor->AgregarContenido($titulo);
		$contenedor->AgregarContenido($texto);
		return $contenedor;
	}

	/**
	 *
	 * Elimina una lámina del contenido del álbum.
	 * @throws Exception Se arroja cuando no se tiene permiso para realizar esta acción o la lámina no está registrada
	 */
	public function Eliminar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('EliminarLaminaAlbum'))
		throw new Exception('No tiene permiso para eliminar láminas del álbum');
		if(!$this->EstaRegistrada)
		throw new Exception('Esta lámina no está registrada');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->IniciarTransaccion();
		$conexionBd->Sentencia = sprintf("DELETE FROM LaminaAlbum WHERE IdLamina = %s",
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Sentencia = sprintf("DELETE FROM Lamina WHERE Id = %s",
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->ConfirmarTransaccion();
		$conexionBd->Desconectar();
		$rutaImagen = $rutaImagen =  Uri::ObtenerRutaARaiz() . Config::RutaImagenes . '/Laminas/';
		@unlink("$rutaImagen$this->NombreImagen");
		@unlink("$rutaImagen$this->NombreImagenGrande");
	}
}
?>