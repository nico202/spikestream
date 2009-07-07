 
/* Create and use the database. */
DROP DATABASE IF EXISTS SpikeStreamNeuralNetwork;
CREATE DATABASE SpikeStreamNeuralNetwork;
USE SpikeStreamNeuralNetwork;

/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;


/* NeuralNetwork
	Allows a number of different networks to be held in the same database.
*/
CREATE TABLE NeuralNetworks (
    NeuralNetworkID SMALLINT NOT NULL AUTO_INCREMENT,
    Name CHAR(100),
    PRIMARY KEY (NeuralNetworkID),
    INDEX NeuralNetworkIDIndex(NeuralNetworkID)
)
ENGINE=InnoDB;


/* NeuronGroups
    Neurons are gathered together into neuron groups. 
    This table stores an overall geometry for the neuron group as the minimum bounding 
    NOTE: Ideally this would be a geometry field, but MySQL only supports 2D geometry at present
*/
CREATE TABLE NeuronGroups (
    NeuronGroupID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT, /* Primary key */
    NeuralNetworkID SMALLINT NOT NULL, /* Network that this group is part of */
    Name CHAR(50), /*Short name, for example, 'motor cortex' */
    Description CHAR(100), /* Description, for example, layer, etc. */
    Parameters BLOB, /* XML describing the parameters used to create the group */
    NeuronTypeID SMALLINT UNSIGNED NOT NULL, /*The type of neuron in the layer. Used for parameters and class loading */

     /* Minimum bounding rectangular box of the neuron group. Ideally would be a 3D geometry field, but not currently supported by MySQL */
    X SMALLINT NOT NULL,
    Y SMALLINT NOT NULL,
    Z SMALLINT NOT NULL,
    Width SMALLINT NOT NULL,
    Length SMALLINT NOT NULL,
    Height SMALLINT NOT NULL,

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
    INDEX Positions (X, Y, Z),/* Used when creating connections. */
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
	dynamically loaded classes. Each entry in this table needs a corresponding
	table containing the parameters for this neuron type.
	AddNeuronTypes.sql adds the individual types and associated parameter tables. */
CREATE TABLE NeuronTypes (
    NeuronTypeID SMALLINT UNSIGNED NOT NULL,
    Description CHAR(100) NOT NULL,
    ParameterTableName CHAR(100) NOT NULL,
    ClassLibrary CHAR(100) NOT NULL,
    PRIMARY KEY (NeuronTypeID),
    UNIQUE (Description)
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
	the simulation without storing the weights permanently. */
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
	PRIMARY KEY (SynapseTypeID),
	UNIQUE (Description)
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
    Each position in the lookup table pattern refers to a particular neuron.
*/
CREATE TABLE WeightlessNeuronConnections (
	FromNeuronID MEDIUMINT UNSIGNED NOT NULL,
	ToNeuronID MEDIUMINT UNSIGNED NOT NULL,
	PatternIndex MEDIUMINT UNSIGNED NOT NULL,

    FOREIGN KEY NeuronID_FK(NeuronID) REFERENCES Neurons(NeuronID) ON DELETE CASCADE
)
ENGINE=InnoDB;




/* Re-enable foreign key checks on the tables */
SET foreign_key_checks = 1;

