<?php
require_once('../../../API/Orion/class.Config.inc');
require_once('../../../API/Orion/class.Uri.inc');
require_once('../../../API/Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once(Uri::ObtenerRutaARaiz() . "/API/Orion/class.$Clase.inc");
}
Script::Cargar('VerificarSesion', true);
Script::Cargar('NotificadorNivelExp');
if(!isset($_SESSION))
	session_start();
$usuarioActivo = UsuarioFacebook::ObtenerUsuarioActivo();
$albumes = $usuarioActivo->ObtenerAlbumes();
if(isset($_POST) && isset($_POST['Nombre']) && isset($_POST['Descripcion']))
{
	try
	{
		$grupo = new Grupo($_POST['Nombre'], $_POST['Descripcion']);
		$grupo->Registrar();
		if(isset($_POST['Defecto']))
			$grupo->HacerPredeterminado();
		$okCreacion = true;
	}
	catch(Exception $e)
	{
		$errorCreacion = $e->getMessage();
	}
}
$grupos = Grupo::ObtenerTodos();
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"><!-- InstanceBegin template="/Templates/General.dwt.php" codeOutsideHTMLIsLocked="false" -->
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<!-- InstanceBeginEditable name="doctitle" -->
<title>Administrar grupos - Club Orión de Astronomía</title>
<!-- InstanceEndEditable -->
<!-- InstanceBeginEditable name="head" -->
<!-- InstanceEndEditable -->
<link href="../../../Estilos/General/ItemsFacebook.css" rel="stylesheet" type="text/css" />
<link href="../../../Estilos/General/Nonometro.css" rel="stylesheet" type="text/css" />
<link href="../../../Estilos/General/General.css" rel="stylesheet" type="text/css" />
<link href="../../../API/Jqueryui/css/orion/jquery-ui-1.8.9.custom.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../../API/Jquery/jquery-1.6.1.min.js"></script>
<script type="text/javascript" src="../../../API/Jqueryui/js/jquery-ui-1.8.9.custom.min.js"></script>
<script type="text/javascript">
$(document).ready(function(){
	$("#Menu").accordion({navigation: true});
<?php
if(isset($_SESSION['notificarNivel']))
{
?>
	alert("¡Felicitaciones! Por ser tan ñoño has recibido una bonificación de láminas para reclamar :D");
<?php
}
?>
});
<?php
if($albumes == null)
{
?>
function obtenerAlbum()
{
	$("#EstadoAlbum").html("<span class=\"InfoCarga\">Estamos creando tu álbum…</span>");	
	$.getJSON('<?php echo Config::RutaBase; ?>/Album/Crear.json', function(data){
		if(data['Estado'] == 'OK')
		{
			$("#EstadoAlbum").html("<a href=\"<?php echo Config::RutaBase; ?>/Panel/Album\">¡Ya tienes tu álbum de láminas, míralo!</a>");
		}
		else
			$("#EstadoAlbum").html("<span class=\"MensajeError\">No se pudo crear tu álbum. Intenta más tarde</a>");
	});
}
<?php
}
?>
</script>
<!-- InstanceBeginEditable name="posthead" -->
<link href="../../../Estilos/General/Tablas.css" rel="stylesheet" type="text/css" />
<link href="../../../API/JqueryPlugs/BotonAjax/BotonAjax.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../../../Estilos/General/Tablas.js"></script>
<script type="text/javascript" src="../../../API/JqueryPlugs/BotonAjax/BotonAjax.js"></script>
<script type="text/javascript">
$(document).ready(function() {
	$(".TablaAccion").TablaAccion();
	$(".BotonAjax").click(function() {
		$(this).BotonAjax('hacerAjax');
	});
	$(".botonEliminar").click(function(){
		if(confirm("¿Está seguro de eliminar este grupo?\r\nSi elimina este grupo, se eliminarán todos los registros que se asocien con este, incluídos los permisos de usuario"))
		{
			var gLink = $(this);
			$.ajax({
				url: '../../../Administracion/EliminarGrupo.json',
				data: {g: $(this).attr("rel")},
				complete: function(datar, estado) {
					if(estado == 'success' || estado == 'notmodified') {
						var respuesta = $.parseJSON(datar.responseText);
						if(respuesta.Estado == 'OK') {
							gLink.parents("tr").hide('medium', function() {
								$(this).remove();
								$(".TablaAccion").TablaAccion();
							});
						} else {
							if(respuesta.Mensaje) {
								alert(respuesta.Mensaje);
							} else {
								alert("Ocurrió un error al intentar eliminar el grupo");
							}
						}
					} else {
						alert("Ocurrió un error en la conexión cuando se intentaba eliminar elgrupo seleccionado");
					}
				}
			});
		}
	});
});
</script>
<link href="../../../Estilos/General/Formularios.css" rel="stylesheet" type="text/css" />
<!-- InstanceEndEditable -->
</head>

