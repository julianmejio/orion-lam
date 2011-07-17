<?php
class MensajeHtml
{
	public static function MensajeJqueryUi($Mensaje, $Tipo, $Icono)
	{
		$contenedorMensaje = new EtiquetaXml('div');
		$contenedorMensaje->AgregarAtributo(new AtributoXml('class', 'ui-widget'));
		$contenedorEstiloMensaje = new EtiquetaXml('div');
		$contenedorEstiloMensaje->AgregarAtributo(new AtributoXml('class', 'ui-state-' . htmlentities($Tipo) . ' ui-corner-all'));
		$parrafoMensaje = new EtiquetaXml('p');
		$parrafoMensaje->AgregarContenido(MensajeHtml::generarIconoX($Icono));
		$parrafoMensaje->AgregarContenido($Mensaje, true);
		$contenedorEstiloMensaje->AgregarContenido($parrafoMensaje);
		$contenedorMensaje->AgregarContenido($contenedorEstiloMensaje);
		return $contenedorMensaje->ATexto(true, false);
	}

	public static function MensajeInLight($Mensaje, $Icono, $Color = "#000")
	{
		$contenedorMensaje = new EtiquetaXml('span');
		$contenedorMensaje->AgregarAtributo(new AtributoXml('style', "color: $Color"));
		$contenedorMensaje->AgregarContenido(MensajeHtml::generarIconoX($Icono));
		$contenedorMensaje->AgregarContenido($Mensaje);
		return $contenedorMensaje->ATexto(true, false);
	}

	private static function generarIconoX($Icono)
	{
		$icono = new EtiquetaXml('span');
		$icono->AgregarAtributo(new AtributoXml('class', 'ui-icon ui-icon-' . htmlentities($Icono)));
		$icono->AgregarAtributo(new AtributoXml('style', 'float: left; margin-right: 0.5 em'));
		return $icono;
	}

	public static function JqueryUiInfo($Mensaje)
	{
		return MensajeHtml::MensajeJqueryUi($Mensaje, 'highlight', 'info');
	}

	public static function JqueryUiError($Mensaje)
	{
		return MensajeHtml::MensajeJqueryUi($Mensaje, 'error', 'alert');
	}

	public static function InLightError($Mensaje)
	{
		return MensajeHtml::MensajeInLight($Mensaje, 'alert', '#F00');
	}
}
?>