<?php
/**
 *
 * Representa un usuario identificado en Facebook.
 * @author Julián Mejio
 * @version 1.12
 * 
 * @property string $IdFacebook
 * @property mixed $DatosFacebook
 * @property bool $EsMiembroClubOrion
 * @property string $ValidadoPorIdFacebook
 * @property bool $EsSuperUsuario
 * @property Experiencia $Experiencia
 */
class UsuarioFacebook
{
	private $idFacebook;
	private $datosFacebook;
	private $validadoPorIdFacebook;
	private $esSuperUsuario;
	private $experiencia;
	private $accessToken;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'IdFacebook':
				$valor = $this->idFacebook;
				break;
			case 'DatosFacebook':
				if($this->datosFacebook == null)
				throw new Exception("Al parecer, el usuario $this->IdFacebook de Facebook no autorizó esta aplicación");
				$valor = $this->datosFacebook;
				break;
			case 'ValidadoPorIdFacebook':
				$valor = $this->validadoPorIdFacebook;
				break;
			case 'EsMiembroClubOrion':
				$valor = ($this->ValidadoPorIdFacebook != null);
				break;
			case 'EsSuperUsuario':
				$valor = $this->esSuperUsuario;
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
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un usuario registrado desde Facebook.
	 * @param string $IdFacebook
	 */
	function __construct($IdFacebook)
	{
		$this->validadoPorIdFacebook = null;
		$this->idFacebook = $IdFacebook;
		if(($datos = $this->obtenerRegistroLocal()) != null)
		{
			$this->datosFacebook = unserialize(stripslashes($datos['DatosFacebook']));
			$this->validadoPorIdFacebook = $datos['ValidadoPor'];
			$this->esSuperUsuario = $datos['SuperUsuario'];
			$this->accessToken = $datos['FbAccessToken'];
			$this->experiencia = new Experiencia($this);
		}
		else
		{
			/* Mala idea :$ jajajaja
			 // Restricción de registro por ciudad.
			 $geoIp = geoip_open(Uri::ObtenerRutaARaiz() . Config::RutaBaseDatosGeoip, GEOIP_STANDARD);
			 $infoIp = geoip_record_by_addr($geoIp, $_SERVER['REMOTE_ADDR']);
			 geoip_close($geoIp);
			 if($infoIp->country_code != 'CO' || utf8_encode($infoIp->city) != 'Medellín')
				throw new Exception('Gracias por tu interés en esta aplicación, pero debes registrate en Medellín, Colombia. Eventualmente estaremos abriendo el registro a otras ciudades y países');
				// Fin de restricción de registro por ciudad.
				*/
			$this->cargarDatosFacebook();
			$this->registrarLocalmente();
		}
	}

