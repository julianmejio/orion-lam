<?php
error_reporting(0);
$mapaImg = null;
if(isset($_REQUEST['m']))
	$mapaImg = $_REQUEST['m'];
if(!preg_match('/^[0-9a-f]{40}$/', $mapaImg))
{
	header('HTTP/1.1 400 Bad Request');
	exit;
}
if(!isset($_SESSION))
	session_start();
if($mapaImg == null || !isset($_SESSION[$mapaImg]) || !file_exists($_SESSION[$mapaImg]))
{
	if(!isset($_SESSION[$mapaImg]))
	{
		header('HTTP/1.1 404 Not Found');
		echo "ERROR: La imagen que estás intentando observar no pertenece a tu sesión";
		exit;
	}
	else if(!file_exists($_SESSION[$mapaImg]))
		$_SESSION[$mapaImg] = 'Orion.jpg';
}
if(!isset($_SESSION["cacheInit"]))
	$_SESSION["cacheInit"] = time();
header("ETag: \"$mapaImg\"");
header('Last-Modified: ' . date("r", filemtime($_SESSION[$mapaImg])));
header('Content-Type: image/jpeg');
header('Content-Disposition: inline');
header('Content-Length: ' . filesize($_SESSION[$mapaImg]));
header('Accept-Ranges: bytes');
header('Cache-control: max-age=' . (($_SESSION['cacheInit'] + 43200) - time()) . ', private');
header('Expires: ' . date("r", $_SESSION["cacheInit"] + 43200));
@readfile($_SESSION[$mapaImg]);
?>