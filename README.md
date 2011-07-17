Orión LAM
=========

**NOTA**: Este código fuente, escrito en 2010 - 2011, es obsoleto y no
mantenido. Se ha liberado para propósitos históricos.

Instalación
-----------

Esta aplicación no ha sido construida sobre un framework por lo que su
instalación es compleja.

La forma más fácil para ejecutar este código consiste en sobreescribir el
método `ParametrosDb::Cargar(string)` con los datos de una conexión a una
base de datos y sobreescribir el método
`UsuarioFacebook::ObtenerUsuarioActivo()` para retornar un objeto con un ID
de Facebook que ya haya sido previamente cargado en la base de datos.

La base de datos se puede encontrar en `estructura.sql`, sin embargo no se
provee información ya que esta contiene información sensible de los usuarios. 
Más adelante se puede proveer una construcción de *fixtures* con la cual se
pueda correr sin exponer información sensible de usuarios.

Por último, se debe copiar el archivo `/API/Orion/class.Config.dist.inc` a
`/API/Orion/class.Config.inc` y parametrizar la aplicación según el entorno de
ejecución.