<?php
/**
 * 
 * Representa un grupo del club.
 * @author Julián Mejio
 * @version 1.5
 *
 * @property int $Id
 * @property string $Nombre
 * @property string $Descripcion
 * @property boolean $EsSistema
 * @property boolean $EsParticipativo
 * @property boolean $PorDefecto
 * @property UsuarioFacebook $Usuarios
 * @property boolean $EstaRegistrado
 */
class Grupo
{
	private $id;
	private $nombre;
	private $descripcion;
	private $esSistema;
	private $esParticipativo;
	private $porDefecto;
	private $usuarios;

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
			case 'EsSistema':
				$valor = $this->esSistema;
				break;
			case 'EsParticipativo':
				$valor = $this->esParticipativo;
				break;
			case 'PorDefecto':
				$valor = $this->porDefecto;
				break;
			case 'EstaRegistrado':
				$valor = ($this->Id != null);
				break;
			case 'Usuarios':
				$valor = $this->usuarios;
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
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un grupo.
	 * @param string $Nombre
	 * @param string $Descripcion
	 */
	function __construct($Nombre, $Descripcion = null)
	{
		$this->Nombre = $Nombre;
		$this->Descripcion = $Descripcion;
		$this->esSistema = false;
		$this->esParticipativo = false;
		$this->usuarios = array();
	}

	/**
	 *
	 * Agrega un usuario al grupo.
	 * @param UsuarioFacebook $Usuario
	 */
	function AgregarUsuario(UsuarioFacebook $Usuario)
	{
		if($this->EstaRegistrado)
			throw new Exception('Use Grupo::RegistrarUsuario() en vez de Grupo::AgregarUsuario()');
		if(!$Usuario->EsMiembroClubOrion)
			throw new Exception('No se puede registar el usuario en el grupo porque no forma parte del club');
		$registrado = false;
		if(!is_array($this->Usuarios))
			$this->usuarios = array();
		else if(!$this->EsMiembro($Usuario))
			$this->usuarios[] = $Usuario;
	}

	function EsMiembro(UsuarioFacebook $Usuario)
	{
		$miembro = false;
		if(is_array($this->Usuarios))
		foreach($this->Usuarios as $usuario)
			if($usuario->IdFacebook == $Usuario->IdFacebook)
			{
				$miembro = true;
				break;
			}
		return $miembro;
	}

