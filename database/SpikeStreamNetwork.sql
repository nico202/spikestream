/*------------------------  SpikeStreamNetwork ----------------------*/
/* Holds the neurons and connections. All of the databases are created using
	InnoDB so that we can use foreign keys. Performance looks roughly the same
	or better than MyISAM. 
	
	IMPORTANT NOTE: COMMENTS WITH SEMI-COLONS BREAK THE DATABASE CONFIGURATION TOOL!
*/             
/*-------------------------------------------------------------------------*/
 
/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;

/* Create and use SpikeStreamNetwork database */
DROP DATABASE IF EXISTS SpikeStreamNetwork;
CREATE DATABASE SpikeStreamNetwork;
USE SpikeStreamNetwork;

/* NeuralNetwork
	Allows a number of different networks to be held in the same database, 
	and allows networks to be archived.
*/
CREATE TABLE Networks (
    NetworkID SMALLINT NOT NULL AUTO_INCREMENT,
    Name CHAR(250) NOT NULL DEFAULT "Untitled",
    Description CHAR(250) NOT NULL DEFAULT "Untitled",
    PRIMARY KEY (NetworkID)
)
ENGINE=InnoDB;

/* NeuronGroups
    Neurons are gathered together into neuron groups that have common parameters and types
*/
CREATE TABLE NeuronGroups (
    NeuronGroupID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT, /* Primary key */
    NetworkID SMALLINT NOT NULL, /* Network that this group is part of */
    Name CHAR(250) NOT NULL DEFAULT "Untitled", /*Short name, for example, 'motor cortex' */
    Description CHAR(250) NOT NULL DEFAULT "No description", /* Description, for example, layer, etc. */
    Parameters TEXT, /* XML describing the parameters used to create the group */
    NeuronTypeID SMALLINT UNSIGNED NOT NULL, /*The type of neuron in the group. Used for parameters and class loading */

    PRIMARY KEY (NeuronGroupID),
	INDEX NetworkIDIndex(NetworkID),

    FOREIGN KEY NetworkID_FK(NetworkID) REFERENCES Networks(NetworkID) ON DELETE CASCADE,
    FOREIGN KEY NeuronTypeID_FK(NeuronTypeID) REFERENCES NeuronTypes(NeuronTypeID) ON DELETE NO ACTION
)
ENGINE=InnoDB;


/* Neurons
	Separate entry for each neuron including its position and the neuron 
	group that it belongs to. */
