-- MySQL dump 10.13  Distrib 5.1.58, for unknown-linux-gnu (x86_64)
--
-- Host: localhost    Database: equipods_orion
-- ------------------------------------------------------
-- Server version	5.1.58-community-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Acta`
--

DROP TABLE IF EXISTS `Acta`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Acta` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Grupo` varchar(20) CHARACTER SET latin1 NOT NULL,
  `Autor` varchar(20) CHARACTER SET latin1 NOT NULL,
  `Fecha` date NOT NULL,
  `Resumen` longtext CHARACTER SET latin1,
  PRIMARY KEY (`Id`),
  KEY `IX_Acta_Fecha` (`Fecha`),
  KEY `FK_Acta_Autor` (`Autor`),
  KEY `IX_Acta_Grupo` (`Grupo`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=22 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Actividad`
--

DROP TABLE IF EXISTS `Actividad`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Actividad` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Tipo` int(11) NOT NULL,
  `Responsable` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `Descripcion` longtext COLLATE utf8_unicode_ci,
  `Fecha` date DEFAULT NULL,
  `ActaAsociada` int(11) DEFAULT NULL,
  `Cancelada` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`),
  KEY `FK_Actividad_Tipo` (`Tipo`),
  KEY `FK_Actividad_Responsable` (`Responsable`),
  KEY `FK_Actividad_ActaAsociada` (`ActaAsociada`)
) ENGINE=InnoDB AUTO_INCREMENT=42 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Album`
--

DROP TABLE IF EXISTS `Album`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Album` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `IdUsuario` varchar(20) CHARACTER SET latin1 NOT NULL,
  `NumeroCromosDisponibles` tinyint(3) unsigned NOT NULL DEFAULT '1',
  `FechaCreacion` date NOT NULL,
  PRIMARY KEY (`Id`),
  KEY `FK_Album_IdUsuario` (`IdUsuario`)
) ENGINE=InnoDB AUTO_INCREMENT=85 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Asistencia`
--

