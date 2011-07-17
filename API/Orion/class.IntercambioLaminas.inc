<?php
/**
 *
 * Representa y contiene la información de un intercambio de láminas.
 * @author Julián Mejio
 * @version 1.0
 *
 * @property int $Id
 * @property Album $AlbumInteresado
 * @property Album $AlbumInteresante
 * @property Lamina $LaminasInteresado
 * @property Lamina $LaminasInteresante
 * @property string $Estado
 * @property bool $EstaRegistrado
 */
class IntercambioLaminas
{
	private $id;
	private $albumInteresado;
	private $albumInteresante;
	private $laminasInteresado;
	private $laminasInteresante;
	private $estado;

	function __get($Variable)
	{
		$valor = null;
		switch($Variable)
		{
			case 'Id':
				$valor = $this->id;
				break;
			case 'AlbumInteresado':
				$valor = $this->albumInteresado;
				break;
			case 'AlbumInteresante':
				$valor = $this->albumInteresante;
				break;
			case 'LaminasInteresado':
				$valor = $this->laminasInteresado;
				break;
			case 'LaminasInteresante':
				$valor = $this->laminasInteresante;
				break;
			case 'Estado':
				$valor = $this->estado;
				break;
			case 'EstaRegistrado':
				$valor = ($this->Id != null) ? true : false;
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
	 * Crea un intercambio de láminas-
	 * @param Album $AlbumInteresante
	 */
	function __construct(Album $AlbumInteresante, $carga = false)
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if(!is_a($usuario, 'UsuarioFacebook'))
		throw new Exception('Debes estar autenticado para poder realizar intercambios');
		$album = $usuario->ObtenerAlbumes();
		$this->albumInteresado = $album[0];
		if(!$carga)
		if($this->AlbumInteresado->Id = $AlbumInteresante->Id)
		throw new Exception('No puedes autointercambiar láminas');
		$this->albumInteresante = $AlbumInteresante;
		$this->laminasInteresado = array();
		$this->laminasInteresante = array();
	}

	/**
	 *
	 * Agrega una lámina para el intercambio, de la parte interesada.
	 * @param LaminaAlbum $Lamina
	 */
	function AgregarLaminaInteresado(LaminaAlbum $Lamina)
	{
		if($this->EstaRegistrado)
		throw new Exception('No se puede modificar la lista de intercambio en este momento');
		if($Lamina->Album->Id != $this->AlbumInteresado->Id)
		throw new Exception('No puedes agregar láminas que no sean de tu álbum');
		if($Lamina->Cantidad <= 1)
		throw new Exception('No puedes intercambiar láminas que no tengas repetidas');
		foreach($this->laminasInteresado as $lamina)
		if($lamina->Lamina->Id == $Lamina->Lamina->Id)
		throw new Exception('Esta lámina ya está agregada en la lista de intercambio');
		$this->laminasInteresado[] = $Lamina;
	}

	/**
	 *
	 * Agrega una lámina para el intercambio, de la parte interesante.
	 * @param LaminaAlbum $Lamina
	 */
	function AgregarLaminaInteresante(LaminaAlbum $Lamina)
	{
		if($this->EstaRegistrado)
		throw new Exception('No se puede modificar la lista de intercambio en este momento');
		if($Lamina->Album->Id != $this->AlbumInteresante->Id)
		throw new Exception('Esta lámina no pertenece al álbum interesante');
		if($Lamina->Cantidad <= 1)
		throw new Exception('Esta lámina no se puede agregar porque no está repetida');
		foreach($this->laminasInteresante as $lamina)
		if($lamina->Lamina->Id == $Lamina->Lamina->Id)
		throw new Exception('Esta lámina ya está agregada en la lista de intercambio');
		$this->laminasInteresante[] = $Lamina;
	}

	/**
	 *
	 * Guarda el intercambio para una futura aprobación, rechazo o invalidez, dependiendo el caso.
	 */
	function Guardar()
	{
		if($this->EstaRegistrado)
		throw new Exception('Este intercambio ya está registrado');
		if(count($this->laminasInteresado) == 0 || count($this->LaminasInteresante) == 0)
		throw new Exception('Ambas partes deben intercambiar al guna lámina para poder registrar el intercambio');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->IniciarTransaccion();
		$conexionBd->Sentencia = sprintf("INSERT IntercambioLaminas (AlbumInteresado, AlbumInteresante) VALUES (%s, %s)",
		$conexionBd->Escapar($this->AlbumInteresado->Id),
		$conexionBd->Escapar($this->AlbumInteresante->Id)
		);
		$conexionBd->EjecutarComando();
		$this->id = $conexionBd->EjecutarEscalar('SELECT LAST_INSERT_ID()');
		foreach($this->laminasInteresado as $lamina)
		{
			$conexionBd->Sentencia = sprintf("INSERT LaminasIntercambio VALUES (%s, %s, 'AInteresante')",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($lamina->Lamina->Id)
			);
			$conexionBd->EjecutarComando();
		}
		foreach($this->laminasInteresante as $lamina)
		{
			$conexionBd->Sentencia = sprintf("INSERT LaminasIntercambio VALUES (%s, %s, 'AInteresado')",
			$conexionBd->Escapar($this->Id),
			$conexionBd->Escapar($lamina->Lamina->Id)
			);
			$conexionBd->EjecutarComando();
		}
		$conexionBd->ConfirmarTransaccion();
		$conexionBd->Desconectar();
	}

	function Validar()
	{
		$valido = true;
		if($this->Estado == 'Cancelado' || $this->Estado == 'NoValido')
		$valido = false;
		if($valido)
		foreach($this->laminasInteresado as $lamina)
		if($lamina->Album->Id != $this->AlbumInteresado->Id || $lamina->Cantidad <= 1)
		{
			$valido = false;
			break;
		}
		if($valido)
		foreach($this->laminasInteresante as $lamina)
		if($lamina->Album->Id != $this->AlbumInteresante->Id || $lamina->Cantidad <= 1)
		{
			$valido = false;
			break;
		}
		return $valido;
	}

	/**
	 *
	 * Aprueba el intercambio.
	 */
	function Aprobar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if($this->AlbumInteresante->Usuario->IdFacebook != $usuario->IdFacebook)
		throw new Exception('Las aprobaciones sólo las puede hacer el dueño del álbum');
		if(!$this->EstaRegistrado)
		throw new Exception('No se puede aprobar un intercambio que no haya sido registrado');
		if($this->Estado == 'Aprobado')
		throw new Exception('Este intercambio ya fue aprobado');
		if(!$this->Validar())
		throw new Exception('Este intercambio no es válido, ya sea porque no hay láminas suficientes para realizar el intercambio o porque hay láminas qe no pertenecen al intercambio');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE IntercambioLaminas SET Estado = 'Aprobado' WHERE Id = %s",
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
		$this->estado = 'Aprobado';
		foreach($this->laminasInteresado as $lamina)
		{
			$this->AlbumInteresado->RetirarLamina($lamina->Lamina);
			$this->AlbumInteresante->AgregarLamina($lamina->Lamina);
		}
		foreach($this->laminasInteresante as $lamina)
		{
			$this->AlbumInteresante->RetirarLamina($lamina->Lamina);
			$this->AlbumInteresado->AgregarLamina($lamina->Lamina);
		}
	}