<body>
<div id="ContenedorPanel">
  <div id="Panel" class="ui-corner-all">
    <div id="CabeceraPanel">
      <div id="ContenedorImagenLogo"><img src="../../../Imagenes/LogoPanel.png" width="202" height="100" alt="Logo del CLub Orión de Astronomía" /></div>
      <div id="ContenedorNombrePanel"></div>
      <div style="clear:both"></div>
    </div>
    <div id="ControlesPanel">
      <div id="ExtraBSPanel">
        <div id="IdentificadorUsuario"><?php
	  $itemUsuario = $usuarioActivo->GenerarItemX();
	  echo $itemUsuario->ATexto(true, false);
	  ?></div>
        <div id="Menu">
          <h2><a href="javascript:void(0)">Mi cuenta</a></h2>
          <div>
            <ul>
              <li><a href="../../Album">Álbum</a></li>
            </ul>
          </div>
          <h2><a href="javascript:void(0)">Club</a></h2>
          <div>
            <ul>
              <li><a href="../../Grupo/Lista.php">Grupos</a></li>
<?php
if($usuarioActivo->TienePermiso('RegistrarActas'))
{
?>
              <li><a href="../../Acta/Crear.php">Registrar acta</a></li>
<?php
}
?>
            </ul>
          </div>
<?php
if($usuarioActivo->TienePermiso('VerMenuAdmin'))
{
?>
          <h2><a href="javascript:void(0)">Administración</a></h2>
          <div><ul>
          <li>Actividad
<?php
if($usuarioActivo->TienePermiso('CrearTipoActividad') || $usuarioActivo->TienePermiso('ModTipoActividad'))
{
?>
<ul><li style="margin-left:5px; "><a href="<?php echo Config::RutaBase; ?>/Panel/Administracion/Actividad/Tipo.php">Tipos</a></li></ul>
<?php
}
?></li>
<?php
if($usuarioActivo->TienePermiso('CrearLaminaAlbum') || $usuarioActivo->TienePermiso('ModificarLaminaAlbum'))
{
?>
          <li><a href="<?php echo Config::RutaBase; ?>/Panel/Administracion/Album/">Álbum</a></li>
<?php
}
?>
<?php
	if($usuarioActivo->TienePermiso('UsuarioBajoNivel'))
	{
?>
          <li><a href="<?php echo Config::RutaBase; ?>/Panel/Administracion/CodigoArbitrario.php">Código arbitrario</a></li>
<?php
	}
	if($usuarioActivo->TienePermiso('VerGrupos'))
	{
?>
          <li><a href="../Grupo/">Grupos</a></li>
<?php
	}
?>
          <li><a href="../Usuario/">Usuarios</a></li>
          </ul></div>
<?php
}
?>
        </div>
      </div>
      <div id="ContenidoPanel">
        <div id="BarraEstado">
          <ul style="float:right">
<?php
$puntos = null;
try
{
	$puntos = $usuarioActivo->ObtenerPuntosDisponibles();
}
catch(Exception $e){}
if(is_int($puntos))
{
?>
            <li><?php if($puntos >= 1) { ?>Tienes <a href="<?php echo Config::RutaBase; ?>/Panel/Tienda"><span class="NombreEstado"><?php echo $puntos; ?> puntos</span></a><?php } else echo 'Tienes <a href="' . Config::RutaBase . '/Panel/Tienda">0 puntos</a>'; ?></li>
<?php
}
else
{
?>
            <li><?php echo MensajeHtml::InLightError('Tu puntaje no es visible.'); ?></li>
<?php
}
?>
            <li id="EstadoAlbum"><?php
$strAlbum = '<a href="javascript:void(0)" onclick="obtenerAlbum()">¡Obtén tu álbum de láminas!</a>';
if($albumes != null && count($albumes) > 1)
	$strAlbum = '<a href="' . Config::RutaBase . '/Panel/Album/Selector.php">Mira tus álbumes</a>';
