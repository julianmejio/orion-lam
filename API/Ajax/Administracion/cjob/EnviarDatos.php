<?php
//error_reporting(0);
require_once('../../../Orion/class.Config.inc');
require_once('../../../Orion/class.Uri.inc');
require_once('../../../Facebook/src/facebook.php');
function __autoload($Clase)
{
	require_once ("../../../Orion/class.$Clase.inc");
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
				}else if(!preg_match('/GeoLiteCity\.dat/', $file)){
					$zipArchive->addFile($dir . $file, $zipdir . $file);
				}
			}
		}
	}
}
// 0. Carpeta de respaldos
$rutaArchivo = '../../../../Sistema/Respaldos/';
// 1. Definir carpeta de respaldo activa
$fecha = date("YmdHis");
mkdir("$rutaArchivo$fecha");
// 2.  Definir nombres de respaldo
// 2.1. Base de datos
$nombreArchivoBdEstructura = 'estructura.sql.gz';
$nombreArchivoBdDatos = 'datos.sql.gz';
// 2.2. Archivos de aplicación
$nombreArchivoApp = 'aplicacion.zip';
// 3. Realizar copia de base de datos
$dumper = ParametrosDb::CargarArchivo('../../../../Sistema/Datos/ConexionesDb/dumper.dbp');
exec("mysqldump -d -h$dumper->Servidor -u$dumper->Usuario -p$dumper->Clave $dumper->BaseDatos | gzip > $rutaArchivo$fecha/$nombreArchivoBdEstructura");
exec("mysqldump --no-create-db --no-create-info -h$dumper->Servidor -u$dumper->Usuario -p$dumper->Clave $dumper->BaseDatos | gzip > $rutaArchivo$fecha/$nombreArchivoBdDatos");
// 4. Realizar copia de aplicación
$zip = new ZipArchive();
$zip->open("$rutaArchivo$fecha/$nombreArchivoApp", ZipArchive::CREATE);
addFolderToZip('../../../../', $zip, '');
$zip->close();
// 5. Crear paquete
$zip = new ZipArchive();
$zip->open("$rutaArchivo$fecha.zip", ZipArchive::CREATE);
addFolderToZip("$rutaArchivo$fecha/", $zip);
$zip->close();
// 6. Eliminar archivos restantes
unlink("$rutaArchivo$fecha/$nombreArchivoBdEstructura");
unlink("$rutaArchivo$fecha/$nombreArchivoBdDatos");
unlink("$rutaArchivo$fecha/$nombreArchivoApp");
rmdir("$rutaArchivo$fecha");
require_once('../../../Phpmailer/class.phpmailer.php');
require_once('../../../Phpmailer/class.pop3.php');
require_once('../../../Phpmailer/class.smtp.php');
$correo = new PHPMailer();
$correo->CharSet = 'utf-8';
$correo->IsSMTP();
$correo->Host = Config::ServidorSmtp;
$correo->Port = 465;
$correo->SMTPAuth = true;
$correo->Username = Config::UsuarioSmtp;
$correo->Password = Config::ClaveSmtp;
$correo->From = 'no-reply@mehiocore.com';
$correo->FromName = 'Orión App';
$correo->AddAddress('julianmejio@gmail.com', 'Julián Mejio Cárdenas');
$correo->AddAttachment("$rutaArchivo$fecha.zip");
$correo->Subject = "Respaldo de apliación de Orión ($fecha)";
$correo->Body = date("Y-m-d H:i:s");
$correo->Send();
exit;
?>