	/**
	 *
	 * Cancela el intercambio.
	 */
	function Cancelar()
	{
		$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
		if($this->AlbumInteresante->Usuario->IdFacebook != $usuario->IdFacebook && $this->AlbumInteresante->Usuario->IdFacebook != $usuario->IdFacebook)
		throw new Exception('Los rechazos de intercambio sólo las puede hacer los dueños del álbum');
		if(!$this->EstaRegistrado)
		throw new Exception('No se pueden rechazar un intercambio que no está registrado');
		if($this->Estado == 'Cancelado' || $this->Estado == 'Aceptado')
		throw new Exception('Este intercambio fue cancelado o aprobado con anterioridad');
		if(!$this->Validar())
		throw new Exception('Este intercambio no es válido');
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("UPDATE IntercambioLaminas SET Estado = 'Cancelado' WHERE Id = %s",
		$conexionBd->Escapar($this->Id)
		);
		$conexionBd->EjecutarComando();
		$conexionBd->Desconectar();
	}

	/**
	 *
	 * Obtiene los intercambios pendientes de un usuario.
	 * @param UsuarioFacebook $Usuario
	 */
	function ObtenerSolicitudesUsuario(UsuarioFacebook $Usuario)
	{

	}

	/**
	 *
	 * Carga un intercambio específico.
	 * @param Id del intercambio $Id
	 * @throws Exception Se arroja cuando el intercambio es inválido
	 * @return IntercambioLaminas
	 */
	static public function Cargar($Id)
	{
		$intercambio = null;
		$conexionBd = new ConectorBaseDatos();
		$conexionBd->Sentencia = sprintf("SELECT Estado, AlbumInteresado, AlbumInteresante FROM IntercambioLaminas WHERE Id = %s",
		$conexionBd->Escapar($Id)
		);
		$datosIntercambio = $conexionBd->EjecutarConsulta();
		if(is_array($datosIntercambio) && count($datosIntercambio) == 1)
		{
			$albumInteresante = Album::Obtener($datosIntercambio['AlbumInteresante']);
			$albumInteresado = Album::Obtener($datosIntercambio['AlbumInteresado']);
			try
			{
				$intercambio = new IntercambioLaminas($albumInteresante, true);
				$intercambio->estado = $datosIntercambio['Estado'];
				$intercambio->albumInteresado = $albumInteresado;
				$conexionBd->Sentencia = sprintf("SELECT Lamina, Direccion FROM LaminasIntercambio Intercambio = %s",
				$conexionBd->Escapar($Id)
				);
				$intercambios = $conexionBd->EjecutarConsulta();
				if(!is_array($intercambios) || count($intercambios) <= 0)
				throw new Exception('Los datos de intercambio no son válidos');
				foreach($intercambios as $dIntercambio)
				{
					switch($dIntercambio['Direccion'])
					{
						case 'AInteresante':
							$intercambio->AgregarLaminaInteresado(LaminaAlbum::Obtener($intercambio->AlbumInteresado, Lamina::ObtenerLamina($dIntercambio['Lamina'])));
							break;
						case 'AInteresado':
							$intercambio->AgregarLaminaInteresante(LaminaAlbum::Obtener($intercambio->AlbumInteresante, Lamina::ObtenerLamina($dIntercambio['Lamina'])));
							break;
					}
				}
			}
			catch(Exception $e)
			{
				$conexionBd->Sentencia = sprintf("UPDATE IntercambioLaminas SET Estado = 'NoValido' WHERE Id = %s",
				$conexionBd->Escapar($Id)
				);
				$conexionBd->EjecutarComando();
				throw new Exception('El intercambio a cargar no es válido');
			}
		}
		$conexionBd->Desconectar();
		return $intercambio;
	}
}
?>