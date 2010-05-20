
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
	
	
	
	inhib_a_1 DOUBLE DEFAULT 0.02 COMMENT 'Time scale of the recovery variable, u, for an inhibitory neuron. \na_1 is the first parameter that is used to generate a in equation a = a_1 + a_2 * random.',
	inhib_a_2 DOUBLE DEFAULT 0.08 COMMENT 'Time scale of the recovery variable, u for an inhibitory neuron. a_2 is the second parameter that is used to generate a in equation a = a_1 + a_2 * random.',
	inhib_b_1 DOUBLE DEFAULT 0.25 COMMENT 'Sensitivity to sub-threshold fluctutations in the membrane potential, v, for an inhibitory neuron. b_1 is the first parameter that is used to generate a in equation b = b_1 - b_2 * random.',
	inhib_b_2 DOUBLE DEFAULT 0.05 COMMENT 'Sensitivity to sub-threshold fluctutations in the membrane potential, v, for an inhibitory neuorn. b_1 is the first parameter that is used to generate a in equation b = b_1 - b_2 * random.',
	inhib_d DOUBLE DEFAULT 2.0 COMMENT 'After-spike reset of the recovery variable, u, for an inhibitory neuron.',
	v DOUBLE DEFAULT -65.0 COMMENT 'Initial value for the membrane potential.',
	sigma DOUBLE DEFAULT 2.0 COMMENT 'Parameter for a random gaussian per-neuron process which generates random input current drawn from an N(0,\a sigma) distribution. If set to zero no random input current will be generated.',
	
	
	PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;

//Excitatory
	float v = -65.0f;
	float a = 0.02f;
	float b = 0.2f;
	float r1 = float(param());
	float r2 = float(param());
	float c = v + 15.0f * r1 * r1;
	float d = 8.0f - 6.0f * r2 * r2;
	float u = b * v;
	float sigma = 5.0f;

//Inhibitory
	float v = -65.0f;
	float r1 = float(param());
	float a = 0.02f + 0.08f * r1;
	float r2 = float(param());
	float b = 0.25f - 0.05f * r2;
	float c = v;
	float d = 2.0f;
	float u = b * v;
	float sigma = 2.0f;


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