	/**
	 *
	 * Registra el usuario directamente en la base de datos.
	 * @param UsuarioFacebook $Usuario
	 */
	function RegistrarUsuario(UsuarioFacebook $Usuario)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$this->EsAdministrador($usuario) && !$usuario->TienePermiso('ModificarGrupos'))
			throw new Exception('No puede registrar usuarios en este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		if(!$Usuario->EsMiembroClubOrion)
			throw new Exception('No se puede registrar el usuario porque no forma parte del club');
		if($this->EsMiembro($Usuario))
			throw new Exception('El usuario ya está registrado');
		$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("INSERT UsuarioGrupo (Grupo, Usuario, FechaRegistro) VALUES (%s, %s, NOW())",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->usuarios[] = $Usuario;
	}

	/**
	 *
	 * Eliminar un usuario del grupo directamente de la base de datos.
	 * @param UsuarioFacebook $Usuario
	 */
	function EliminarUsuario(UsuarioFacebook $Usuario)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$this->EsAdministrador($usuario) && !$usuario->TienePermiso('ModificarGrupos'))
			throw new Exception('No puede eliminar usuarios en este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		if(!$this->EsMiembro($Usuario))
			throw new Exception('El usuario ya no es miembro del grupo');
		if($usuario->IdFacebook == $Usuario->IdFacebook && !$usuario->TienePermiso('AdministrarGrupos'))
			throw new Exception('No puede auto-eliminarse del grupo');
		$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("DELETE FROM UsuarioGrupo WHERE Grupo = %s AND Usuario = %s",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Concede derechos de administrador a un usuario.
	 * @param UsuarioFacebook $Usuario
	 */
	function HacerAdministrador(UsuarioFacebook $Usuario)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$this->EsAdministrador($usuario) && !$usuario->TienePermiso('AdministrarGrupos'))
			throw new Exception('No puede nombrar administradores en este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo aún no está registrado');
		if(!$this->EsMiembro($Usuario))
			$this->RegistrarUsuario($Usuario);
		else if($this->EsAdministrador($Usuario))
			throw new Exception('El usuario ya es administrador');
		$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("UPDATE UsuarioGrupo SET Administrador = 1 WHERE Grupo = %s AND Usuario = %s",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Quita los derechoas de administración a un usuario.
	 * @param UsuarioFacebook $Usuario
	 */
	function QuitarAdministracion(UsuarioFacebook $Usuario)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$this->EsAdministrador($usuario) && !$usuario->TienePermiso('AdministrarGrupos'))
			throw new Exception('No puede quitar administradores en este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está rgistrado');
		if(!$this->EsAdministrador($Usuario))
			throw new Exception('El usuario no es administrador');
		if($usuario->IdFacebook == $Usuario->IdFacebook && !$usuario->TienePermiso('AdministrarGrupos'))
			throw new Exception('No puede quitarse uste mismo los derechos de administración del grupo');
		$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("UPDATE UsuarioGrupo SET Administrador = 0 WHERE Grupo = %s AND Usuario = %s",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Determina si un usuario es administrador del grupo o no.
	 * @param UsuarioFacebook $Usuario
	 */
	function EsAdministrador(UsuarioFacebook $Usuario)
	{
		$administrador = false;
		if(!$this->EstaRegistrado)
		throw new Exception('El grupo no está registrado');
		if($this->EsMiembro($Usuario))
		{
			$conexionBd = new ConectorBaseDatos();
				$conexionBd->Sentencia = sprintf("SELECT COUNT(*) FROM UsuarioGrupo WHERE Grupo = %s AND Usuario = %s AND Administrador = 1",
				$conexionBd->Escapar($this->Id),
				$conexionBd->Escapar($Usuario->IdFacebook)
			);
			if($conexionBd->EjecutarEscalar() >= 1)
				$administrador = true;
			$conexionBd->Desconectar();
		}
		return $administrador;
	}

	/**
	 *
	 * Carga los usuarios del grupo.
	 */
	private function cargarUsuarios()
	{
		$this->usuarios = array();
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("SELECT Usuario FROM UsuarioGrupo WHERE Grupo = %s",
			$conexionBd->Escapar($this->Id)
		);
		$idUsuarios = $conexionBd->EjecutarConsulta();
		if($idUsuarios != null && count($idUsuarios) >= 1)
			foreach($idUsuarios as $idUsuario)
				$this->usuarios[] = new UsuarioFacebook($idUsuario['Usuario']);
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Registra el grupo en la base de datos.
	 */
	function Registrar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('CrearGrupos'))
			throw new Exception('No puede crear grupos');
		if($this->EstaRegistrado)
			throw new Exception('El grupo ya está registrado');
		$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("INSERT Grupo (Nombre, Descripcion) VALUES (%s, %s)",
			$conexionBd->Escapar($this->Nombre),
			$conexionBd->Escapar($this->Descripcion)
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		if(is_array($this->Usuarios) && count($this->Usuarios) >= 1)
			foreach($this->Usuarios as $usuario)
				$conexionBd->EjecutarComando(sprintf(
					"INSERT UsuarioGrupo (Grupo, Usuario, Fecha) VALUES (%s, %s, NOW())",
					$conexionBd->Escapar($this->Id),
					$conexionBd->Escapar($usuario->IdFacebook)
				));
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Guarda los cambios realizados del grupo en la base de datos.
	 */
	function GuardarCambios()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$this->EsAdministrador($usuario) && !$usuario->TienePermiso('ModificarGrupos'))
			throw new Exception('No puede modificar la información de este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("UPDATE Grupo SET Nombre = %s, Descripcion = %s WHERE Id = %s",
			$conexionBd->Escapar($this->Nombre),
			$conexionBd->Escapar($this->Descripcion),
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Sentencia = sprintf("SELECT Usuario FROM UsuarioGrupo WHERE Grupo = %s",
			$conexionBd->Escapar($this->Id)
		);
		$idsUsuario = $conexionBd->EjecutarConsulta();
		$conexionBd->Desconectar();
		$usuariosProcesados = array();
		foreach($this->Usuarios as $usuario)
		{
			$accion = 'Registrar';
			foreach($idsUsuario as $idUsuario)
				if($usuario->IdFacebook == $idUsuario['Usuario'])
				{
					$accion = 'Omitir';
					break;
				}
			if($accion == 'Registrar')
				$this->RegistrarUsuario($usuario);
			$usuariosProcesados[] = $usuario;
		}
		foreach($idsUsuario as $idUsuario)
		{
			$accion = 'Eliminar';
			foreach($usuariosProcesados as $usuario)
				if($idUsuario['Usuario'] == $usuario->IdFacebook)
				{
					$accion = 'Omitir';
					break;
				}
			if($accion == 'Eliminar')
				$this->EliminarUsuario(new UsuarioFacebook($idUsuario['Usuario']));
		}
	}

	/**
	 *
	 * Elimina el grupo de la base de datos. Esta función también elimina cualquier asociación
	 * de usuarios al grupo.
	 */
	function Eliminar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('EliminarGrupos'))
			throw new Exception('No puede eliminar este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		if($this->esSistema)
			throw new Exception('No se puede eliminar este grupo porque fue marcado como parte esencial del sistema');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->IniciarTransaccion();
		$conexionBd->Sentencia = sprintf("DELETE FROM UsuarioGrupo WHERE Grupo = %s",
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
			$conexionBd->Sentencia = sprintf("DELETE FROM Grupo WHERE Id = %s",
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->ConfirmarTransaccion();
		$conexionBd->Desconectar();
		$this->id = null;
		$this->usuarios = array();
	}

	/**
	 *
	 * Marca un grupo como de sistema.
	 */
	function MarcarComoSistema()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('AdministrarSistema'))
			throw new Exception('No puede cambiar el comportamiento de este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no se encuentra registrado');
		if($this->EsSistema)
			throw new Exception('El grupo ya es de sistema');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Grupo SET Sistema = 1 WHERE Id = %s",
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->esSistema = true;
	}

	/**
	 *
	 * Marca el grupo como predeterminado.
	 */
	function HacerPredeterminado()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('AdministrarSistema'))
			throw new Exception('No puede cambiar el comportamiento de este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		if($this->PorDefecto)
			throw new Exception('El grupo ya es predeterminado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Grupo SET Defecto = 1 WHERE Id = %s",
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->porDefecto = 1;
	}
	
	/**
	 * 
	 * Marca el grupo como participativo para que se cuenten puntos por actividad y experiencia.
	 */
	function HacerPaticipativo()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('AdministrarSistema'))
			throw new Exception('No puede cambiar el comportamiento de este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		if($this->EsParticipativo)
			throw new Exception('El grupo ya es participativo');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Grupo SET Participativo = 1 WHERE Id = %s",
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->esParticipativo = 1;
	}

	/**
	 *
	 * Quita el grupo como predeterminado.
	 */
	function QuitarPredeterminado()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('AdministrarSistema'))
			throw new Exception('No puede cambiar el comportamiento de este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		if(!$this->PorDefecto)
			throw new Exception('El grupo ya no está predeterminado');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Grupo SET Defecto = 0 WHERE Id = %s",
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->porDefecto = 0;
	}

	/**
	 *
	 * Quita el grupo como participativo.
	 */
	function QuitarParticipativo()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('AdministrarSistema'))
			throw new Exception('No puede cambiar el comportamiento de este grupo');
		if(!$this->EstaRegistrado)
			throw new Exception('El grupo no está registrado');
		if(!$this->EsParticipativo)
			throw new Exception('El grupo ya no está como participativo');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE Grupo SET Participativo = 0 WHERE Id = %s",
			$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->esParticipativo = 0;
	}
	
	/**
	 *
	 * Carga un grupo.
	 * @param int $Id
	 * @return Grupo
	 */
	static function Cargar($Id)
	{
		$grupo = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Nombre, Descripcion, Sistema, Defecto, Participativo FROM Grupo WHERE Id = %s",
		$conexionBd->Escapar($Id)
		);
		$datos = $conexionBd->EjecutarConsulta();
		if(is_array($datos) && count($datos) >= 1)
		{
			$datos = $datos[0];
			$grupo = new Grupo($datos['Nombre'], $datos['Descripcion']);
			$grupo->id = $Id;
			$grupo->esSistema = $datos['Sistema'];
			$grupo->porDefecto = $datos['Defecto'];
			$grupo->esParticipativo = $datos['Participativo'];
			$grupo->cargarUsuarios();
			$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
			if(!$grupo->EsMiembro($usuario) && !$usuario->TienePermiso('VerGrupos'))
			throw new Exception('No puede ver la información de este grupo');
		}
		return $grupo;
	}

	/**
	 *
	 * Obtiene todos los grupos registrados en la base de datos.
	 */
	static function ObtenerTodos()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->TienePermiso('VerGrupos'))
		throw new Exception('No puede usar esta función [Grupo::ObtenerTodos()]');
		$grupos = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT Id, Nombre, Descripcion, Sistema, Defecto, Participativo FROM Grupo';
		$datosGrupos = $conexionBd->EjecutarConsulta();
		if(is_array($datosGrupos) && count($datosGrupos) >= 1)
		{
			$grupos = array();
			foreach($datosGrupos as $datosGrupo)
			{
				$grupo = new Grupo($datosGrupo['Nombre']);
				$grupo->id = $datosGrupo['Id'];
				$grupo->Descripcion = $datosGrupo['Descripcion'];
				$grupo->esSistema = $datosGrupo['Sistema'];
				$grupo->porDefecto = $datosGrupo['Defecto'];
				$grupo->esParticipativo =  $datosGrupo['Participativo'];
				$grupo->cargarUsuarios();
				$grupos[] = $grupo;
			}
		}
		return $grupos;
	}

	/**
	 *
	 * Registra un usuario en los grupos predeterminados.
	 * @param UsuarioFacebook $Usuario
	 */
	static function RegistrarEnPredeterminados(UsuarioFacebook $Usuario)
	{
		if(!$Usuario->EsMiembroClubOrion)
		throw new Exception('No se puede registrar un usuario en un grupo');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT Id FROM Grupo WHERE Defecto = 1';
		$idsGrupos = $conexionBd->EjecutarConsulta();
		if(is_array($idsGrupos) && count($idsGrupos) >= 1)
		{
			foreach($idsGrupos as $idGrupo)
			try
			{
				$conexionBd->EjecutarComando(sprintf("INSERT INTO UsuarioGrupo (Usuario, Grupo, FechaRegistro) VALUES (%s, %s, NOW())",
				$conexionBd->Escapar($Usuario->IdFacebook),
				$conexionBd->Escapar($idGrupo['Id'])
				));
			}
			catch(Excpetion $e) {}
		}
	}
}
?>