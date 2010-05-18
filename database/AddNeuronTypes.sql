
/*---------------------------- Add Neuron Types ---------------------------- */
/*	SQL to add the current neuron types to the Network Database.*/
/*-----------------------------------------------------------------------------*/

/* Add an entry for the STDP1 Neuron to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (1, "Izhikevich Neuron", "IzhikevichNeuronParameters", "");

/* Add table for Izhikevich Neuron parameters. 
	These are described in E. M. Izhikevich. Simple model of spiking neurons. IEEE Trans. Neural
	Networks, 14:1569{1572, 2003.*/
CREATE TABLE IzhikevichNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	a DOUBLE DEFAULT 1.0 COMMENT 'Time scale of the recovery variable.',

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Add an entry for the STDP1 Neuron to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (2, "Weightless Neuron", "WeightlessNeuronParameters", "");


/* Add table for STDP1 Neuron parameters
	Table containing the parameters for the Spike time dependent plasticity Neuron type 1
	Each neuron group created with this type will have an entry in this table.*/
CREATE TABLE WeightlessNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	Generalization DOUBLE DEFAULT 1.0 COMMENT 'Degree of match using hamming distance with incoming pattern',

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


