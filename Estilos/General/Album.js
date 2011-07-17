// Realiza el elipsis en la descripción, enlaza el trigger para abrir la descripción y genera
// la funcionalidad de LightBox.
// REQUIERE
// Plugs: Autoellipsis, lightBox, JqueryTools
// Estilos: ItemsAlbum
// Código autogenerado: contenedor de descripción larga
// USO
// 1. generar lámina
// 2. Ejecutar función
function prepararLamina() {
	$(".Lamina .Nombre").ellipsis({setTitle: 'onEllipsis'});
	$(".Lamina .Propiedades").ellipsis({setTitle: 'onEllipsis'});
	$(".Lamina .Propiedades span[title]").css('cursor', 'pointer');
	$(".Lamina .Propiedades").click(function() {
		var attrTitle = $(this).find("span").attr("title");
		if(typeof attrTitle !== 'undefined' && attrTitle !== false) {
			$("#InformacionLamina .TituloLamina").text($(this).parent().find(".Nombre").text());
			$("#InformacionLamina .Texto").text(attrTitle);
			$(this).overlay({target: $("#InformacionLamina"), load:true});
		}
	});
	$("a[rel=rtimg]").lightBox();
}