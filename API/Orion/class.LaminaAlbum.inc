<?php
/**
 *
 * Representa una entrada de lámina en un álbum.
 * @author Julián Mejio
 *
 * @property Album $Album
 * @property Lamina $Lamina
 * @property date $FechaAdquisicion
 * @property int $Cantidad
 */
class LaminaAlbum
{
	private $album;
	private $lamina;
	private $fechaAdquisicion;
	private $cantidad;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Album':
				$valor = $this->album;
				break;
			case 'Lamina':
				$valor = $this->lamina;
				break;
			case 'FechaAdquisicion':
				$valor = $this->fechaAdquisicion;
				break;
			case 'Cantidad':
				$valor = $this->cantidad;
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
	 * Obtiene una lámina de un álbum
	 * @param Album $Album
	 * @param Lamina $Lamina
	 * @throws Exception
	 * @return LaminaAlbum
	 */
	public static function Obtener(Album $Album, Lamina $Lamina)
	{
		if(!$Album->EstaRegistrado)
		throw new Exception('El álbum no está registrado');
		if(!$Lamina->EstaRegistrada)
		throw new Exception('La lámina no está registrada');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT FechaAdquisicion, Cantidad FROM LaminaAlbum WHERE IdAlbum = %s AND IdLamina = %s LIMIT 1",
		$conexionBd->Escapar($Album->Id),
		$conexionBd->Escapar($Lamina->Id)
		);
		$datos = $conexionBd->EjecutarConsulta();
		if($datos == null)
		throw new Exception('El álbum no tiene dicha lámina');
		$laminaAlbum = new LaminaAlbum();
		$laminaAlbum->album = $Album;
		$laminaAlbum->lamina = $Lamina;
		$laminaAlbum->fechaAdquisicion = $datos[0]['FechaAdquisicion'];
		$laminaAlbum->cantidad = $datos[0]['Cantidad'];
		return $laminaAlbum;
	}
}
?>