<?php
/**
 *
 * Representa la experiencia por medio de puntos.
 * @author Julián Mejio
 * @version 1.1-rv2
 *
 * @property int $Puntos
 * @property UsuarioFacebook $Usuario
 * @property int $Nivel
 * @property int $PuntosProximoNivel
 * @property int $DiferenciaPuntosNivel
 * @property int $PuntosRelativosNivel
 * @property double $ProgresoNivel
 */
class Experiencia
{
	private $puntos;
	private $usuario;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Puntos':
				$valor = $this->puntos;
				break;
			case 'Usuario':
				$valor = $this->usuario;
				break;
			case 'Nivel':
				$valor = Experiencia::obtenerNivel($this->Puntos);
				break;
			case 'PuntosProximoNivel':
				$valor = Experiencia::obtenerPuntosNivel($this->Nivel + 1);
				break;
			case 'DiferenciaPuntosNivel':
				$valor = $this->PuntosProximoNivel - $this->Puntos;
				break;
			case 'PuntosRelativosNivel':
				$valor = $this->Puntos - Experiencia::obtenerPuntosNivel($this->Nivel);
				break;
			case 'ProgresoNivel':
				$valor = $this->PuntosRelativosNivel / ($this->PuntosProximoNivel - Experiencia::obtenerPuntosNivel($this->Nivel));
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
			case 'Usuario':
				if(!is_a($Valor, 'UsuarioFacebook'))
				throw new Exception('Usuario no válido');
				$this->usuario = $Valor;
				$this->actualizarExperiencia();
				break;
			default:
				throw new Exception("La propiedad \"$Variable\" no existe en el contexto actual o es de sólo lectura");
		}
	}

	/**
	 *
	 * Crea un contenedor de experiencia.
	 * @param UsuarioFacebook $Usuario
	 */
	function __construct(UsuarioFacebook $Usuario)
	{
		$this->Usuario = $Usuario;
	}

	/**
	 *
	 * Otorga al usuario puntos de experiencia.
	 * @param int $Puntos
	 * @throws Exception Se arroja cuando el usuario no es miembro del club Orión.
	 */
	public function OtorgarExperiencia($Puntos)
	{
		$Puntos = intval($Puntos);
		if($this->Usuario->EsMiembroClubOrion && $Puntos >= 1)
		{
			$conexionBd = new ConectorBaseDatos();
			$this->puntos = $conexionBd->EjecutarEscalar(sprintf(
				"SELECT Experiencia FROM UsuarioFacebook WHERE IdFacebook = %s",
			$conexionBd->Escapar($this->Usuario->IdFacebook)
			));
			$nivelAnterior = $this->Nivel;
			$conexionBd->Sentencia = sprintf("UPDATE UsuarioFacebook SET UltimaActualizacion = UltimaActualizacion, Experiencia = Experiencia + %s WHERE IdFacebook = %s",
			$conexionBd->Escapar($Puntos),
			$conexionBd->Escapar($this->Usuario->IdFacebook)
			);
			$conexionBd->EjecutarComando();
			$this->puntos += $Puntos;
			if($this->Nivel > $nivelAnterior)
			{
				$conexionBd->Sentencia = sprintf("INSERT CambioNivel (IdUsuario, NivelAnterior, NivelActual, Fecha) VALUES (%s, %s, %s, NOW())",
				$conexionBd->Escapar($this->Usuario->IdFacebook),
				$conexionBd->Escapar("$nivelAnterior"),
				$conexionBd->Escapar($this->Nivel)
				);
				$conexionBd->EjecutarComando();
				try
				{
					$postFb = new FacebookPost($this->Usuario, '¡Subí de nivel!', 'http://mehiocore.com/orionapp');
					$postFb->NombreVinculo = 'Club Orión de Astronomía';
					$postFb->Titulo = 'El Ñoñómetro';
					$postFb->Descripcion = "{$this->Usuario->DatosFacebook['name']} ha subido al nivel $this->Nivel.";
					$postFb->UrlImagen = 'http://mehiocore.com/orionapp/Imagenes/FBImg/NivelUp.png';
					$postFb->Publicar();
				}
				catch(Exception $e) {}
			}
			$conexionBd->Desconectar();
		}
	}

	/**
	 *
	 * Procesa las bonificaciones pendientes
	 */
	public function ProcesarBonificacion()
	{
		$bonificado = false;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Niveles FROM BonificacionNivel WHERE IdUsuario = %s",
		$conexionBd->Escapar($this->Usuario->IdFacebook)
		);
		$bonificacion = $conexionBd->EjecutarEscalar();
		$conexionBd->Sentencia = sprintf("UPDATE CambioNivel SET BonificacionProcesada = TRUE WHERE IdUsuario = %s",
		$conexionBd->Escapar($this->Usuario->IdFacebook)
		);
		$conexionBd->EjecutarComando();
		if($bonificacion >= 1)
		{
			$bonificado = true;
			// BONIFICACIÓN DE LÁMINAS
			// Agregar láminas disponibles para reclamar en razón al número de niveles de
			// bonificación que tenga.
			$album = $this->Usuario->ObtenerAlbumes();
			$album = $album[0];
			$album->GestionarCromosDisponibles($bonificacion);
		}
		$conexionBd->Desconectar();
		return $bonificado;
	}

	/**
	 *
	 * Actualiza los datos de experiencia del usuario.
	 */
	private function actualizarExperiencia()
	{
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Experiencia FROM UsuarioFacebook WHERE IdFacebook = %s",
		$conexionBd->Escapar($this->Usuario->IdFacebook)
		);
		$this->puntos = intval($conexionBd->EjecutarEscalar());
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Obtiene los puntos necesarios por nivel.
	 * @param int $Nivel
	 */
	static private function obtenerPuntosNivel($Nivel)
	{
		$Nivel = intval($Nivel);
		// Crecimiento real
		return intval(ceil(9 * pow($Nivel, 2) / 5));
	}

	/**
	 *
	 * Obtiene el nivel a razón de los puntos.
	 * @param int $Puntos
	 */
	static private function obtenerNivel($Puntos)
	{
		$Puntos = intval($Puntos);
		// Crecimiento real
		return intval(preg_replace('/\..*/', '', sqrt(5 * $Puntos / 9)));
	}

	/**
	 *
	 * Genera un ítem tipo {@see EtiquetaXml} que contiene la información en HTML del progreso del usuario.
	 * @return EtiquetaXml
	 */
	function GenerarBarraProgresoX()
	{
		$contenedor = new EtiquetaXml('div');
		if($this->Usuario->EsMiembroClubOrion)
		{
			$contenedor->AgregarAtributo(new AtributoXml('class', 'Contenedor-nonometro ui-corner-all'));
			$titulo = new EtiquetaXml('p');
			$titulo->AgregarContenido('Ñoñómetro');
			$titulo->AgregarAtributo(new AtributoXml('class', 'Titulo'));
			$contenedor->AgregarContenido($titulo);
			$contenedorBarra = new EtiquetaXml('div');
			$contenedorBarra->AgregarAtributo(new AtributoXml('class', 'Contenedor-barra ui-corner-all'));
			$contenedor->AgregarAtributo(new AtributoXml('title', "$this->Puntos puntos de experiencia. $this->DiferenciaPuntosNivel puntos para el próximo nivel"));
			$barraProgreso = new EtiquetaXml('div');
			$barraProgreso->AgregarAtributo(new AtributoXml('style', 'width: ' . round($this->ProgresoNivel * 100) . '%'));
			$barraProgreso->AgregarAtributo(new AtributoXml('class', 'Barra-progreso ui-corner-left'));
			$barraProgreso->AgregarContenido('&nbsp;');
			$contenedorBarra->AgregarContenido($barraProgreso);
			$contenedor->AgregarContenido($contenedorBarra);
			$nivelTxt = new EtiquetaXml('p');
			$nivelTxt->AgregarContenido($this->Nivel);
			$nivelTxt->AgregarAtributo(new AtributoXml('class', 'Nivel ui-corner-right'));
			$contenedor->AgregarContenido($nivelTxt);
			$clear = new EtiquetaXml('div');
			$clear->AgregarAtributo(new AtributoXml('class', 'ClearHelper'));
			$contenedor->AgregarContenido($clear);
		}
		return $contenedor;
	}


	function GenerarNivelXInLine()
	{
		$nivel = new EtiquetaXml('span');
		if($this->Usuario->EsMiembroClubOrion)
		{
			$nivel->AgregarAtributo(new AtributoXml('class', 'Nivel ui-corner-all'));
			$nivel->AgregarAtributo(new AtributoXml('title', "$this->Puntos puntos de experiencia"));
			$nivel->AgregarAtributo(new AtributoXml('rel', 'nonometro'));
			$nivel->AgregarContenido($this->Nivel);
		}
		return $nivel;
	}
}
?>