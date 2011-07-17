<?php
error_reporting(0);
require_once('../../Orion/class.Config.inc');
require_once('../../Orion/class.Uri.inc');
require_once('../../Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once ("../../Orion/class.$Clase.inc");
}

function addFolderToZip($dir, $zipArchive, $zipdir = ''){
	if (is_dir($dir)) {
		if ($dh = opendir($dir)) {
			if(!empty($zipdir)) $zipArchive->addEmptyDir($zipdir);
			while (($file = readdir($dh)) !== false) {
				if(!is_file($dir . $file)){
					if(($file !== ".") && ($file !== "..") && (!preg_match('/Respaldos/', $file))){
						addFolderToZip($dir . $file . "/", $zipArchive, $zipdir . $file . "/");
					}
				}else{
					$zipArchive->addFile($dir . $file, $zipdir . $file);
				}
			}
		}
	}
}
try
{
	$usuario = UsuarioFacebook::ObtenerUsuarioActivo();
	if($usuario->EsSuperUsuario)
	{
		// 0. Carpeta de respaldos
		$rutaArchivo = '../../../Sistema/Respaldos/';
		// 1. Definir carpeta de respaldo activa
		$fecha = date("YmdHis");
		mkdir("$rutaArchivo$fecha");
		// 2.  Definir nombres de respaldo
		// 2.1. Base de datos
		$nombreArchivoBd = 'datos.sql';
		// 2.2. Archivos de aplicación
		$nombreArchivoApp = 'aplicacion.zip';
		// 3. Realizar copia de base de datos
		$dumper = ParametrosDb::Cargar('dumper');
		exec("mysqldump -h$dumper->Servidor -u$dumper->Usuario -p$dumper->Clave $dumper->BaseDatos > $rutaArchivo$fecha/$nombreArchivoBd");
		// 4. Realizar copia de aplicación
		$zip = new ZipArchive();
		$zip->open("$rutaArchivo$fecha/$nombreArchivoApp", ZipArchive::CREATE);
		addFolderToZip('../../../', $zip, '');
		$zip->close();
		// 5. Crear paquete
		$zip = new ZipArchive();
		$zip->open("$rutaArchivo$fecha.zip", ZipArchive::CREATE);
		addFolderToZip("$rutaArchivo$fecha/", $zip);
		$zip->close();
		// 6. Eliminar archivos restantes
		unlink("$rutaArchivo$fecha/$nombreArchivoBd");
		unlink("$rutaArchivo$fecha/$nombreArchivoApp");
		rmdir("$rutaArchivo$fecha");
		header('Content-Type: application/zip');
		header("Content-Disposition: attachment; filename=respaldo-orion-$fecha.zip");
		header('Content-Lenght: ' . filesize("$rutaArchivo$fecha.zip"));
		readfile("$rutaArchivo$fecha.zip");
		exit;
	}
	else
	{
		header('HTTP/1.1 403 Forbidden');
		exit;
	}
}
catch(Exception $e) { }
?>