<?php
/**
 *
 * Clase que contiene los parámetrosde configuración globables del sitio.
 * @author Julián Mejio
 * @version 1.1
 */
class Config
{

	/**
	 *
	 * Ruta base de toda la aplicación
	 * @var string
	 */
	const RutaBase = '/';
	/**
	 *
	 * Ruta del repositorio de scripts del sitio.
	 * @var string
	 */
	const RutaScripts = '/Scripts';
	/**
	 *
	 * Ruta de las imágenes del sitio.
	 * @var string
	 */
	const RutaImagenes = '/Imagenes';

	/**
	 *
	 * Númeor de imágenes aleatorias predeterminadas
	 * @var int
	 */
	const NumeroLaminasAleatoriasPredeterminadas = 3;

	/**
	 *
	 * Nombre de la imagen predeterminada para las láminas.
	 * @var string
	 */
	const NombreImagenLaminaPredet = 'Orion.jpg';
	/**
	 *
	 * Dirección IP del servidor de pruebas, para propósitos de desarrollo y depuración.
	 * @var string
	 */
	const DireccionIpPruebas = '127.0.0.1';

	/**
	 *
	 * Ruta del repositorio de conexiones para las bases de datos.
	 * @var string
	 */
	const RutaConexionesDb = '/Sistema/Datos/ConexionesDb';

	/**
	 *
	 * Ruta a la base de datos de Geoip
	 * @var string
	 */
	const RutaBaseDatosGeoip = '/Sistema/Datos/Geoip/GeoLiteCity.dat';

	/**
	 *
	 * Ruta a la caché del sistema.
	 * @var string
	 */
	const RutaCache = '/Sistema/Cache';

	/**
	 *
	 * Nombre de los parámetros de conexión generales de la base de datos.
	 * @var string
	 */
	const NombreConexionDbGeneral = 'general';

	/**
	 *
	 * Contraseña utilizada par el cifrado interno del sitio.
	 * @var string
	 */
	const ClaveSitio = 'clavesecreta';

	/**
	 * 
	 * Contraseña utilizada para el cifrado de códigos de puntos.
	 * @var int
	 */
	const ClaveCifradorCodigo = '12345';
	
	/**
	 *
	 * Ruta de archivos local.
	 * @var string
	 */
	const RutaLocal = '/ruta/local/a/app';

	/**
	 *
	 * Ruta de archivos remota.
	 * @var string
	 */
	const RutaRemota = '/ruta/remota/a/app';

	/**
	 *
	 * Id de aplicación de Facebook.
	 * @var string
	 */
	const FbAppId = 'idfacebookapp';

	/**
	 *
	 * Secreto de aplicación de Facebook.
	 * @var string
	 */
	const FbSecret = 'clavesecreta';

	/**
	 *
	 * Cantidad de puntos de experiencia que se generan por lámina nueva.
	 * @var int
	 */
	const ExperienciaLaminaNueva = 1;

	/**
	 *
	 * Cantidad de puntos de experiencia que se generan por cada asistencia.
	 * @var int
	 */
	const ExperienciaAsistencia = 3;

	const ServidorSmtp = 'ssl://example.com';
	const UsuarioSmtp = 'smtp@example.com';
	const ClaveSmtp = 'secret';

	/**
	 *
	 * Obtiene la ruta respectiva según el servidor asociado.
	 */
	static function ObtenerRutaArchivos()
	{
		$rutaArchivos = (is_dir(Config::RutaRemota)) ? Config::RutaRemota : Config::RutaLocal;
	}
}
?>