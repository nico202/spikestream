/*------------------------  SpikeStreamNeuralNetwork ----------------------*/
/* Holds the neurons and connections. All of the databases are created using
	InnoDB so that we can use foreign keys. Performance looks roughly the same
	or better than MyISAM. */             
/*-------------------------------------------------------------------------*/
 
/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;


/* Create and use the database. */
DROP DATABASE IF EXISTS SpikeStreamNetwork;
CREATE DATABASE SpikeStreamNetwork;
USE SpikeStreamNetwork;


/* NeuralNetwork
	Allows a number of different networks to be held in the same database, 
	and allows networks to be archived.
*/
CREATE TABLE NeuralNetworks (
    NeuralNetworkID SMALLINT NOT NULL AUTO_INCREMENT,
    Name CHAR(100) NOT NULL DEFAULT "Untitled",
    PRIMARY KEY (NeuralNetworkID),
    INDEX NeuralNetworkIDIndex(NeuralNetworkID)
)
ENGINE=InnoDB;


/* NeuronGroups
    Neurons are gathered together into neuron groups that have common parameters and types
*/
CREATE TABLE NeuronGroups (
    NeuronGroupID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT, /* Primary key */
    NeuralNetworkID SMALLINT NOT NULL, /* Network that this group is part of */
    Name CHAR(50) NOT NULL DEFAULT "Untitled", /*Short name, for example, 'motor cortex' */
    Description CHAR(100) NOT NULL DEFAULT "No description", /* Description, for example, layer, etc. */
    Parameters BLOB, /* XML describing the parameters used to create the group */
    NeuronTypeID SMALLINT UNSIGNED NOT NULL, /*The type of neuron in the group. Used for parameters and class loading */

    PRIMARY KEY (NeuronGroupID),
    INDEX NeuronGroupIDIndex(NeuronGroupID),
    INDEX NeuralNetworkIDIndex(NeuralNetworkID),/* To support the foreign key constraint */

    FOREIGN KEY NeuralNetworkID_FK(NeuralNetworkID) REFERENCES NeuralNetworks(NeuralNetworkID) ON DELETE CASCADE,
    FOREIGN KEY NeuronTypeID_FK(NeuronTypeID) REFERENCES NeuronTypes(NeuronTypeID) ON DELETE NO ACTION
)
ENGINE=InnoDB;


/* Need to change NeuronGroups so that auto increment starts with 10 
	This is to reserve numbers 0-9 for deleting and empty keys in 
	dense_hash_maps and other things. */
ALTER TABLE NeuronGroups AUTO_INCREMENT = 10;


/* Neurons
	Separate entry for each neuron including its position and the neuron 
	group that it belongs to. */
CREATE TABLE Neurons (
    NeuronID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
    NeuronGroupID SMALLINT UNSIGNED NOT NULL,

    /* Location of the neuron in 3D space */ 
    X SMALLINT NOT NULL,
    Y SMALLINT NOT NULL,
    Z SMALLINT NOT NULL,

    PRIMARY KEY (NeuronID),
    INDEX Positions (X, Y, Z),/* Used when creating connections. FIXME: CHECK MIN(X), MIN(Y) AND MIN(Z) ON THE INDEX. */
    INDEX NeuronGroupIDIndex(NeuronGroupID),

    FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Need to change Neurons so that auto increment starts with 10 
	This is to reserve numbers 0-9 for deleting and empty keys in 
	dense_hash_maps */
ALTER TABLE Neurons AUTO_INCREMENT = 10;


/* NeuronTypes
	Holds the differnt types of neuron, which are implemented by different
	dynamically loaded classes. Each entry in this table must match a corresponding
	table containing the parameters for this neuron type.
	AddSpikeStreamNeuronTypes.sql adds the individual types and associated parameter tables. */
CREATE TABLE NeuronTypes (
    NeuronTypeID SMALLINT UNSIGNED NOT NULL,
    Description CHAR(100) NOT NULL,
    ParameterTableName CHAR(100) NOT NULL,/* Table where parameters for this neuron type are stored. */
    ClassLibrary CHAR(100) NOT NULL, /* Dynamically loadable library used to simulate this neuron type */
    PRIMARY KEY (NeuronTypeID)
)
ENGINE=InnoDB;


/* ConnectionGroups
	Connections are held as groups with global characteristics. */
CREATE TABLE ConnectionGroups (
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	NeuralNetworkID SMALLINT NOT NULL,
	Description CHAR(100) NOT NULL, 
	FromNeuronGroupID SMALLINT UNSIGNED NOT NULL,
	ToNeuronGroupID SMALLINT UNSIGNED NOT NULL,
	SynapseTypeID SMALLINT UNSIGNED NOT NULL,
	Parameters BLOB,

	PRIMARY KEY (ConnectionGroupID),
	INDEX ConnectionGroupIDIndex(ConnectionGroupID),

    FOREIGN KEY NeuralNetworkID_FK(NeuralNetworkID) REFERENCES NeuralNetworks(NeuralNetworkID) ON DELETE CASCADE,
    FOREIGN KEY SynapseTypeID_FK(SynapseTypeID) REFERENCES SynapseTypes(SynapseTypeID) ON DELETE NO ACTION
)
ENGINE=InnoDB;


/* Need to change ConnectionGroups so that auto increment starts with 10 
	This is to reserve numbers 0-9 for deleting and empty keys in 
	dense_hash_maps and other things. */
ALTER TABLE ConnectionGroups AUTO_INCREMENT = 10;


/* Connections
	Each connection between two neurons has an entry in this table
	containing its delay, weight and the connection group it belongs to
	TempWeight is for storing weights temporarily for viewing the state of 
	the simulation without overwriting the current weight values. */
CREATE TABLE Connections (
	FromNeuronID MEDIUMINT UNSIGNED NOT NULL,
	ToNeuronID MEDIUMINT UNSIGNED NOT NULL,
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL,
	
	Delay FLOAT UNSIGNED NOT NULL,
	Weight FLOAT NOT NULL,
	TempWeight FLOAT DEFAULT 0.0,/* To allow the user to view the connections without overwriting the current connections */

	PRIMARY KEY (FromNeuronID, ToNeuronID),
	INDEX ConnectionGroupID (ConnectionGroupID),
	INDEX ToNeuronIndex (ToNeuronID),

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
	Description CHAR(100) NOT NULL,
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
CREATE TABLE WeightlessNeuronLookupTables (
	NeuronID MEDIUMINT UNSIGNED NOT NULL,
	Pattern MEDIUMBLOB NOT NULL,
	OUTPUT BOOLEAN NOT NULL,

	PRIMARY KEY (NeuronID),
	INDEX NeuronIDIndex(NeuronID),

    FOREIGN KEY NeuronID_FK(NeuronID) REFERENCES Neurons(NeuronID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* WeightlessNeuronPatterns
    Each position in the lookup table pattern refers to connection between two neurons.
*/
CREATE TABLE WeightlessNeuronConnections (
	FromNeuronID MEDIUMINT UNSIGNED NOT NULL,
	ToNeuronID MEDIUMINT UNSIGNED NOT NULL,
	PatternIndex MEDIUMINT UNSIGNED NOT NULL,

	PRIMARY KEY (FromNeuronID, ToNeuronID),

    FOREIGN KEY FromNeuronID_FK(FromNeuronID) REFERENCES Neurons(NeuronID) ON DELETE CASCADE,
    FOREIGN KEY ToNeuronID_FK(ToNeuronID) REFERENCES Neurons(NeuronID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Re-enable foreign key checks on the tables */
SET foreign_key_checks = 1;

