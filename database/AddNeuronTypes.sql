
/*---------------------------- Add Neuron Types ---------------------------- */
/*	SQL to add the current neuron types to the Network Database.*/
/*-----------------------------------------------------------------------------*/

/* Add an entry for the STDP1 Neuron to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (1, "STDP1 Neuron", "STDP1NeuronParameters", "libstdp1neuron.so");

/* Add table for STDP1 Neuron parameters
	Table containing the parameters for the Spike time dependent plasticity Neuron type 1
	Each neuron group created with this type will have an entry in this table.*/
CREATE TABLE STDP1NeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	CalciumIncreaseAmnt_val DOUBLE DEFAULT 1.0,
	CalciumIncreaseAmnt_desc CHAR(100) DEFAULT "Calcium increase amount",
	CalciumDecayRate_val DOUBLE DEFAULT 60.0,
	CalciumDecayRate_desc CHAR(100) DEFAULT "Calcium decay rate",
	RefractoryPeriod_val DOUBLE DEFAULT 1.0,
	RefractoryPeriod_desc CHAR(100) DEFAULT "Refractory period (ms)",
	MembraneTimeConstant_val DOUBLE DEFAULT 3.0,
	MembraneTimeConstant_desc CHAR(100) DEFAULT "Membrane time constant (ms)",
	RefractoryParamM_val DOUBLE DEFAULT 0.8,
	RefractoryParamM_desc CHAR(100) DEFAULT "Refractory parameter M",
	RefractoryParamN_val DOUBLE DEFAULT 3.0,
	RefractoryParamN_desc CHAR(100) DEFAULT "Refractory parameter N",
	Threshold_val DOUBLE DEFAULT 1.0,
	Threshold_desc CHAR(100) DEFAULT "Threshold",
	MinPostsynapticPotential_val DOUBLE DEFAULT -5.0,
	MinPostsynapticPotential_desc CHAR(100) DEFAULT "Min postsynaptic potential",
	Learning_val BOOLEAN DEFAULT 0,
	Learning_desc CHAR(100) DEFAULT "Learning",

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Add an entry for the STDP1 Neuron to the NeuronTypes table */
INSERT INTO NeuronTypes(NeuronTypeID, Description, ParameterTableName, ClassLibrary) VALUES (2, "Weightless Neuron", "WeigthlessNeuronParameters", "undefined.so");


/* Add table for STDP1 Neuron parameters
	Table containing the parameters for the Spike time dependent plasticity Neuron type 1
	Each neuron group created with this type will have an entry in this table.*/
CREATE TABLE WeightlessNeuronParameters (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	Generalization_val DOUBLE DEFAULT 1.0,
	Generalization_desc CHAR(100) DEFAULT "Generalization",

	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;