CREATE TABLE Neurons (
    NeuronID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
    NeuronGroupID SMALLINT UNSIGNED NOT NULL,

    /* Location of the neuron in 3D space */ 
    X FLOAT NOT NULL,
    Y FLOAT NOT NULL,
    Z FLOAT NOT NULL,

    PRIMARY KEY (NeuronID),
    INDEX NeuronGroupIDIndex(NeuronGroupID),

	CONSTRAINT UniquePositions_CON UNIQUE (NeuronGroupID, X, Y, Z),/* Neurons in the same group cannot occupy the same point in space. */
    FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* NeuronTypes
	Holds the differnt types of neuron, which are implemented by different
	dynamically loaded classes. Each entry in this table must match a corresponding
	table containing the parameters for this neuron type.
	AddSpikeStreamNeuronTypes.sql adds the individual types and associated parameter tables. */
CREATE TABLE NeuronTypes (
    NeuronTypeID SMALLINT UNSIGNED NOT NULL,
    Description CHAR(250) NOT NULL,
    ParameterTableName CHAR(100) NOT NULL,/* Table where parameters for this neuron type are stored. */
    ClassLibrary CHAR(100) NOT NULL, /* Dynamically loadable library used to simulate this neuron type */
    PRIMARY KEY (NeuronTypeID)
)
ENGINE=InnoDB;


/* ConnectionGroups
	Connections are held as groups with global characteristics. */
CREATE TABLE ConnectionGroups (
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	NetworkID SMALLINT NOT NULL,
	Description CHAR(250) NOT NULL, 
	FromNeuronGroupID SMALLINT UNSIGNED NOT NULL,
	ToNeuronGroupID SMALLINT UNSIGNED NOT NULL,
	SynapseTypeID SMALLINT UNSIGNED NOT NULL,
	Parameters TEXT,

	PRIMARY KEY (ConnectionGroupID),
	INDEX NetworkIDIndex(NetworkID),

    FOREIGN KEY NetworkID_FK(NetworkID) REFERENCES Networks(NetworkID) ON DELETE CASCADE,
    FOREIGN KEY FromNeuronGroupID_FK(FromNeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE,
    FOREIGN KEY ToNeuronGroupID_FK(ToNeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE,
    FOREIGN KEY SynapseTypeID_FK(SynapseTypeID) REFERENCES SynapseTypes(SynapseTypeID) ON DELETE NO ACTION
)
ENGINE=InnoDB;


/* Connections
	Each connection between two neurons has an entry in this table
	containing its delay, weight and the connection group it belongs to
	TempWeight is for storing weights temporarily for viewing the state of 
	the simulation without overwriting the current weight values. */
CREATE TABLE Connections (
	ConnectionID BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
	FromNeuronID MEDIUMINT UNSIGNED NOT NULL,
	ToNeuronID MEDIUMINT UNSIGNED NOT NULL,
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL,
	Delay FLOAT UNSIGNED NOT NULL,
	Weight FLOAT NOT NULL,

	PRIMARY KEY(ConnectionID),
	INDEX ConnectionGroupID (ConnectionGroupID),

    FOREIGN KEY ConnectionGroupID_FK(ConnectionGroupID) REFERENCES ConnectionGroups(ConnectionGroupID) ON DELETE CASCADE,
    FOREIGN KEY FromNeuronID_FK(FromNeuronID) REFERENCES Neurons(NeuronID) ON DELETE CASCADE,
    FOREIGN KEY ToNeuronID_FK(ToNeuronID) REFERENCES Neurons(NeuronID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* SynapseTypes
	Holds the differnt types of synapse, which are implemented by different
	dynamically loaded classes. Each entry in this table needs a corresponding
	table containing the parameters for this synapse type. 
	AddSynapseTypes.sql adds the indiviual types and their associated parameter tables.*/
CREATE TABLE SynapseTypes (
	SynapseTypeID SMALLINT UNSIGNED NOT NULL,
	Description CHAR(250) NOT NULL,
	ParameterTableName CHAR(100) NOT NULL,
	ClassLibrary CHAR(100) NOT NULL,
	PRIMARY KEY (SynapseTypeID)
)
ENGINE=InnoDB;


/* WeightlessNeuronLookupTables
	Each row stores an entry in a weightless neuron's lookup table.
	The connection represented by an entry in the pattern is stored in the weightless
	neuron connection table.
*/
CREATE TABLE WeightlessNeuronTrainingPatterns (
	PatternID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
	NeuronID MEDIUMINT UNSIGNED NOT NULL,
	Pattern MEDIUMBLOB NOT NULL,
	Output BOOLEAN NOT NULL,

	PRIMARY KEY (PatternID),
	INDEX NeuronIDIndex(NeuronID),

    FOREIGN KEY NeuronID_FK(NeuronID) REFERENCES Neurons(NeuronID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* WeightlessConnections
    Stores an index for each connection that refers to the connection between two neurons.
*/
CREATE TABLE WeightlessConnections (
	ConnectionID BIGINT UNSIGNED NOT NULL,
	PatternIndex MEDIUMINT UNSIGNED NOT NULL,

	PRIMARY KEY (ConnectionID),
    FOREIGN KEY ConnectionID_FK(ConnectionID) REFERENCES Connections(ConnectionID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Re-enable foreign key checks on the tables */
SET foreign_key_checks = 1;

