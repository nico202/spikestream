-- MySQL dump 10.11
--
-- Host: localhost    Database: SpikeStreamNetwork
-- ------------------------------------------------------
-- Server version	5.0.67

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
-- Current Database: `SpikeStreamNetwork`
--

/*!40000 DROP DATABASE IF EXISTS `SpikeStreamNetwork`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `SpikeStreamNetwork` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `SpikeStreamNetwork`;

--
-- Table structure for table `ConnectionGroups`
--

DROP TABLE IF EXISTS `ConnectionGroups`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ConnectionGroups` (
  `ConnectionGroupID` smallint(5) unsigned NOT NULL auto_increment,
  `NetworkID` smallint(6) NOT NULL,
  `Description` char(250) NOT NULL,
  `FromNeuronGroupID` smallint(5) unsigned NOT NULL,
  `ToNeuronGroupID` smallint(5) unsigned NOT NULL,
  `SynapseTypeID` smallint(5) unsigned NOT NULL,
  `Parameters` text,
  PRIMARY KEY  (`ConnectionGroupID`),
  KEY `ConnectionGroupIDIndex` (`ConnectionGroupID`),
  KEY `NetworkID_FK` (`NetworkID`),
  KEY `SynapseTypeID_FK` (`SynapseTypeID`),
  CONSTRAINT `ConnectionGroups_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `Networks` (`NetworkID`) ON DELETE CASCADE,
  CONSTRAINT `ConnectionGroups_ibfk_2` FOREIGN KEY (`SynapseTypeID`) REFERENCES `SynapseTypes` (`SynapseTypeID`) ON DELETE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `ConnectionGroups`
--

LOCK TABLES `ConnectionGroups` WRITE;
/*!40000 ALTER TABLE `ConnectionGroups` DISABLE KEYS */;
/*!40000 ALTER TABLE `ConnectionGroups` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Connections`
--

DROP TABLE IF EXISTS `Connections`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Connections` (
  `ConnectionID` bigint(20) unsigned NOT NULL auto_increment,
  `FromNeuronID` mediumint(8) unsigned NOT NULL,
  `ToNeuronID` mediumint(8) unsigned NOT NULL,
  `ConnectionGroupID` smallint(5) unsigned NOT NULL,
  `Delay` float unsigned NOT NULL,
  `Weight` float NOT NULL,
  `TempWeight` float default '0',
  PRIMARY KEY  (`ConnectionID`),
  KEY `FromNeuronIndex` (`FromNeuronID`,`ToNeuronID`),
  KEY `ToNeuronIndex` (`ToNeuronID`),
  KEY `ConnectionGroupID` (`ConnectionGroupID`),
  CONSTRAINT `Connections_ibfk_1` FOREIGN KEY (`ConnectionGroupID`) REFERENCES `ConnectionGroups` (`ConnectionGroupID`) ON DELETE CASCADE,
  CONSTRAINT `Connections_ibfk_2` FOREIGN KEY (`FromNeuronID`) REFERENCES `Neurons` (`NeuronID`) ON DELETE CASCADE,
  CONSTRAINT `Connections_ibfk_3` FOREIGN KEY (`ToNeuronID`) REFERENCES `Neurons` (`NeuronID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Connections`
--

LOCK TABLES `Connections` WRITE;
/*!40000 ALTER TABLE `Connections` DISABLE KEYS */;
/*!40000 ALTER TABLE `Connections` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Networks`
--

DROP TABLE IF EXISTS `Networks`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Networks` (
  `NetworkID` smallint(6) NOT NULL auto_increment,
  `Name` char(250) NOT NULL default 'Untitled',
  `Description` char(250) NOT NULL default 'Untitled',
  PRIMARY KEY  (`NetworkID`),
  KEY `NetworkIDIndex` (`NetworkID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Networks`
--

LOCK TABLES `Networks` WRITE;
/*!40000 ALTER TABLE `Networks` DISABLE KEYS */;
/*!40000 ALTER TABLE `Networks` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `NeuronGroups`
--

DROP TABLE IF EXISTS `NeuronGroups`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `NeuronGroups` (
  `NeuronGroupID` smallint(5) unsigned NOT NULL auto_increment,
  `NetworkID` smallint(6) NOT NULL,
  `Name` char(250) NOT NULL default 'Untitled',
  `Description` char(250) NOT NULL default 'No description',
  `Parameters` text,
  `NeuronTypeID` smallint(5) unsigned NOT NULL,
  PRIMARY KEY  (`NeuronGroupID`),
  KEY `NeuronGroupIDIndex` (`NeuronGroupID`),
  KEY `NetworkIDIndex` (`NetworkID`),
  KEY `NeuronTypeID_FK` (`NeuronTypeID`),
  CONSTRAINT `NeuronGroups_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `Networks` (`NetworkID`) ON DELETE CASCADE,
  CONSTRAINT `NeuronGroups_ibfk_2` FOREIGN KEY (`NeuronTypeID`) REFERENCES `NeuronTypes` (`NeuronTypeID`) ON DELETE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `NeuronGroups`
--

LOCK TABLES `NeuronGroups` WRITE;
/*!40000 ALTER TABLE `NeuronGroups` DISABLE KEYS */;
/*!40000 ALTER TABLE `NeuronGroups` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `NeuronTypes`
--

DROP TABLE IF EXISTS `NeuronTypes`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `NeuronTypes` (
  `NeuronTypeID` smallint(5) unsigned NOT NULL,
  `Description` char(250) NOT NULL,
  `ParameterTableName` char(100) NOT NULL,
  `ClassLibrary` char(100) NOT NULL,
  PRIMARY KEY  (`NeuronTypeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `NeuronTypes`
--

LOCK TABLES `NeuronTypes` WRITE;
/*!40000 ALTER TABLE `NeuronTypes` DISABLE KEYS */;
INSERT INTO `NeuronTypes` VALUES (1,'STDP1 Neuron','STDP1NeuronParameters','libstdp1neuron.so'),(2,'Weightless Neuron','WeigthlessNeuronParameters','undefined.so');
/*!40000 ALTER TABLE `NeuronTypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Neurons`
--

DROP TABLE IF EXISTS `Neurons`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Neurons` (
  `NeuronID` mediumint(8) unsigned NOT NULL auto_increment,
  `NeuronGroupID` smallint(5) unsigned NOT NULL,
  `X` smallint(6) NOT NULL,
  `Y` smallint(6) NOT NULL,
  `Z` smallint(6) NOT NULL,
  PRIMARY KEY  (`NeuronID`),
  UNIQUE KEY `UniquePositions_CON` (`NeuronGroupID`,`X`,`Y`,`Z`),
  KEY `Positions` (`X`,`Y`,`Z`),
  KEY `NeuronGroupIDIndex` (`NeuronGroupID`),
  CONSTRAINT `Neurons_ibfk_1` FOREIGN KEY (`NeuronGroupID`) REFERENCES `NeuronGroups` (`NeuronGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Neurons`
--

LOCK TABLES `Neurons` WRITE;
/*!40000 ALTER TABLE `Neurons` DISABLE KEYS */;
/*!40000 ALTER TABLE `Neurons` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `STDP1NeuronParameters`
--

DROP TABLE IF EXISTS `STDP1NeuronParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `STDP1NeuronParameters` (
  `NeuronGroupID` smallint(5) unsigned NOT NULL,
  `CalciumIncreaseAmnt_val` double default '1',
  `CalciumIncreaseAmnt_desc` char(100) default 'Calcium increase amount',
  `CalciumDecayRate_val` double default '60',
  `CalciumDecayRate_desc` char(100) default 'Calcium decay rate',
  `RefractoryPeriod_val` double default '1',
  `RefractoryPeriod_desc` char(100) default 'Refractory period (ms)',
  `MembraneTimeConstant_val` double default '3',
  `MembraneTimeConstant_desc` char(100) default 'Membrane time constant (ms)',
  `RefractoryParamM_val` double default '0.8',
  `RefractoryParamM_desc` char(100) default 'Refractory parameter M',
  `RefractoryParamN_val` double default '3',
  `RefractoryParamN_desc` char(100) default 'Refractory parameter N',
  `Threshold_val` double default '1',
  `Threshold_desc` char(100) default 'Threshold',
  `MinPostsynapticPotential_val` double default '-5',
  `MinPostsynapticPotential_desc` char(100) default 'Min postsynaptic potential',
  `Learning_val` tinyint(1) default '0',
  `Learning_desc` char(100) default 'Learning',
  PRIMARY KEY  (`NeuronGroupID`),
  CONSTRAINT `STDP1NeuronParameters_ibfk_1` FOREIGN KEY (`NeuronGroupID`) REFERENCES `NeuronGroups` (`NeuronGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `STDP1NeuronParameters`
--

LOCK TABLES `STDP1NeuronParameters` WRITE;
/*!40000 ALTER TABLE `STDP1NeuronParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `STDP1NeuronParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `STDP1SynapseParameters`
--

DROP TABLE IF EXISTS `STDP1SynapseParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `STDP1SynapseParameters` (
  `ConnectionGroupID` smallint(5) unsigned NOT NULL,
  `Learning_val` tinyint(1) default '0',
  `Learning_desc` char(100) default 'Learning',
  `Disable_val` tinyint(1) default '0',
  `Disable_desc` char(100) default 'Disable',
  `CalciumThreshUpLow_val` double default '30',
  `CalciumThreshUpLow_desc` char(100) default 'Calcium threshold up low',
  `CalciumThreshUpHigh_val` double default '120',
  `CalciumThreshUpHigh_desc` char(100) default 'Calcium threshold up high',
  `CalciumThreshDownLow_val` double default '30',
  `CalciumThreshDownLow_desc` char(100) default 'Calcium threshold down low',
  `CalciumThreshDownHigh_val` double default '40',
  `CalciumThreshDownHigh_desc` char(100) default 'Calcium threshold down high',
  `WeightChangeThreshold_val` double default '0.8',
  `WeightChangeThreshold_desc` char(100) default 'Weight change threshold',
  `WeightIncreaseAmnt_val` double default '0.1',
  `WeightIncreaseAmnt_desc` char(100) default 'Weight increase amount',
  `WeightDecreaseAmnt_val` double default '0.1',
  `WeightDecreaseAmnt_desc` char(100) default 'Weight decrease amount',
  `DriftThreshold_val` double default '0.5',
  `DriftThreshold_desc` char(100) default 'Drift threshold',
  `PositiveDrift_val` double default '0',
  `PositiveDrift_desc` char(100) default 'Positive drift',
  `NegativeDrift_val` double default '0',
  `NegativeDrift_desc` char(100) default 'Negative drift',
  `MaximumDrift_val` double default '1',
  `MaximumDrift_desc` char(100) default 'Maximum drift',
  `MinimumDrift_val` double default '0',
  `MinimumDrift_desc` char(100) default 'Minimum drift',
  PRIMARY KEY  (`ConnectionGroupID`),
  CONSTRAINT `STDP1SynapseParameters_ibfk_1` FOREIGN KEY (`ConnectionGroupID`) REFERENCES `ConnectionGroups` (`ConnectionGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `STDP1SynapseParameters`
--

LOCK TABLES `STDP1SynapseParameters` WRITE;
/*!40000 ALTER TABLE `STDP1SynapseParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `STDP1SynapseParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SynapseTypes`
--

DROP TABLE IF EXISTS `SynapseTypes`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `SynapseTypes` (
  `SynapseTypeID` smallint(5) unsigned NOT NULL,
  `Description` char(250) NOT NULL,
  `ParameterTableName` char(100) NOT NULL,
  `ClassLibrary` char(100) NOT NULL,
  PRIMARY KEY  (`SynapseTypeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `SynapseTypes`
--

LOCK TABLES `SynapseTypes` WRITE;
/*!40000 ALTER TABLE `SynapseTypes` DISABLE KEYS */;
INSERT INTO `SynapseTypes` VALUES (1,'STDP1 Synapse','STDP1SynapseParameters','libstdp1synapse.so'),(2,'Weightless Synapse','WeigthlessSynapseParameters','undefined.so');
/*!40000 ALTER TABLE `SynapseTypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessConnections`
--

DROP TABLE IF EXISTS `WeightlessConnections`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessConnections` (
  `ConnectionID` bigint(20) unsigned NOT NULL,
  `PatternIndex` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY  (`ConnectionID`),
  CONSTRAINT `WeightlessConnections_ibfk_1` FOREIGN KEY (`ConnectionID`) REFERENCES `Connections` (`ConnectionID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessConnections`
--

LOCK TABLES `WeightlessConnections` WRITE;
/*!40000 ALTER TABLE `WeightlessConnections` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessConnections` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessNeuronParameters`
--

DROP TABLE IF EXISTS `WeightlessNeuronParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessNeuronParameters` (
  `NeuronGroupID` smallint(5) unsigned NOT NULL,
  `Generalization_val` double default '1',
  `Generalization_desc` char(100) default 'Generalization',
  PRIMARY KEY  (`NeuronGroupID`),
  CONSTRAINT `WeightlessNeuronParameters_ibfk_1` FOREIGN KEY (`NeuronGroupID`) REFERENCES `NeuronGroups` (`NeuronGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessNeuronParameters`
--

LOCK TABLES `WeightlessNeuronParameters` WRITE;
/*!40000 ALTER TABLE `WeightlessNeuronParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessNeuronParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessNeuronTrainingPatterns`
--

DROP TABLE IF EXISTS `WeightlessNeuronTrainingPatterns`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessNeuronTrainingPatterns` (
  `PatternID` mediumint(8) unsigned NOT NULL auto_increment,
  `NeuronID` mediumint(8) unsigned NOT NULL,
  `Pattern` mediumblob NOT NULL,
  `Output` tinyint(1) NOT NULL,
  PRIMARY KEY  (`PatternID`),
  KEY `NeuronIDIndex` (`NeuronID`),
  CONSTRAINT `WeightlessNeuronTrainingPatterns_ibfk_1` FOREIGN KEY (`NeuronID`) REFERENCES `Neurons` (`NeuronID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessNeuronTrainingPatterns`
--

LOCK TABLES `WeightlessNeuronTrainingPatterns` WRITE;
/*!40000 ALTER TABLE `WeightlessNeuronTrainingPatterns` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessNeuronTrainingPatterns` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessSynapseParameters`
--

DROP TABLE IF EXISTS `WeightlessSynapseParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessSynapseParameters` (
  `ConnectionGroupID` smallint(5) unsigned NOT NULL,
  `Test_val` tinyint(1) default '0',
  `Test_desc` char(100) default 'Test parameter',
  PRIMARY KEY  (`ConnectionGroupID`),
  CONSTRAINT `WeightlessSynapseParameters_ibfk_1` FOREIGN KEY (`ConnectionGroupID`) REFERENCES `ConnectionGroups` (`ConnectionGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessSynapseParameters`
--

LOCK TABLES `WeightlessSynapseParameters` WRITE;
/*!40000 ALTER TABLE `WeightlessSynapseParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessSynapseParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Current Database: `SpikeStreamNetworkTest`
--

/*!40000 DROP DATABASE IF EXISTS `SpikeStreamNetworkTest`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `SpikeStreamNetworkTest` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `SpikeStreamNetworkTest`;

--
-- Table structure for table `ConnectionGroups`
--

DROP TABLE IF EXISTS `ConnectionGroups`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ConnectionGroups` (
  `ConnectionGroupID` smallint(5) unsigned NOT NULL auto_increment,
  `NetworkID` smallint(6) NOT NULL,
  `Description` char(250) NOT NULL,
  `FromNeuronGroupID` smallint(5) unsigned NOT NULL,
  `ToNeuronGroupID` smallint(5) unsigned NOT NULL,
  `SynapseTypeID` smallint(5) unsigned NOT NULL,
  `Parameters` text,
  PRIMARY KEY  (`ConnectionGroupID`),
  KEY `ConnectionGroupIDIndex` (`ConnectionGroupID`),
  KEY `NetworkID_FK` (`NetworkID`),
  KEY `SynapseTypeID_FK` (`SynapseTypeID`),
  CONSTRAINT `ConnectionGroups_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `Networks` (`NetworkID`) ON DELETE CASCADE,
  CONSTRAINT `ConnectionGroups_ibfk_2` FOREIGN KEY (`SynapseTypeID`) REFERENCES `SynapseTypes` (`SynapseTypeID`) ON DELETE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `ConnectionGroups`
--

LOCK TABLES `ConnectionGroups` WRITE;
/*!40000 ALTER TABLE `ConnectionGroups` DISABLE KEYS */;
/*!40000 ALTER TABLE `ConnectionGroups` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Connections`
--

DROP TABLE IF EXISTS `Connections`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Connections` (
  `ConnectionID` bigint(20) unsigned NOT NULL auto_increment,
  `FromNeuronID` mediumint(8) unsigned NOT NULL,
  `ToNeuronID` mediumint(8) unsigned NOT NULL,
  `ConnectionGroupID` smallint(5) unsigned NOT NULL,
  `Delay` float unsigned NOT NULL,
  `Weight` float NOT NULL,
  `TempWeight` float default '0',
  PRIMARY KEY  (`ConnectionID`),
  KEY `FromNeuronIndex` (`FromNeuronID`,`ToNeuronID`),
  KEY `ToNeuronIndex` (`ToNeuronID`),
  KEY `ConnectionGroupID` (`ConnectionGroupID`),
  CONSTRAINT `Connections_ibfk_1` FOREIGN KEY (`ConnectionGroupID`) REFERENCES `ConnectionGroups` (`ConnectionGroupID`) ON DELETE CASCADE,
  CONSTRAINT `Connections_ibfk_2` FOREIGN KEY (`FromNeuronID`) REFERENCES `Neurons` (`NeuronID`) ON DELETE CASCADE,
  CONSTRAINT `Connections_ibfk_3` FOREIGN KEY (`ToNeuronID`) REFERENCES `Neurons` (`NeuronID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Connections`
--

LOCK TABLES `Connections` WRITE;
/*!40000 ALTER TABLE `Connections` DISABLE KEYS */;
/*!40000 ALTER TABLE `Connections` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Networks`
--

DROP TABLE IF EXISTS `Networks`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Networks` (
  `NetworkID` smallint(6) NOT NULL auto_increment,
  `Name` char(250) NOT NULL default 'Untitled',
  `Description` char(250) NOT NULL default 'Untitled',
  PRIMARY KEY  (`NetworkID`),
  KEY `NetworkIDIndex` (`NetworkID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Networks`
--

LOCK TABLES `Networks` WRITE;
/*!40000 ALTER TABLE `Networks` DISABLE KEYS */;
/*!40000 ALTER TABLE `Networks` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `NeuronGroups`
--

DROP TABLE IF EXISTS `NeuronGroups`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `NeuronGroups` (
  `NeuronGroupID` smallint(5) unsigned NOT NULL auto_increment,
  `NetworkID` smallint(6) NOT NULL,
  `Name` char(250) NOT NULL default 'Untitled',
  `Description` char(250) NOT NULL default 'No description',
  `Parameters` text,
  `NeuronTypeID` smallint(5) unsigned NOT NULL,
  PRIMARY KEY  (`NeuronGroupID`),
  KEY `NeuronGroupIDIndex` (`NeuronGroupID`),
  KEY `NetworkIDIndex` (`NetworkID`),
  KEY `NeuronTypeID_FK` (`NeuronTypeID`),
  CONSTRAINT `NeuronGroups_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `Networks` (`NetworkID`) ON DELETE CASCADE,
  CONSTRAINT `NeuronGroups_ibfk_2` FOREIGN KEY (`NeuronTypeID`) REFERENCES `NeuronTypes` (`NeuronTypeID`) ON DELETE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `NeuronGroups`
--

LOCK TABLES `NeuronGroups` WRITE;
/*!40000 ALTER TABLE `NeuronGroups` DISABLE KEYS */;
/*!40000 ALTER TABLE `NeuronGroups` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `NeuronTypes`
--

DROP TABLE IF EXISTS `NeuronTypes`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `NeuronTypes` (
  `NeuronTypeID` smallint(5) unsigned NOT NULL,
  `Description` char(250) NOT NULL,
  `ParameterTableName` char(100) NOT NULL,
  `ClassLibrary` char(100) NOT NULL,
  PRIMARY KEY  (`NeuronTypeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `NeuronTypes`
--

LOCK TABLES `NeuronTypes` WRITE;
/*!40000 ALTER TABLE `NeuronTypes` DISABLE KEYS */;
INSERT INTO `NeuronTypes` VALUES (1,'STDP1 Neuron','STDP1NeuronParameters','libstdp1neuron.so'),(2,'Weightless Neuron','WeigthlessNeuronParameters','undefined.so');
/*!40000 ALTER TABLE `NeuronTypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Neurons`
--

DROP TABLE IF EXISTS `Neurons`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Neurons` (
  `NeuronID` mediumint(8) unsigned NOT NULL auto_increment,
  `NeuronGroupID` smallint(5) unsigned NOT NULL,
  `X` smallint(6) NOT NULL,
  `Y` smallint(6) NOT NULL,
  `Z` smallint(6) NOT NULL,
  PRIMARY KEY  (`NeuronID`),
  UNIQUE KEY `UniquePositions_CON` (`NeuronGroupID`,`X`,`Y`,`Z`),
  KEY `Positions` (`X`,`Y`,`Z`),
  KEY `NeuronGroupIDIndex` (`NeuronGroupID`),
  CONSTRAINT `Neurons_ibfk_1` FOREIGN KEY (`NeuronGroupID`) REFERENCES `NeuronGroups` (`NeuronGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Neurons`
--

LOCK TABLES `Neurons` WRITE;
/*!40000 ALTER TABLE `Neurons` DISABLE KEYS */;
/*!40000 ALTER TABLE `Neurons` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `STDP1NeuronParameters`
--

DROP TABLE IF EXISTS `STDP1NeuronParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `STDP1NeuronParameters` (
  `NeuronGroupID` smallint(5) unsigned NOT NULL,
  `CalciumIncreaseAmnt_val` double default '1',
  `CalciumIncreaseAmnt_desc` char(100) default 'Calcium increase amount',
  `CalciumDecayRate_val` double default '60',
  `CalciumDecayRate_desc` char(100) default 'Calcium decay rate',
  `RefractoryPeriod_val` double default '1',
  `RefractoryPeriod_desc` char(100) default 'Refractory period (ms)',
  `MembraneTimeConstant_val` double default '3',
  `MembraneTimeConstant_desc` char(100) default 'Membrane time constant (ms)',
  `RefractoryParamM_val` double default '0.8',
  `RefractoryParamM_desc` char(100) default 'Refractory parameter M',
  `RefractoryParamN_val` double default '3',
  `RefractoryParamN_desc` char(100) default 'Refractory parameter N',
  `Threshold_val` double default '1',
  `Threshold_desc` char(100) default 'Threshold',
  `MinPostsynapticPotential_val` double default '-5',
  `MinPostsynapticPotential_desc` char(100) default 'Min postsynaptic potential',
  `Learning_val` tinyint(1) default '0',
  `Learning_desc` char(100) default 'Learning',
  PRIMARY KEY  (`NeuronGroupID`),
  CONSTRAINT `STDP1NeuronParameters_ibfk_1` FOREIGN KEY (`NeuronGroupID`) REFERENCES `NeuronGroups` (`NeuronGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `STDP1NeuronParameters`
--

LOCK TABLES `STDP1NeuronParameters` WRITE;
/*!40000 ALTER TABLE `STDP1NeuronParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `STDP1NeuronParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `STDP1SynapseParameters`
--

DROP TABLE IF EXISTS `STDP1SynapseParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `STDP1SynapseParameters` (
  `ConnectionGroupID` smallint(5) unsigned NOT NULL,
  `Learning_val` tinyint(1) default '0',
  `Learning_desc` char(100) default 'Learning',
  `Disable_val` tinyint(1) default '0',
  `Disable_desc` char(100) default 'Disable',
  `CalciumThreshUpLow_val` double default '30',
  `CalciumThreshUpLow_desc` char(100) default 'Calcium threshold up low',
  `CalciumThreshUpHigh_val` double default '120',
  `CalciumThreshUpHigh_desc` char(100) default 'Calcium threshold up high',
  `CalciumThreshDownLow_val` double default '30',
  `CalciumThreshDownLow_desc` char(100) default 'Calcium threshold down low',
  `CalciumThreshDownHigh_val` double default '40',
  `CalciumThreshDownHigh_desc` char(100) default 'Calcium threshold down high',
  `WeightChangeThreshold_val` double default '0.8',
  `WeightChangeThreshold_desc` char(100) default 'Weight change threshold',
  `WeightIncreaseAmnt_val` double default '0.1',
  `WeightIncreaseAmnt_desc` char(100) default 'Weight increase amount',
  `WeightDecreaseAmnt_val` double default '0.1',
  `WeightDecreaseAmnt_desc` char(100) default 'Weight decrease amount',
  `DriftThreshold_val` double default '0.5',
  `DriftThreshold_desc` char(100) default 'Drift threshold',
  `PositiveDrift_val` double default '0',
  `PositiveDrift_desc` char(100) default 'Positive drift',
  `NegativeDrift_val` double default '0',
  `NegativeDrift_desc` char(100) default 'Negative drift',
  `MaximumDrift_val` double default '1',
  `MaximumDrift_desc` char(100) default 'Maximum drift',
  `MinimumDrift_val` double default '0',
  `MinimumDrift_desc` char(100) default 'Minimum drift',
  PRIMARY KEY  (`ConnectionGroupID`),
  CONSTRAINT `STDP1SynapseParameters_ibfk_1` FOREIGN KEY (`ConnectionGroupID`) REFERENCES `ConnectionGroups` (`ConnectionGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `STDP1SynapseParameters`
--

LOCK TABLES `STDP1SynapseParameters` WRITE;
/*!40000 ALTER TABLE `STDP1SynapseParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `STDP1SynapseParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `SynapseTypes`
--

DROP TABLE IF EXISTS `SynapseTypes`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `SynapseTypes` (
  `SynapseTypeID` smallint(5) unsigned NOT NULL,
  `Description` char(250) NOT NULL,
  `ParameterTableName` char(100) NOT NULL,
  `ClassLibrary` char(100) NOT NULL,
  PRIMARY KEY  (`SynapseTypeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `SynapseTypes`
--

LOCK TABLES `SynapseTypes` WRITE;
/*!40000 ALTER TABLE `SynapseTypes` DISABLE KEYS */;
INSERT INTO `SynapseTypes` VALUES (1,'STDP1 Synapse','STDP1SynapseParameters','libstdp1synapse.so'),(2,'Weightless Synapse','WeigthlessSynapseParameters','undefined.so');
/*!40000 ALTER TABLE `SynapseTypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessConnections`
--

DROP TABLE IF EXISTS `WeightlessConnections`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessConnections` (
  `ConnectionID` bigint(20) unsigned NOT NULL,
  `PatternIndex` mediumint(8) unsigned NOT NULL,
  PRIMARY KEY  (`ConnectionID`),
  CONSTRAINT `WeightlessConnections_ibfk_1` FOREIGN KEY (`ConnectionID`) REFERENCES `Connections` (`ConnectionID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessConnections`
--

LOCK TABLES `WeightlessConnections` WRITE;
/*!40000 ALTER TABLE `WeightlessConnections` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessConnections` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessNeuronParameters`
--

DROP TABLE IF EXISTS `WeightlessNeuronParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessNeuronParameters` (
  `NeuronGroupID` smallint(5) unsigned NOT NULL,
  `Generalization_val` double default '1',
  `Generalization_desc` char(100) default 'Generalization',
  PRIMARY KEY  (`NeuronGroupID`),
  CONSTRAINT `WeightlessNeuronParameters_ibfk_1` FOREIGN KEY (`NeuronGroupID`) REFERENCES `NeuronGroups` (`NeuronGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessNeuronParameters`
--

LOCK TABLES `WeightlessNeuronParameters` WRITE;
/*!40000 ALTER TABLE `WeightlessNeuronParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessNeuronParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessNeuronTrainingPatterns`
--

DROP TABLE IF EXISTS `WeightlessNeuronTrainingPatterns`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessNeuronTrainingPatterns` (
  `PatternID` mediumint(8) unsigned NOT NULL auto_increment,
  `NeuronID` mediumint(8) unsigned NOT NULL,
  `Pattern` mediumblob NOT NULL,
  `Output` tinyint(1) NOT NULL,
  PRIMARY KEY  (`PatternID`),
  KEY `NeuronIDIndex` (`NeuronID`),
  CONSTRAINT `WeightlessNeuronTrainingPatterns_ibfk_1` FOREIGN KEY (`NeuronID`) REFERENCES `Neurons` (`NeuronID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessNeuronTrainingPatterns`
--

LOCK TABLES `WeightlessNeuronTrainingPatterns` WRITE;
/*!40000 ALTER TABLE `WeightlessNeuronTrainingPatterns` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessNeuronTrainingPatterns` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `WeightlessSynapseParameters`
--

DROP TABLE IF EXISTS `WeightlessSynapseParameters`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `WeightlessSynapseParameters` (
  `ConnectionGroupID` smallint(5) unsigned NOT NULL,
  `Test_val` tinyint(1) default '0',
  `Test_desc` char(100) default 'Test parameter',
  PRIMARY KEY  (`ConnectionGroupID`),
  CONSTRAINT `WeightlessSynapseParameters_ibfk_1` FOREIGN KEY (`ConnectionGroupID`) REFERENCES `ConnectionGroups` (`ConnectionGroupID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `WeightlessSynapseParameters`
--

LOCK TABLES `WeightlessSynapseParameters` WRITE;
/*!40000 ALTER TABLE `WeightlessSynapseParameters` DISABLE KEYS */;
/*!40000 ALTER TABLE `WeightlessSynapseParameters` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Current Database: `SpikeStreamArchive`
--

/*!40000 DROP DATABASE IF EXISTS `SpikeStreamArchive`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `SpikeStreamArchive` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `SpikeStreamArchive`;

--
-- Table structure for table `ArchiveData`
--

DROP TABLE IF EXISTS `ArchiveData`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ArchiveData` (
  `ArchiveID` mediumint(8) unsigned NOT NULL,
  `TimeStep` int(10) unsigned NOT NULL,
  `FiringNeurons` longtext NOT NULL,
  PRIMARY KEY  (`ArchiveID`,`TimeStep`),
  CONSTRAINT `ArchiveData_ibfk_1` FOREIGN KEY (`ArchiveID`) REFERENCES `Archives` (`ArchiveID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `ArchiveData`
--

LOCK TABLES `ArchiveData` WRITE;
/*!40000 ALTER TABLE `ArchiveData` DISABLE KEYS */;
/*!40000 ALTER TABLE `ArchiveData` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Archives`
--

DROP TABLE IF EXISTS `Archives`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Archives` (
  `ArchiveID` mediumint(8) unsigned NOT NULL auto_increment,
  `StartTime` bigint(20) NOT NULL,
  `NetworkID` smallint(6) NOT NULL,
  `Description` char(100) default NULL,
  PRIMARY KEY  (`ArchiveID`),
  KEY `NetworkID_FK` (`NetworkID`),
  CONSTRAINT `Archives_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `SpikeStreamNetwork`.`Networks` (`NetworkID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Archives`
--

LOCK TABLES `Archives` WRITE;
/*!40000 ALTER TABLE `Archives` DISABLE KEYS */;
/*!40000 ALTER TABLE `Archives` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Current Database: `SpikeStreamArchiveTest`
--

/*!40000 DROP DATABASE IF EXISTS `SpikeStreamArchiveTest`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `SpikeStreamArchiveTest` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `SpikeStreamArchiveTest`;

--
-- Table structure for table `ArchiveData`
--

DROP TABLE IF EXISTS `ArchiveData`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `ArchiveData` (
  `ArchiveID` mediumint(8) unsigned NOT NULL,
  `TimeStep` int(10) unsigned NOT NULL,
  `FiringNeurons` longtext NOT NULL,
  PRIMARY KEY  (`ArchiveID`,`TimeStep`),
  CONSTRAINT `ArchiveData_ibfk_1` FOREIGN KEY (`ArchiveID`) REFERENCES `Archives` (`ArchiveID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `ArchiveData`
--

LOCK TABLES `ArchiveData` WRITE;
/*!40000 ALTER TABLE `ArchiveData` DISABLE KEYS */;
/*!40000 ALTER TABLE `ArchiveData` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Archives`
--

DROP TABLE IF EXISTS `Archives`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Archives` (
  `ArchiveID` mediumint(8) unsigned NOT NULL auto_increment,
  `StartTime` bigint(20) NOT NULL,
  `NetworkID` smallint(6) NOT NULL,
  `Description` char(100) default NULL,
  PRIMARY KEY  (`ArchiveID`),
  KEY `NetworkID_FK` (`NetworkID`),
  CONSTRAINT `Archives_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `SpikeStreamNetworkTest`.`Networks` (`NetworkID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Archives`
--

LOCK TABLES `Archives` WRITE;
/*!40000 ALTER TABLE `Archives` DISABLE KEYS */;
/*!40000 ALTER TABLE `Archives` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Current Database: `SpikeStreamAnalysis`
--

/*!40000 DROP DATABASE IF EXISTS `SpikeStreamAnalysis`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `SpikeStreamAnalysis` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `SpikeStreamAnalysis`;

--
-- Table structure for table `Analyses`
--

DROP TABLE IF EXISTS `Analyses`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Analyses` (
  `AnalysisID` smallint(5) unsigned NOT NULL auto_increment,
  `NetworkID` smallint(6) NOT NULL,
  `ArchiveID` mediumint(8) unsigned NOT NULL,
  `StartTime` bigint(20) NOT NULL,
  `Description` char(255) NOT NULL,
  `Parameters` mediumtext NOT NULL,
  `AnalysisTypeID` smallint(6) NOT NULL,
  PRIMARY KEY  (`AnalysisID`),
  KEY `NetworkID_FK` (`NetworkID`),
  KEY `ArchiveID_FK` (`ArchiveID`),
  KEY `AnalysisTypeID_FK` (`AnalysisTypeID`),
  CONSTRAINT `Analyses_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `SpikeStreamNetwork`.`Networks` (`NetworkID`) ON DELETE CASCADE,
  CONSTRAINT `Analyses_ibfk_2` FOREIGN KEY (`ArchiveID`) REFERENCES `SpikeStreamArchive`.`Archives` (`ArchiveID`) ON DELETE CASCADE,
  CONSTRAINT `Analyses_ibfk_3` FOREIGN KEY (`AnalysisTypeID`) REFERENCES `AnalysisTypes` (`AnalysisTypeID`) ON DELETE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Analyses`
--

LOCK TABLES `Analyses` WRITE;
/*!40000 ALTER TABLE `Analyses` DISABLE KEYS */;
/*!40000 ALTER TABLE `Analyses` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `AnalysisTypes`
--

DROP TABLE IF EXISTS `AnalysisTypes`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `AnalysisTypes` (
  `AnalysisTypeID` smallint(6) NOT NULL,
  `Description` char(255) NOT NULL,
  PRIMARY KEY  (`AnalysisTypeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `AnalysisTypes`
--

LOCK TABLES `AnalysisTypes` WRITE;
/*!40000 ALTER TABLE `AnalysisTypes` DISABLE KEYS */;
INSERT INTO `AnalysisTypes` VALUES (1,'State-based Phi Analysis');
/*!40000 ALTER TABLE `AnalysisTypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `StateBasedPhiData`
--

DROP TABLE IF EXISTS `StateBasedPhiData`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `StateBasedPhiData` (
  `ComplexID` mediumint(8) unsigned NOT NULL auto_increment,
  `AnalysisID` smallint(5) unsigned NOT NULL,
  `TimeStep` int(10) unsigned NOT NULL,
  `Phi` double NOT NULL,
  `Neurons` longtext NOT NULL,
  PRIMARY KEY  (`ComplexID`),
  KEY `AnalysisIDIndex` (`AnalysisID`),
  CONSTRAINT `StateBasedPhiData_ibfk_1` FOREIGN KEY (`AnalysisID`) REFERENCES `Analyses` (`AnalysisID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `StateBasedPhiData`
--

LOCK TABLES `StateBasedPhiData` WRITE;
/*!40000 ALTER TABLE `StateBasedPhiData` DISABLE KEYS */;
/*!40000 ALTER TABLE `StateBasedPhiData` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Current Database: `SpikeStreamAnalysisTest`
--

/*!40000 DROP DATABASE IF EXISTS `SpikeStreamAnalysisTest`*/;

