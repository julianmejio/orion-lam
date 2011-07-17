<?php
/**
 *
 * Contiene una interfaz de gestión de permisos.
 * @author Julián Mejio
 * @version 1.0
 */
class Permiso
{
	/**
	 *
	 * Obtiene una lista de permisos registrados (0: permiso, 1: descripcion).
	 * @return Array
	 */
	static function ObtenerLista()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('Para ver la lista de permisos debe ser un súper usuario');
		$permisos = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = 'SELECT Permiso, Descripcion FROM Permiso';
		$datosPermisos = $conexionBd->EjecutarConsulta();
		if(is_array($datosPermisos) && count($datosPermisos) >= 1)
		{
			$permisos = array();
			foreach($datosPermisos as $datosPermiso)
			$permisos[] = array($datosPermiso['Permiso'], $datosPermiso['Descripcion']);
		}
		return $permisos;
	}

	/**
	 *
	 * Determina si un usuario tiene un permiso específico, ya sea por permiso individual o por grupal
	 * @param UsuarioFacebook $Usuario
	 * @param string $Permiso
	 * @return boolean
	 */
	static function Otorgado(UsuarioFacebook $Usuario, $Permiso)
	{
		$permiso = ($Usuario->EsSuperUsuario);
		if(!$permiso)
		{
			$permiso = Permiso::OtorgadoSoloUsuario($Usuario, $Permiso);
			if(!$permiso)
			{
				$conexionBd = new ConectorBaseDatos();
				$conexionBd->Sentencia = sprintf("SELECT COUNT(*) FROM PermisoGrupo WHERE Permiso = %s AND Grupo IN(SELECT Grupo FROM UsuarioGrupo WHERE Usuario = %s)",
				$conexionBd->Escapar($Permiso),
				$conexionBd->Escapar($Usuario->IdFacebook)
				);
				$permiso = ($conexionBd->EjecutarEscalar()) ? true : false;
				$conexionBd->Desconectar();
			}
		}
		return $permiso;
	}

	/**
	 *
	 * Determina si un permiso fue otorgado individualmente a un usuario.
	 * @param UsuarioFacebook $Usuario
	 * @param string $Permiso
	 */
	static function OtorgadoSoloUsuario(UsuarioFacebook $Usuario, $Permiso)
	{
		$permiso = ($Usuario->EsSuperUsuario);
		if(!$permiso)
		{
			$conexionBd = new ConectorBaseDatos();
			$conexionBd->Sentencia = sprintf("SELECT COUNT(*) FROM PermisoUsuario WHERE Usuario = %s AND Permiso = %s",
			$conexionBd->Escapar($Usuario->IdFacebook),
			$conexionBd->Escapar($Permiso)
			);
			$permiso = ($conexionBd->EjecutarEscalar()) ? true : false;
			$conexionBd->Desconectar();
		}
		return $permiso;
	}

	/**
	 *
	 * Determina si un grupo tiene un permiso específico.
	 * @param Grupo $Grupo
	 * @param string $Permiso
	 */
	static function OtorgadoSoloGrupo(Grupo $Grupo, $Permiso)
	{
		$permiso = false;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT COUNT(*) FROM PermisoGrupo WHERE Grupo = %s AND Permiso = %s",
		$conexionBd->Escapar($Grupo->Id),
		$conexionBd->Escapar($Permiso)
		);
		$permiso = ($conexionBd->EjecutarEscalar()) ? true : false;
		$conexionBd->Desconectar();
		return $permiso;
	}

	/**
	 *
	 * Otorga un permiso a un objeto.
	 * @param $Objeto
	 * @param string $Permiso
	 */
	static function Otorgar($Objeto, $Permiso)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('No puede otorgar permisos porque no es un súper usuario');
		$conexionBd = new ConectorBaseDatos();
		if(is_a($Objeto, 'UsuarioFacebook'))
		{
			if(!$Objeto->EsMiembroClubOrion)
			throw new Exception('No puede otorgar permisos a usuarios que no sean miembros del club');
			$conexionBd->Sentencia = sprintf("INSERT PermisoUsuario VALUES (%s, %s)",
			$conexionBd->Escapar($Objeto->IdFacebook),
			$conexionBd->Escapar($Permiso)
			);
		}
		else if(is_a($Objeto, 'Grupo'))
		$conexionBd->Sentencia = sprintf("INSERT PermisoGrupo VALUES (%s, %s)",
		$conexionBd->Escapar($Objeto->Id),
		$conexionBd->Escapar($Permiso)
		);
		else
		throw new Exception('No se pueden otrogar permisos a este tipo de objeto');
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Otroga el permiso de súper usuario.
	 * @param UsuarioFacebook $Usuario
	 * @throws Exception se arroja cuando el usuario otorgante no es súper usuario o cuando el usuario a otrogar ya es súper usuario.
	 */
	static function OtorgarSuperUsuario(UsuarioFacebook $Usuario)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('No puede otorgar el permiso de súper usuario porque no es un súper usuario');
		if($Usuario->EsSuperUsuario)
		throw new Exception('El usuario ya es súper usuario');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE UsuarioFacebook SET SuperUsuario = 1 WHERE IdFacebook = %s",
		$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Quitar el permiso de súper usuario.
	 * @param UsuarioFacebook $Usuario
	 */
	static function QuitarSuperUsuario(UsuarioFacebook $Usuario)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('No puede quitar el permiso de súper usuario porque no es un súper usuario');
		if(!$Usuario->EsSuperUsuario)
		throw new Exception('El usuario ya no es un súper usuario');
		if($Usuario->IdFacebook == $usuario->IdFacebook)
		throw new Exception('No puedes quitarte tú mismo el permiso de súper usuario');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE UsuarioFacebook SET SuperUsuario = 0 WHERE IdFacebook = %s",
		$conexionBd->Escapar($Usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Quia un permiso otrogado a un objeto
	 * @param unknown_type $Objeto
	 * @param unknown_type $Permiso
	 */
	static function Quitar($Objeto, $Permiso)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('No puede quitar permisos porque no es un súper usuario');
		$conexionBd = new ConectorBaseDatos();
		if(is_a($Objeto, 'UsuarioFacebook'))
		$conexionBd->Sentencia = sprintf("DELETE FROM PermisoUsuario WHERE Usuario = %s AND Permiso = %s",
		$conexionBd->Escapar($Objeto->IdFacebook),
		$conexionBd->Escapar($Permiso)
		);
		else if(is_a($Objeto, 'Grupo'))
		$conexionBd->Sentencia = sprintf("DELETE FROM PermisoGrupo WHERE Grupo = %s AND Permiso = %s",
		$conexionBd->Escapar($Objeto->Id),
		$conexionBd->Escapar($Permiso)
		);
		else
		throw new Exception('No se pueden quitar permisos a este tipo de objeto');
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Crea un permiso nuevo.
	 * @param string $Nombre
	 * @param string $Descripcion
	 */
	static function Crear($Nombre, $Descripcion = null)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('No puede crear permisos porque no es un súper usuario');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("INSERT Permiso VALUES (%s, %s)",
		$conexionBd->Escapar($Nombre),
		$conexionBd->Escapar($Descripcion)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Elimina un permiso de la base de datos. Este no debe tener referencia alguna.
	 * @param string $Permiso
	 */
	static function Eliminar($Permiso)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!$usuario->EsSuperUsuario)
		throw new Exception('No puede eliminar pemrisos porque no es un súper usuario');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("DELETE FROM Permiso WHERE Permiso = %s",
		$conexionBd->Escapar($Permiso)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}
}
?>