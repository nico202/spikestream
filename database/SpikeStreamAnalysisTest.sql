/*
	IMPORTANT NOTE: COMMENTS WITH SEMI-COLONS BREAK THE DATABASE CONFIGURATION TOOL!
*/ 
 
/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;


/* Create and use the database */
DROP DATABASE IF EXISTS SpikeStreamAnalysisTest;
CREATE DATABASE SpikeStreamAnalysisTest;
USE SpikeStreamAnalysisTest;


/* PhiAnalysisDescriptions
	Holds information about each phi analysis saved in the database. */
CREATE TABLE Analyses (
	AnalysisID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	NetworkID SMALLINT NOT NULL,/* The ID of the network that is being analyzed */
	ArchiveID MEDIUMINT UNSIGNED NOT NULL, /* The ID of the archive that is being analyzed */
	StartTime BIGINT NOT NULL, /*When the analysis started. */
	Description CHAR(255) NOT NULL,
	Parameters MEDIUMTEXT NOT NULL,
	AnalysisTypeID SMALLINT NOT NULL,

	PRIMARY KEY (AnalysisID),
	FOREIGN KEY NetworkID_FK(NetworkID) REFERENCES SpikeStreamNetworkTest.Networks(NetworkID) ON DELETE CASCADE,
	FOREIGN KEY ArchiveID_FK(ArchiveID) REFERENCES SpikeStreamArchiveTest.Archives(ArchiveID) ON DELETE CASCADE,
	FOREIGN KEY AnalysisTypeID_FK(AnalysisTypeID) REFERENCES AnalysisTypes(AnalysisTypeID) ON DELETE NO ACTION
)
ENGINE=InnoDB;

/* The different types of analysis */
CREATE TABLE AnalysisTypes (
	AnalysisTypeID SMALLINT NOT NULL,
	Description CHAR(255) NOT NULL,

	PRIMARY KEY (AnalysisTypeID)
)
ENGINE=InnoDB;

INSERT INTO AnalysisTypes (AnalysisTypeID, Description) VALUES (1, "State-based Phi Analysis");


/* PhiAnalysisData
	The data associated with a phi analysis description. */
CREATE TABLE StateBasedPhiData (
	ComplexID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
	AnalysisID SMALLINT UNSIGNED NOT NULL,
	TimeStep INT UNSIGNED NOT NULL, /* The time step of the firing pattern. */
	Phi DOUBLE NOT NULL,
	Neurons LONGTEXT NOT NULL,

	PRIMARY KEY (ComplexID),
	INDEX AnalysisIDIndex(AnalysisID),
	FOREIGN KEY AnalysisID_FK(AnalysisID) REFERENCES Analyses(AnalysisID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* A cluster of neurons connected by lively connections.
	The Liveliness value should measure the total information integration of the cluster. */
CREATE TABLE ClusterLiveliness (
	ClusterID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
	AnalysisID SMALLINT UNSIGNED NOT NULL,
	TimeStep INT UNSIGNED NOT NULL, /* The time step of the firing pattern. */
	Liveliness DOUBLE NOT NULL,
	Neurons LONGTEXT NOT NULL,

	PRIMARY KEY (ClusterID),
	INDEX AnalysisIDIndex(AnalysisID),
	FOREIGN KEY AnalysisID_FK(AnalysisID) REFERENCES Analyses(AnalysisID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* The liveliness of each neuron. */
CREATE TABLE NeuronLiveliness (
	NeuronID MEDIUMINT UNSIGNED NOT NULL,
	AnalysisID SMALLINT UNSIGNED NOT NULL,
	TimeStep INT UNSIGNED NOT NULL, /* The time step of the firing pattern. */
	Liveliness DOUBLE NOT NULL,

	PRIMARY KEY (NeuronID, AnalysisID, TimeStep),
	INDEX AnalysisIDIndex(AnalysisID),
	FOREIGN KEY AnalysisID_FK(AnalysisID) REFERENCES Analyses(AnalysisID) ON DELETE CASCADE,
	FOREIGN KEY NeuronID_FK(NeuronID) REFERENCES SpikeStreamNetworkTest.Neurons(NeuronID) ON DELETE CASCADE
)
ENGINE=InnoDB;

/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 1;