CREATE DATABASE /*!32312 IF NOT EXISTS*/ `SpikeStreamAnalysisTest` /*!40100 DEFAULT CHARACTER SET latin1 */;

USE `SpikeStreamAnalysisTest`;

--
-- Table structure for table `Analyses`
--

DROP TABLE IF EXISTS `Analyses`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `Analyses` (
  `AnalysisID` smallint(5) unsigned NOT NULL auto_increment,
  `NetworkID` smallint(6) NOT NULL,
  `ArchiveID` mediumint(8) unsigned NOT NULL,
  `StartTime` bigint(20) NOT NULL,
  `Description` char(255) NOT NULL,
  `Parameters` mediumtext NOT NULL,
  `AnalysisTypeID` smallint(6) NOT NULL,
  PRIMARY KEY  (`AnalysisID`),
  KEY `NetworkID_FK` (`NetworkID`),
  KEY `ArchiveID_FK` (`ArchiveID`),
  KEY `AnalysisTypeID_FK` (`AnalysisTypeID`),
  CONSTRAINT `Analyses_ibfk_1` FOREIGN KEY (`NetworkID`) REFERENCES `SpikeStreamNetworkTest`.`Networks` (`NetworkID`) ON DELETE CASCADE,
  CONSTRAINT `Analyses_ibfk_2` FOREIGN KEY (`ArchiveID`) REFERENCES `SpikeStreamArchiveTest`.`Archives` (`ArchiveID`) ON DELETE CASCADE,
  CONSTRAINT `Analyses_ibfk_3` FOREIGN KEY (`AnalysisTypeID`) REFERENCES `AnalysisTypes` (`AnalysisTypeID`) ON DELETE NO ACTION
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `Analyses`
--

LOCK TABLES `Analyses` WRITE;
/*!40000 ALTER TABLE `Analyses` DISABLE KEYS */;
/*!40000 ALTER TABLE `Analyses` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `AnalysisTypes`
--

DROP TABLE IF EXISTS `AnalysisTypes`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `AnalysisTypes` (
  `AnalysisTypeID` smallint(6) NOT NULL,
  `Description` char(255) NOT NULL,
  PRIMARY KEY  (`AnalysisTypeID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `AnalysisTypes`
--

LOCK TABLES `AnalysisTypes` WRITE;
/*!40000 ALTER TABLE `AnalysisTypes` DISABLE KEYS */;
INSERT INTO `AnalysisTypes` VALUES (1,'State-based Phi Analysis');
/*!40000 ALTER TABLE `AnalysisTypes` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `StateBasedPhiData`
--

DROP TABLE IF EXISTS `StateBasedPhiData`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `StateBasedPhiData` (
  `ComplexID` mediumint(8) unsigned NOT NULL auto_increment,
  `AnalysisID` smallint(5) unsigned NOT NULL,
  `TimeStep` int(10) unsigned NOT NULL,
  `Phi` double NOT NULL,
  `Neurons` longtext NOT NULL,
  PRIMARY KEY  (`ComplexID`),
  KEY `AnalysisIDIndex` (`AnalysisID`),
  CONSTRAINT `StateBasedPhiData_ibfk_1` FOREIGN KEY (`AnalysisID`) REFERENCES `Analyses` (`AnalysisID`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `StateBasedPhiData`
--

LOCK TABLES `StateBasedPhiData` WRITE;
/*!40000 ALTER TABLE `StateBasedPhiData` DISABLE KEYS */;
/*!40000 ALTER TABLE `StateBasedPhiData` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2010-01-04 20:13:34