else if($albumes != null && count($albumes) == 1)
{
	$album = $albumes[0];
	$strAlbum = '<span class="NombreEstado"><a href="' . Config::RutaBase . '/Panel/Album">Tu álbum</a></span>: ';
	$strAlbum .= count($album->Cromos) . ' láminas';
	if($album->NumeroCromosDisponibles >= 1)
		$strAlbum .= ', <a href="' . Config::RutaBase . '/Panel/Album/Llenar.php">¡Tienes ' . $album->NumeroCromosDisponibles . ' láminas disponibles!</a>';
}
echo $strAlbum;
?></li>
            <li><a href="../../../Sesion/CerrarSesion.php">Cerrar sesión</a></li>
          </ul>
          <div style="clear:both"></div>
        </div>
        <!-- InstanceBeginEditable name="ContenidoPanel" -->
        <h1>Administrar grupos</h1>
<?php
if(isset($okCreacion))
	echo '<p>Grupo creado.</p>';
else if(isset($errorCreacion))
	echo MensajeHtml::JqueryUiError($errorCreacion);
?>
        <h2>Crear</h2>
        <form id="fCrearGrupo" name="fCrearGrupo" method="post" action="">
          <table class="Formulario">
            <tr>
              <th scope="row"><label for="Nombre">Nombre</label></th>
              <td><input type="text" name="Nombre" id="Nombre" /></td>
            </tr>
            <tr>
              <th scope="row"><label for="Descripcion">Descripción</label></th>
              <td><textarea name="Descripcion" id="Descripcion" cols="45" rows="5"></textarea></td>
            </tr>
            <tr>
              <th scope="row">Opciones</th>
              <td><label>
                  <input type="checkbox" name="Defecto" id="Defecto" />
                  Por defecto</label><label title="Si se habilita esta opción, las asistencias y las actividades que se registren en este grupo se traducirán en puntos y experiencia a los usuarios">
                  <input type="checkbox" name="Participativo" id="Participativo" />
                  Participativo</label></td>
            </tr>
            <tr>
              <td colspan="2" scope="row"><input type="submit" name="button" id="button" value="Crear" /></td>
            </tr>
          </table>
          <h2>Lista de grupos</h2>
        </form>
        <?php
if(is_array($grupos))
{
?>
        <table class="TablaAccion">
          <tr>
            <th scope="col" style="width: 50%">Grupo</th>
            <th scope="col" style="width: 10%">Por defecto</th>
            <th scope="col" style="width: 10%" title="Si está activado, las asistencias y las actividades realizadas en este grupo se traducirán en puntos y experiencia">Participativo</th>
            <th scope="col" style="width: 10%">Miembros</th>
            <th scope="col" style="width: 20%">Acción</th>
          </tr>
          <?php
	foreach($grupos as $grupo)
	{
?>
          <tr>
            <td><?php echo "<a href=\"../../Grupo/Usuarios.php?g=$grupo->Id\">$grupo->Nombre</a>"; ?></td>
            <td><div id="bd<?php echo $grupo->Id; ?>_d"></div><script>$("#bd<?php echo $grupo->Id; ?>_d").BotonAjax({Url: '../../../Administracion/GrupoPropiedadHdlr.json', Datos: {g: <?php echo $grupo->Id; ?>, a: 'd'}, Estado: '<?php echo ($grupo->PorDefecto) ? 'Activado' : 'Desactivado'; ?>'});</script></td>
			<td><div id="bd<?php echo $grupo->Id; ?>_p"></div><script>$("#bd<?php echo $grupo->Id; ?>_p").BotonAjax({Url: '../../../Administracion/GrupoPropiedadHdlr.json', Datos: {g: <?php echo $grupo->Id; ?>, a: 'p'}, Estado: '<?php echo ($grupo->EsParticipativo) ? 'Activado' : 'Desactivado'; ?>'});</script></td>
            <td style="text-align:center"><?php echo count($grupo->Usuarios); ?></td>
            <td style="text-align:center"><?php if(!$grupo->EsSistema) { ?>
              <a class="botonEliminar" href="javascript:void(0)" rel="<?php echo $grupo->Id; ?>">Eliminar</a>
              <?php } else echo "Sistema"; ?><br/>
              <a href="Permisos.php?g=<?php echo $grupo->Id; ?>">Permisos</a>
              </td>
          </tr>
          <?php
	}
?>
        </table>
        <?php
}
else
	echo MensajeHtml::JqueryUiError('No hay grupos registrados');
?>
        <!-- InstanceEndEditable --></div>
      <div style="clear:both"></div>
    </div>
  </div>
</div>
</body>
<!-- InstanceEnd --></html>