	/**
	 *
	 * Marca al usuario como integrante del club. No confundir con miembro oficial.
	 */
	function Integrar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('IntegrarUsuarios'))
		throw new Exception('No puede integrar usuarios al club porque no tiene permisos para hacerlo');
		if($this->EsMiembroClubOrion)
		throw new Exception('Este usuario ya está vinculado al club');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE UsuarioFacebook SET ValidadoPor = %s WHERE IdFacebook = %s",
		$conexionBd->Escapar($usuario->IdFacebook),
		$conexionBd->Escapar($this->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->validadoPorIdFacebook = $usuario->IdFacebook;
		Grupo::RegistrarEnPredeterminados($this);
	}

	/**
	 *
	 * Retira la marca de integrante al usuario. No confundir integrante con miembro oficial.
	 */
	function Desintegrar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('IntegrarUsuarios'))
		throw new Exception('No puede desvincular este usuario del club porque no tiene permiso para hacerlo');
		if(!$this->EsMiembroClubOrion)
		throw new Exception('Este usuario ya está desvinculado del club');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->IniciarTransaccion();
		// Eliminar permisos
		$conexionBd->Sentencia = sprintf("DELETE FROM PermisoUsuario WHERE Usuario = %s",
		$conexionBd->Escapar($this->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		// Eliminar asociaciones a los grupos
		$conexionBd->Sentencia = sprintf("DELETE FROM UsuarioGrupo WHERE Usuario = %s",
		$conexionBd->Escapar($this->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Sentencia = sprintf("UPDATE UsuarioFacebook SET ValidadoPor = NULL WHERE IdFacebook = %s",
		$conexionBd->Escapar($this->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->ConfirmarTransaccion();
		$conexionBd->Desconectar();
		$this->validadoPorIdFacebook = null;
	}

	/**
	 *
	 * Obtiene el registro local del usuario, o devuelve un valor nulo.
	 * @return Array
	 */
	private function obtenerRegistroLocal()
	{
		$registroLocal = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT DatosFacebook, ValidadoPor, SuperUsuario, FbAccessToken FROM UsuarioFacebook WHERE IdFacebook = %s",
		$conexionBd->Escapar($this->IdFacebook)
		);
		$registroLocal = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		if(count($registroLocal) == 1)
		$registroLocal = $registroLocal[0];
		else
		$registroLocal = null;
		return $registroLocal;
	}

	/**
	 *
	 * Registra el usuario en la base de datos local.
	 */
	private function registrarLocalmente()
	{
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT UsuarioFacebook (IdFacebook, DatosFacebook, ValidadoPor, FbAccessToken) VALUES (%s, %s, NULL, %s)",
		$conexionBd->Escapar($this->IdFacebook),
		$conexionBd->Escapar(addslashes(serialize($this->DatosFacebook))),
		$conexionBd->Escapar($this->accessToken)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Actualiza los datos locales con los más recientes de Facebook.
	 */
	function ActualizarDatosFacebook()
	{
		$this->cargarDatosFacebook();
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE UsuarioFacebook SET DatosFacebook = %s, FbAccessToken = %s WHERE IdFacebook = %s",
		$conexionBd->Escapar(serialize($this->DatosFacebook)),
		$conexionBd->Escapar($this->accessToken),
		$conexionBd->Escapar($this->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Carga los datos desde Facebook.
	 */
	private function cargarDatosFacebook()
	{
		$this->datosFacebook = null;
		$facebook = new Facebook(FacebookConfig::ObtenerConfiguracionAplicacion());
		$idUsuarioActivo = $facebook->getUser();
		if($idUsuarioActivo != 0 && $idUsuarioActivo == $this->IdFacebook)
		$this->accessToken = $facebook->getAccessToken();
		if($this->IdFacebook)
		$this->datosFacebook = $this->ApiAuth("/$this->IdFacebook");
	}

	/**
	 *
	 * Realiza una llamada al API de Facebook usando el access_token registrado.
	 * @param string $Api
	 * @param string $Metodo
	 * @throws Exception Se arroja cuando no se encuentra el access_token.
	 */
	public function ApiAuth($Api, $Metodo = 'GET', $Parametros = null)
	{
		$datos = null;
		if(!is_array($Parametros))
		$Parametros = array();
		if(!isset($Parametros['access_token']) && $this->accessToken != null)
		$Parametros['access_token'] = $this->accessToken;
		$facebook = new Facebook(FacebookConfig::ObtenerConfiguracionAplicacion());
		if(count($Parametros) >= 1)
		$datos = $facebook->api($Api, $Metodo, $Parametros);
		else
		$datos = $facebook->api($Api, $Metodo);
		return $datos;
	}

	/**
	 *
	 * Obtiene los permisos que el usuario ha concedido a la aplicación.
	 * @return Array
	 */
	function ObtenerPermisosFbConcedidos()
	{
		$permisosConcedidos = null;
		try
		{
			$permisos = $this->ApiAuth("/$this->IdFacebook/permissions");
			if(isset($permisos['data']) && count($permisos['data']) >= 1)
			$permisos = $permisos['data'][0];
			else
			$permisos = array();
			if(is_array($permisos) && count($permisos) >= 1)
			{
				$permisosConcedidos = array();
				foreach($permisos as $k => $v)
				$permisosConcedidos[] = $k;
			}
		} catch(Exception $e) { }
		return $permisosConcedidos;
	}

	/**
	 *
	 * Verifica si el usuario ha concedido derehcos a la aplicación o no.
	 */
	function ObtenerEstadoVFb()
	{
		$valido = '¡Debe revalidar!';
		if($this->accessToken != null)
		$valido = 'Válido';
		return $valido;
	}

	/**
	 *
	 * Verifica los permisos en offline_access.
	 */
	function VerificarOfflinePermFb()
	{
		$valido = false;
		try
		{
			$amigos = $this->ApiAuth("/$this->IdFacebook/friends");
			if(is_array($amigos) && count($amigos) >= 1)
			$valido = true;
		}
		catch(Exception $e) {}
		return $valido;
	}

	/**
	 *
	 * Obtiene el usuario de Facebook que tenga la sesión activa y esté autorizado.
	 * @return UsuarioFacebook
	 */
	static function ObtenerUsuarioActivo()
	{
		$usuarioActivo = null;
		$facebook = new Facebook(FacebookConfig::ObtenerConfiguracionAplicacion());
		$usuario = $facebook->getUser();
		if($usuario)
		$usuarioActivo = new UsuarioFacebook($usuario);
		return $usuarioActivo;
	}

	/**
	 *
	 * Genera un ítem XML del cual se puede renderizar un elemento HTML preformateado con la información de lista del usuario.
	 * @return EtiquetaXml
	 */
	function GenerarItemX()
	{
		$informacion = new EtiquetaXml('div');
		$informacion->AgregarAtributo(new AtributoXml('class', 'ContenedorInformacionUsuarioFb'));
		$nombreUsuario = new EtiquetaXml('p');
		if($this->EsMiembroClubOrion)
		$nombreUsuario->AgregarAtributo(new AtributoXml('class', 'NombreUsuarioFacebook IconoMembresia'));
		else
		$nombreUsuario->AgregarAtributo(new AtributoXml('class', 'NombreUsuarioFacebook'));
		$linkUsuario = new EtiquetaXml('a');
		$linkUsuario->AgregarAtributo(new AtributoXml('href', $this->DatosFacebook['link']));
		$linkUsuario->AgregarAtributo(new AtributoXml('target', '_blank'));
		$linkUsuario->AgregarContenido($this->DatosFacebook['name']);
		$nombreUsuario->AgregarContenido($linkUsuario);
		$informacion->AgregarContenido($nombreUsuario);
		if($this->EsMiembroClubOrion)
		{
			$membresia = new EtiquetaXml('p');
			$membresia->AgregarAtributo(new AtributoXml('class', 'EstadoMembresia'));
			$membresia->AgregarContenido('Miembro');
			$informacion->AgregarContenido($membresia, true);
		}
		$imagen = new EtiquetaXml('div');
		$imagen->AgregarAtributo(new AtributoXml('class', 'ContenedorImagenUsuarioFb ui-corner-all'));
		$imagen->AgregarAtributo(new AtributoXml('style', "background: url(http://graph.facebook.com/$this->IdFacebook/picture) no-repeat top left"));
		$item = new EtiquetaXml('div');
		$item->AgregarAtributo(new AtributoXml('class', 'ItemUsuarioFacebookX'));
		$item->AgregarContenido($imagen);
		$item->AgregarContenido($informacion, true);
		$clear = new EtiquetaXml('div');
		$clear->AgregarAtributo(new AtributoXml('class', 'ClearHelper'));
		$clear2 = new EtiquetaXml('div');
		$clear2->AgregarAtributo(new AtributoXml('class', 'ClearHelper'));
		$item->AgregarContenido($clear, true);
		$item->AgregarContenido($this->Experiencia->GenerarBarraProgresoX());
		$item->AgregarContenido($clear2);
		return $item;
	}

	/**
	 *
	 * Genera un ítem XML que puede renderizar un elemento HTML preformatedo. Ideal para listas, a diferencia de UsuarioFacebook::GenerarItemX()
	 */
	function GenerarItemXInLine()
	{
		$informacion = new EtiquetaXml('div');
		$informacion->AgregarAtributo(new AtributoXml('class', 'ItemUsuarioFacebookX'));
		$linkUsuario = new EtiquetaXml('a');
		$linkUsuario->AgregarAtributo(new AtributoXml('href', $this->DatosFacebook['link']));
		$linkUsuario->AgregarAtributo(new AtributoXml('target', '_blank'));
		$linkUsuario->AgregarContenido($this->DatosFacebook['name']);
		$informacion->AgregarContenido($linkUsuario);
		$informacion->AgregarContenido($this->Experiencia->GenerarNivelXInLine());
		return $informacion;
	}

	/**
	 *
	 * Encuentra un usuario basado en un datos dentro del perfil de Facebook.
	 * @param string $DatoBusqueda
	 * @return
	 */
	public static function EncontrarUsuarios($DatoBusqueda)
	{
		$usuarios = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT IdFacebook FROM UsuarioFacebook WHERE DatosFacebook REGEXP %s",
		$conexionBd->Escapar($DatosBusqueda)
		);
		$datosUsuarios = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		if(is_array($datosUsuarios) && count($datosUsuario) >= 1)
		{
			$usuarios = array();
			foreach($datosUsuarios as $datosUsuario)
			$usuarios[] = new UsuarioFacebook($datosUsuario['IdFacebook']);
		}
		return $usuarios;
	}

	/**
	 *
	 * Obtiene la lista de usuarios que se han registrado en la aplicación.
	 * @return UsuarioFacebook
	 */
	public static function ObtenerUsuariosRegistrados()
	{
		$usuariosRegistrados = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT IdFacebook FROM UsuarioFacebook';
		$idUsuarios = $conexionBd->EjecutarConsulta();
		if($idUsuarios != null && count($idUsuarios) >= 1)
		{
			$usuariosRegistrados = array();
			foreach($idUsuarios as $idUsuario)
			$usuariosRegistrados[] = new UsuarioFacebook($idUsuario['IdFacebook']);
		}
		return $usuariosRegistrados;
	}

	/**
	 *
	 * Obtiene la lista de miembros del Club.
	 */
	public static function ObtenerMiembros()
	{
		$miembros = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT IdFacebook FROM UsuarioFacebook WHERE ValidadoPor IS NOT NULL';
		$idUsuarios = $conexionBd->EjecutarConsulta();
		if($idUsuarios != null && count($idUsuarios) >= 1)
		{
			$miembros = array();
			foreach($idUsuarios as $idUsuario)
			$miembros[] = new UsuarioFacebook($idUsuario['IdFacebook']);
		}
		for($i = 0; $i < count($miembros) - 1; $i++)
		for($j = $i + 1; $j < count($miembros); $j++)
		if($miembros[$i]->DatosFacebook['name'] > $miembros[$j]->DatosFacebook['name'])
		{
			$mt = $miembros[$i];
			$miembros[$i] = $miembros[$j];
			$miembros[$j] = $mt;
		}
		$conexionBd->Desconectar();
		return $miembros;
	}

	/**
	 *
	 * Obtiene la cantidad de puntos disponibles de un usuario.
	 * @return int
	 */
	public function ObtenerPuntosDisponibles()
	{
		return ItemPuntaje::ObtenerPuntaje($this);
	}

	/**
	 *
	 * Gestiona el puntaje del usuario.
	 * @param ItemPuntaje $ItemPuntaje
	 */
	public function GestionarPuntos(ItemPuntaje $ItemPuntaje)
	{
		$ItemPuntaje->Registrar($this);
	}

	/**
	 *
	 * Obtiene la URL de cierre de sesión de Facebook.
	 */
	function ObtenerLogoutFb()
	{
		$fb = new Facebook(FacebookConfig::ObtenerConfiguracionAplicacion());
		return $fb->getLogoutUrl();
	}

	/**
	 *
	 * Obtiene los grupos en donde el usuario tiene alguna relación de pertenencia o administración
	 */
	public function ObtenerGruposAsociados()
	{
		$grupos = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Grupo FROM UsuarioGrupo WHERE Usuario = %s",
		$conexionBd->Escapar($this->IdFacebook)
		);
		$idsGrupo = $conexionBd->EjecutarConsulta();
		if(is_array($idsGrupo) && count($idsGrupo) >= 1)
		{
			$grupos = array();
			foreach($idsGrupo as $idGrupo)
			$grupos[] = Grupo::Cargar($idGrupo['Grupo']);
		}
		return $grupos;
	}

	/**
	 *
	 * Elimina un usuario completamente del registro.
	 * @throws Se arroja cuando el usuario se relaciona con datos indispensables para el club.
	 */
	public function EliminarRegistro()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('No eres súper usuario, así que no puedes eliminar a otros usuarios');
		if($usuario->IdFacebook == $this->IdFacebook)
		throw new Exception('No puedes auto-eliminarte');
		/*
		 * NOTA: Las claves foráneas que no se eliminan son obligatorias
		 * para los datos del club. Si alguna clave restringe la eliminación del usuario,
		 * entonces será imposible realizar dicha acción.
		 * Para esto puede usar la suspensión dle usuario.
		 */
		$conexionBd = new ConectorBaseDatos();
		$idFacebook = $conexionBd->Escapar($this->IdFacebook);
		$conexionBd->IniciarTransaccion();
		// Eliminar ítems vendidos
		$conexionBd->EjecutarComando(sprintf("DELETE FROM Venta WHERE IdUsuario = %s", $idFacebook));
		// Eliminar álbumes
		$conexionBd->EjecutarComando(sprintf("DELETE FROM LaminaAlbum WHERE Album IN (SELECT Id FROM Album WHERE IdUsuario = %s)", $idFacebook));
		$conexionBd->EjecutarComando(sprintf("DELETE FROM Album WHERE IdUsuario = %s", $idFacebook));
		// Eliminar historial de puntos
		$conexionBd->EjecutarComando(sprintf("DELETE FROM Puntos WHERE IdUsuario = %s", $idFacebook));
		// Eliminar membresía de grupos
		$conexionBd->EjecutarComando(sprintf("DELETE FROM UsuarioGrupo WHERE Usuario = %s", $idFacebook));
		// Eliminar cambio de nivel
		$conexionBd->EjecutarComando(sprintf("DELETE FROM CambioNivel WHERE IdUsuario = %s", $idFacebook));
		// Eliminar permisos concedidos
		$conexionBd->EjecutarComando(sprintf("DELETE FROM PermisoUsuario WHERE Usuario = %s", $idFacebook));
		// Eliminar asistencias
		$conexionBd->EjecutarComando(sprintf("DELETE FROM Asistencia WHERE IdUsuario = %s", $idFacebook));
		// Heredar los usuarios validados por el próximo a eliminarse
		$conexionBd->Sentencia = sprintf("UPDATE UsuarioFacebook SET ValidadoPor = %s WHERE ValidadoPor = %s",
		$conexionBd->Escapar($usuario->IdFacebook),
		$idFacebook
		);
		$conexionBd->EjecutarComando();
		// Eliminarlo.
		$conexionBd->EjecutarComando();
		$conexionBd->Sentencia = sprintf("DELETE FROM UsuarioFacebook WHERE IdFacebook = %s",
		$idFacebook
		);
		$conexionBd->EjecutarComando();
		$conexionBd->ConfirmarTransaccion();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Obtiene los álbumes registrados al usuario.
	 * @return Album
	 */
	public function ObtenerAlbumes()
	{
		return Album::ObtenerAlbumes($this);
	}

	/**
	 *
	 * Determina si el usuario tiene o no el permiso.
	 * @param string $Permiso
	 */
	public function TienePermiso($Permiso)
	{
		return Permiso::Otorgado($this, $Permiso);
	}
}
?>