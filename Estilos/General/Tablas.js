(function($){
	$.fn.TablaAccion = function(Opciones) {
		var Configuracion = {
			'EstiloOdd': 'ff2'
		};
		
		if(Opciones)
			$.extend(Configuracion, Opciones);
		
		this.find("tr").removeClass(Configuracion.EstiloOdd);
		this.find("tr:odd").addClass(Configuracion.EstiloOdd);
		if(Configuracion.EstiloEven) {
			this.find("tr").removeClass(Configuracion.EstiloEven);
			this.find("tr:even").addClass(Configuracion.EstiloEven);
		}
	}
})(jQuery);