(function($) {
	
	function bagui(Elemento) {
		var data = Elemento.data('BotonAjax');
		if(data.Estado == 'Deshabilitado' || data.Url == null || data.Datos == null)
		{
			Elemento.addClass('Deshabilitado');
			Elemento.removeClass('Activado');
			Elemento.removeClass('Desactivado');
		} else if(data.Estado == 'Activado') {
			Elemento.addClass('Activado');
			Elemento.removeClass('Deshabilitado');
			Elemento.removeClass('Desactivado');
		} else if(data.Estado == 'Desactivado') {
			Elemento.addClass('Desactivado');
			Elemento.removeClass('Activado');
			Elemento.removeClass('Deshabilitado');
		} else {
			$.error('El estado ' + data.Estado + ' no es válido');
		}
	}
	
	var Metodos = {
		init: function(Opciones) {
			var Configuracion = {
				'Url': null,
				'Datos': null,
				'Estado': 'Deshabilitado',
				'EnEjecucion': false
			};
			return this.each(function() {
				if(Opciones)
					$.extend(Configuracion, Opciones);
				var data = $(this).data('BotonAjax');
				if(!data) {
					data = $.extend(true, {}, Configuracion);
					$(this).data('BotonAjax', data);
					$(this).addClass('BotonAjax');
				}
				//$(this).bind('click.BotonAjax', Metodos.hacerAjax);
				bagui($(this));
			});
		},
		hacerAjax: function() {
			return this.each(function() {
				var elm = $(this);
				var data = elm.data('BotonAjax');
				if(data) {
					if(data.Estado != 'Deshabilitado' && data.Url != null && data.Datos != null && !data.EnEjecucion) {
						data.EnEjecucion = true;
						elm.fadeTo('fast', 0.5);
						$.ajax({
							url: data.Url,
							data: data.Datos,
							complete: function(datar, estado) {
								data.EnEjecucion = false;
								if(estado == 'success' || estado == 'notmodified') {
									var respuesta = $.parseJSON(datar.responseText);
									if(respuesta.ejecucion == 'OK') {
										if(respuesta.EstadoDeseado) {
											data.Estado = respuesta.EstadoDeseado;
										} else {
											if (data.Estado == 'Activado') {
												data.Estado = 'Desactivado';
											} else {
												data.Estado = 'Activado';
											}
										}
									} else {
										if(respuesta.EstadoDeseado) {
											data.Estado = respuesta.EstadoDeseado;
										}
										if(respuesta.MensajeError) {
											alert(respuesta.MensajeError);
										} else {
											alert('Ocurrió un error que imposibilita terminar la acción correctamente');
										}
									}
								} else {
									alert('Hubo un error en la conexión que impidió completar la acción solicitada');
								}
								bagui(elm);
								elm.fadeTo('fast', 1);
							}
						});
					}
				} else {
					$.error('Botón Ajax no inicializado');
				}
			});
		}
	};
	
	$.fn.BotonAjax = function(Metodo) {
		if(Metodos[Metodo]) {
			return Metodos[Metodo].apply(this, Array.prototype.slice.call(arguments, 1));
		} else if (typeof Metodo === 'object' || !Metodo) {
			return Metodos.init.apply(this, arguments);
		} else {
			$.error(Metodo + ' es una función inexistente en este componente');
		}
	}
})(jQuery);