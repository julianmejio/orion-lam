<?php
/**
 * 
 * Representa la tienda de objetos del club.
 * @author Julián Mejio
 * @version 2.2
 *
 */
class Tienda
{
	/**
	 * 
	 * Vende un objeto {@see $Macro} al usuario que esté activo.
	 * @param string $Macro
	 * @param int $ValorUnitario
	 * @param int $Cantidad
	 * @param int $Parametros
	 */
	static function Vender($Macro, $ValorUnitario, $Cantidad, $Parametros = null)
	{
		if($Cantidad >= 1)
		{
			$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
			$valorTotal = $ValorUnitario * $Cantidad;
			if($usuario->ObtenerPuntosDisponibles() < $valorTotal)
				throw new Exception('No tienes tantos puntos disponibles para realizar esta compra');
			switch($Macro)
			{
				case 'Laminas1':
					Macro::AgregarCromosDisponibles($Cantidad);
					break;
				case 'Laminas10':
					Macro::AgregarCromosDisponibles(10 * $Cantidad);
					break;
				default:
					throw new Exception("El ítem seleccionado no se considera un ítem de venta: \"$Macro\"");
			}
			$usuario->GestionarPuntos(new ItemPuntaje(-1 * $valorTotal, "Compra en la tienda: $Macro"));
		}
	}

	/**
	 * 
	 * Obtiene una lista de ítems que están disponibles en la tienda.
	 */
	static function ObtenerItems()
	{
		$items = array();
		// ComprarCromos
		$items[] = array(
			'Nombre' => '1 lámina para reclamar',
			'Descripcion' => 'Una lámina para reclamar en tu álbum',
			'UrlImagen' => Config::RutaBase . '/Imagenes/TiendaImg/Cromos.png',
			'Macro' => 'Laminas1',
			'ValorUnitario' => 5,
		);
		$items[] = array(
			'Nombre' => 'Paquete de 10 láminas para reclamar',
			'Descripcion' => 'Ahorra cinco puntos comprando este paquete de diez láminas para reclamar',
			'UrlImagen' => Config::RutaBase . '/Imagenes/TiendaImg/Cromos.png',
			'Macro' => 'Laminas10',
			'ValorUnitario' => 45,
		);
		return $items;
	}
	
	/**
	 * 
	 * Generar los ítems HTML listos para su renderización.
	 * @return EtiquetaXml
	 */
	static function GenerarItemsX()
	{
		$itemsX = null;
		$items = Tienda::ObtenerItems();
		if(count($items) >= 1)
		{
			$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
			$puntos = $usuario->ObtenerPuntosDisponibles();
			$clear = new EtiquetaXml('div');
			$clear->AgregarAtributo(new AtributoXml('class', 'ClearHelper'));
			foreach($items as $item)
			{
				if($item['ValorUnitario'] > 0)
				{
					$itemX = new EtiquetaXml('div');
					$itemX->AgregarAtributo(new AtributoXml('class', 'Item'));
					$icono = new EtiquetaXml('div');
					$icono->AgregarAtributo(new AtributoXml('class', 'ControlItem Izquierdo Icono'));
					$icono->AgregarAtributo(new AtributoXml('style', "background-image: url({$item['UrlImagen']})"));
					$itemX->AgregarContenido($icono);
					$descripcion = new EtiquetaXml('div');
					$descripcion->AgregarAtributo(new AtributoXml('class', 'ControlItem Izquierdo Informacion'));
					$descripcion->AgregarAtributo(new AtributoXml('style', 'width: 449px'));
					$titulo = new EtiquetaXml('p', $item['Nombre']);
					$titulo->AgregarAtributo(new AtributoXml('class', 'Titulo'));
					$descripcion->AgregarContenido($titulo);
					$resumen = new EtiquetaXml('span', $item['Descripcion']);
					$resumen->AgregarAtributo(new AtributoXml('class', 'Descripcion'));
					$descripcion->AgregarContenido(new EtiquetaXml('p', $resumen));
					$itemX->AgregarContenido($descripcion);
					$controles = new EtiquetaXml('div');
					$controles->AgregarAtributo(new AtributoXml('class', 'ControlItem Derecho'));
					$controles->AgregarAtributo(new AtributoXml('style', 'text-align: right'));
					$controles->AgregarContenido(new EtiquetaXml('p', "{$item['ValorUnitario']} puntos"));
					$selector = new EtiquetaXml('select');
					$selector->AgregarAtributo(new AtributoXml('name', "cantidad_{$item['Macro']}"));
					$selector->AgregarAtributo(new AtributoXml('id', "cantidad_{$item['Macro']}"));
					$selector->AgregarAtributo(new AtributoXml('class', "selectorCantidad"));
					$selector->AgregarAtributo(new AtributoXml('rel', $item['ValorUnitario']));
					for($i = 0; $i <= floor($puntos / $item['ValorUnitario']); $i++)
						$selector->AgregarContenido(new EtiquetaXml('option', strval($i)));
					$controles->AgregarContenido($selector);
					$itemX->AgregarContenido($controles);
					$itemX->AgregarContenido($clear);
					$itemsX[] = $itemX;
				}
			}
		}
		return $itemsX;
	}
}
?>