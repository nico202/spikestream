
/*---------------------------- Add Synapse Types ---------------------------
	SQL to add the current synapse types to the NeuralNetwork Database.
	If you create a new synapse class you will need to add an entry to the
	SynapseTypes table and a new table to hold its parameters.
	NOTE Currently the entries to synapse types are added by the 
	CreateSpikeStreamDatabases script so that it can add the librarys at
	SPIKESTREAM_ROOT/lib.
---------------------------------------------------------------------------*/

/* Add entry to the SynapseTypes table */
INSERT INTO SynapseTypes(SynapseTypeID, Description, ParameterTableName, ClassLibrary) VALUES (1, "Izhikevich Synapse", "IzhikevichSynapseParameters", "");

INSERT INTO SynapseTypes(SynapseTypeID, Description, ParameterTableName, ClassLibrary) VALUES (2, "Weightless Synapse", "WeightlessSynapseParameters", "");


/* Add table for Izhikevich Synapse parameters
	Table containing the parameters for the Izhikevich synapse
	Currently only switches learning on or off - the STDP function is set globally. */
CREATE TABLE IzhikevichSynapseParameters (
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL,
	Learning BOOLEAN DEFAULT 0 COMMENT 'Switches STDP learning on and off.',
	Disable BOOLEAN DEFAULT 0 COMMENT 'Disables the connection group, which will not be included in the simulation.',
	weight_factor DOUBLE DEFAULT 10.0 COMMENT 'Factor by which weights are multiplied for the simulation. When set to 10 it typically takes two synchronous spikes to fire an Izhikevich neuron.',

	PRIMARY KEY (ConnectionGroupID),
	FOREIGN KEY ConnectionGroupID_FK(ConnectionGroupID) REFERENCES ConnectionGroups(ConnectionGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Add table for WeigthlessSynapse parameters
	Table containing the parameters for the Weightless synapse type
	Each connection group created with this type will have an entry in this table.*/
CREATE TABLE WeightlessSynapseParameters (
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL,

	PRIMARY KEY (ConnectionGroupID),
	FOREIGN KEY ConnectionGroupID_FK(ConnectionGroupID) REFERENCES ConnectionGroups(ConnectionGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