DROP TABLE IF EXISTS `Asistencia`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Asistencia` (
  `IdUsuario` varchar(20) CHARACTER SET latin1 NOT NULL,
  `IdActa` int(11) NOT NULL,
  PRIMARY KEY (`IdUsuario`,`IdActa`),
  KEY `FK_Asistencia_IdUsuario` (`IdUsuario`),
  KEY `FK_Asistencia_IdActa` (`IdActa`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `AsistenciaUsuario`
--

DROP TABLE IF EXISTS `AsistenciaUsuario`;
/*!50001 DROP VIEW IF EXISTS `AsistenciaUsuario`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `AsistenciaUsuario` (
  `IdUsuario` varchar(20),
  `Grupo` varchar(20),
  `Cantidad` bigint(21)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Temporary table structure for view `BonificacionNivel`
--

DROP TABLE IF EXISTS `BonificacionNivel`;
/*!50001 DROP VIEW IF EXISTS `BonificacionNivel`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `BonificacionNivel` (
  `IdUsuario` varchar(20),
  `Niveles` decimal(27,0)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `CambioNivel`
--

DROP TABLE IF EXISTS `CambioNivel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `CambioNivel` (
  `IdUsuario` varchar(20) CHARACTER SET latin1 NOT NULL,
  `NivelAnterior` tinyint(3) unsigned NOT NULL,
  `NivelActual` tinyint(3) unsigned NOT NULL,
  `BonificacionProcesada` tinyint(1) NOT NULL DEFAULT '0',
  `Fecha` date NOT NULL,
  PRIMARY KEY (`IdUsuario`,`NivelAnterior`,`NivelActual`),
  KEY `FK_CambioNivel_IdUsuario` (`IdUsuario`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `CodigoReclamado`
--

DROP TABLE IF EXISTS `CodigoReclamado`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `CodigoReclamado` (
  `IdUsuario` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `HashCodigo` varchar(40) COLLATE utf8_unicode_ci NOT NULL,
  `Fecha` date NOT NULL,
  `Puntos` smallint(5) unsigned NOT NULL,
  PRIMARY KEY (`IdUsuario`,`HashCodigo`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Grupo`
--

DROP TABLE IF EXISTS `Grupo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Grupo` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Nombre` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `Descripcion` tinytext COLLATE utf8_unicode_ci,
  `Sistema` tinyint(1) NOT NULL DEFAULT '0',
  `Defecto` tinyint(1) NOT NULL DEFAULT '0',
  `Participativo` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `IntercambioLaminas`
--

DROP TABLE IF EXISTS `IntercambioLaminas`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `IntercambioLaminas` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Fecha` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Estado` enum('Pendiente','Aceptado','Cancelado','NoValido') COLLATE utf8_unicode_ci NOT NULL DEFAULT 'Pendiente',
  `AlbumInteresado` int(10) unsigned NOT NULL,
  `AlbumInteresante` int(10) unsigned NOT NULL,
  PRIMARY KEY (`Id`),
  KEY `FK_IntercambioLaminas_AlbumInteresado` (`AlbumInteresado`),
  KEY `FK_IntercambioLaminas_AlbumInteresante` (`AlbumInteresante`),
  CONSTRAINT `FK_IntercambioLaminas_AlbumInteresado` FOREIGN KEY (`AlbumInteresado`) REFERENCES `Album` (`Id`) ON UPDATE CASCADE,
  CONSTRAINT `FK_IntercambioLaminas_AlbumInteresante` FOREIGN KEY (`AlbumInteresante`) REFERENCES `Album` (`Id`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Lamina`
--

DROP TABLE IF EXISTS `Lamina`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Lamina` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `Nombre` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `Clasificacion` varchar(45) COLLATE utf8_unicode_ci NOT NULL,
  `Orden` int(10) unsigned DEFAULT NULL,
  `Descripcion` text COLLATE utf8_unicode_ci NOT NULL,
  `NombreImagen` tinytext COLLATE utf8_unicode_ci,
  `Abundancia` float NOT NULL DEFAULT '1',
  `FechaRegistro` date NOT NULL,
  PRIMARY KEY (`Id`),
  UNIQUE KEY `UQ_Lamina_NombreCalsificacion` (`Nombre`,`Clasificacion`),
  KEY `IX_Lamina_Clasificacion` (`Clasificacion`),
  KEY `IX_Lamina_Abundancia` (`Abundancia`)
) ENGINE=InnoDB AUTO_INCREMENT=73 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `LaminaAlbum`
--

DROP TABLE IF EXISTS `LaminaAlbum`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `LaminaAlbum` (
  `IdAlbum` int(10) unsigned NOT NULL,
  `IdLamina` int(10) unsigned NOT NULL,
  `FechaAdquisicion` date NOT NULL,
  `Cantidad` int(10) unsigned NOT NULL DEFAULT '1',
  `Cambiable` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`IdAlbum`,`IdLamina`),
  KEY `FK_LaminaAlbum_IdAlbum` (`IdAlbum`),
  KEY `FK_LaminaAlbum_IdLamina` (`IdLamina`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `LaminasIntercambio`
--

DROP TABLE IF EXISTS `LaminasIntercambio`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `LaminasIntercambio` (
  `Intercambio` int(10) unsigned NOT NULL,
  `Lamina` int(10) unsigned NOT NULL,
  `Direccion` enum('AInteresado','AInteresante') COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`Intercambio`,`Lamina`),
  UNIQUE KEY `UQ_LaminasIntercambio_1` (`Intercambio`,`Lamina`),
  KEY `FK_LIntercambio_Interesado` (`Lamina`),
  KEY `FK_LIntercambio_Interc` (`Intercambio`),
  CONSTRAINT `FK_LIntercambio_Interc` FOREIGN KEY (`Intercambio`) REFERENCES `IntercambioLaminas` (`Id`) ON UPDATE CASCADE,
  CONSTRAINT `FK_LIntercambio_Interesado` FOREIGN KEY (`Lamina`) REFERENCES `Lamina` (`Id`) ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Permiso`
--

DROP TABLE IF EXISTS `Permiso`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Permiso` (
  `Permiso` varchar(20) CHARACTER SET latin1 NOT NULL,
  `Descripcion` longtext CHARACTER SET latin1,
  PRIMARY KEY (`Permiso`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `PermisoGrupo`
--

DROP TABLE IF EXISTS `PermisoGrupo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PermisoGrupo` (
  `Grupo` int(10) unsigned NOT NULL,
  `Permiso` varchar(20) CHARACTER SET latin1 NOT NULL,
  PRIMARY KEY (`Grupo`,`Permiso`),
  KEY `FK_PermisoGrupo_Grupo` (`Grupo`),
  KEY `FK_PermisoGrupo_Permiso` (`Permiso`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `PermisoUsuario`
--

DROP TABLE IF EXISTS `PermisoUsuario`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PermisoUsuario` (
  `Usuario` varchar(20) CHARACTER SET latin1 NOT NULL,
  `Permiso` varchar(20) CHARACTER SET latin1 NOT NULL,
  PRIMARY KEY (`Usuario`,`Permiso`),
  KEY `FK_PermisoUsuario_Usuario` (`Usuario`),
  KEY `FK_PermisoUsuario_Permiso` (`Permiso`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `PropiedadLamina`
--

DROP TABLE IF EXISTS `PropiedadLamina`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `PropiedadLamina` (
  `IdLamina` int(10) unsigned NOT NULL,
  `Tipo` varchar(50) CHARACTER SET latin1 NOT NULL,
  `Nombre` varchar(50) CHARACTER SET latin1 NOT NULL,
  `Valor` tinytext CHARACTER SET latin1 NOT NULL,
  PRIMARY KEY (`IdLamina`,`Tipo`,`Nombre`),
  KEY `FK_PropiedadLamina_IdLamina` (`IdLamina`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Puntos`
--

DROP TABLE IF EXISTS `Puntos`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Puntos` (
  `Id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `IdUsuario` varchar(20) CHARACTER SET latin1 NOT NULL,
  `Fecha` date NOT NULL,
  `Descripcion` tinytext CHARACTER SET latin1 NOT NULL,
  `Cantidad` smallint(6) NOT NULL,
  PRIMARY KEY (`Id`),
  KEY `FK_Puntos_IdUsuario` (`IdUsuario`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=196 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `PuntosUsuario`
--

DROP TABLE IF EXISTS `PuntosUsuario`;
/*!50001 DROP VIEW IF EXISTS `PuntosUsuario`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `PuntosUsuario` (
  `IdUsuario` varchar(20),
  `Puntos` decimal(27,0)
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `TipoActividad`
--

DROP TABLE IF EXISTS `TipoActividad`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `TipoActividad` (
  `Id` int(11) NOT NULL AUTO_INCREMENT,
  `Nombre` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `Descripcion` longtext COLLATE utf8_unicode_ci,
  `ItemPuntos` blob NOT NULL,
  PRIMARY KEY (`Id`),
  UNIQUE KEY `UQ_TipoActividad_Nombre` (`Nombre`)
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `UsuarioFacebook`
--

DROP TABLE IF EXISTS `UsuarioFacebook`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `UsuarioFacebook` (
  `IdFacebook` varchar(20) CHARACTER SET latin1 NOT NULL,
  `DatosFacebook` blob NOT NULL,
  `ValidadoPor` varchar(20) CHARACTER SET latin1 DEFAULT NULL,
  `UltimaActualizacion` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `SuperUsuario` tinyint(1) NOT NULL DEFAULT '0',
  `Experiencia` smallint(5) unsigned NOT NULL DEFAULT '0',
  `FbAccessToken` tinytext COLLATE utf8_unicode_ci,
  PRIMARY KEY (`IdFacebook`),
  KEY `FK_UsuarioFacebook_ValidadoPor` (`ValidadoPor`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `UsuarioGrupo`
--

DROP TABLE IF EXISTS `UsuarioGrupo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `UsuarioGrupo` (
  `Grupo` int(10) unsigned NOT NULL,
  `Usuario` varchar(20) CHARACTER SET latin1 NOT NULL,
  `FechaRegistro` date NOT NULL,
  `Administrador` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`Grupo`,`Usuario`),
  KEY `FK_UsuarioGrupo_Grupo` (`Grupo`),
  KEY `FK_UsuarioGrupo_Usuario` (`Usuario`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `x_pub`
--

DROP TABLE IF EXISTS `x_pub`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `x_pub` (
  `IdF` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `IdUsuario` varchar(20) COLLATE utf8_unicode_ci NOT NULL,
  `Comp` text COLLATE utf8_unicode_ci,
  `Fecha` date NOT NULL,
  `TStamp` int(11) NOT NULL,
  PRIMARY KEY (`Fecha`,`TStamp`,`IdUsuario`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Final view structure for view `AsistenciaUsuario`
--

/*!50001 DROP TABLE IF EXISTS `AsistenciaUsuario`*/;
/*!50001 DROP VIEW IF EXISTS `AsistenciaUsuario`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`equipods`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `AsistenciaUsuario` AS select `Asistencia`.`IdUsuario` AS `IdUsuario`,`Acta`.`Grupo` AS `Grupo`,count(0) AS `Cantidad` from (`Asistencia` left join `Acta` on((`Asistencia`.`IdActa` = `Acta`.`Id`))) group by `Asistencia`.`IdUsuario`,`Acta`.`Grupo` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `BonificacionNivel`
--

/*!50001 DROP TABLE IF EXISTS `BonificacionNivel`*/;
/*!50001 DROP VIEW IF EXISTS `BonificacionNivel`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`equipods`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `BonificacionNivel` AS select `CambioNivel`.`IdUsuario` AS `IdUsuario`,sum((`CambioNivel`.`NivelActual` - `CambioNivel`.`NivelAnterior`)) AS `Niveles` from `CambioNivel` where (`CambioNivel`.`BonificacionProcesada` = 0) group by `CambioNivel`.`IdUsuario` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;

--
-- Final view structure for view `PuntosUsuario`
--

/*!50001 DROP TABLE IF EXISTS `PuntosUsuario`*/;
/*!50001 DROP VIEW IF EXISTS `PuntosUsuario`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`equipods`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `PuntosUsuario` AS select `Puntos`.`IdUsuario` AS `IdUsuario`,sum(`Puntos`.`Cantidad`) AS `Puntos` from `Puntos` group by `Puntos`.`IdUsuario` order by `Puntos`.`IdUsuario` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-07-17  0:00:08
