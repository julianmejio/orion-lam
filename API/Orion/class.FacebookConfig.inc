<?php
class FacebookConfig
{
	static public function ObtenerConfiguracionAplicacion()
	{
		return array(
			'appId' => Config::FbAppId,
			'secret' => Config::FbSecret
		);
	}
}
?>