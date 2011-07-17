<?php
/**
 * 
 * Contiene procedimientos predefinidos para la automatización de tareas.
 * @author Julián Mejio
 * @version 1.0
 *
 */
class Macro
{
	/**
	 * 
	 * Agrega una cantidad de cromos al primer álbum del usuario que esté activo.
	 * @param int $Cantidad
	 */
	static function AgregarCromosDisponibles($Cantidad = 1)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		$albumes = $usuario->ObtenerAlbumes();
		$albumes[0]->GestionarCromosDisponibles(intval($Cantidad));
	}
}
?>