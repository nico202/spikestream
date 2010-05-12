
/*---------------------------- Add Synapse Types ---------------------------
	SQL to add the current synapse types to the NeuralNetwork Database.
	If you create a new synapse class you will need to add an entry to the
	SynapseTypes table and a new table to hold its parameters.
	NOTE Currently the entries to synapse types are added by the 
	CreateSpikeStreamDatabases script so that it can add the librarys at
	SPIKESTREAM_ROOT/lib.
---------------------------------------------------------------------------*/

/* Add entry to the SynapseTypes table */
INSERT INTO SynapseTypes(SynapseTypeID, Description, ParameterTableName, ClassLibrary) VALUES (1, "Izhikevich Synapse 1", "IzhikevichSynapse1Parameters", "");

INSERT INTO SynapseTypes(SynapseTypeID, Description, ParameterTableName, ClassLibrary) VALUES (2, "Weightless Synapse", "WeightlessSynapseParameters", "");


/* Add table for STDP1Synapse parameters
	Table containing the parameters for the STDO1 synapse type
	Each connection group created with this type will have an entry in this table.*/
CREATE TABLE STDP1SynapseParameters (
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL,
	Learning_val BOOLEAN DEFAULT 0,
	Learning_desc CHAR(100) DEFAULT "Learning",
	Disable_val BOOLEAN DEFAULT 0,
	Disable_desc CHAR(100) DEFAULT "Disable",
	CalciumThreshUpLow_val DOUBLE DEFAULT 30.0,
	CalciumThreshUpLow_desc CHAR(100) DEFAULT "Calcium threshold up low",
	CalciumThreshUpHigh_val DOUBLE DEFAULT 120.0,
	CalciumThreshUpHigh_desc CHAR(100) DEFAULT "Calcium threshold up high",
	CalciumThreshDownLow_val DOUBLE DEFAULT 30.0,
	CalciumThreshDownLow_desc CHAR(100) DEFAULT "Calcium threshold down low",
	CalciumThreshDownHigh_val DOUBLE DEFAULT 40.0,
	CalciumThreshDownHigh_desc CHAR(100) DEFAULT "Calcium threshold down high",
	WeightChangeThreshold_val DOUBLE DEFAULT 0.8,
	WeightChangeThreshold_desc CHAR(100) DEFAULT "Weight change threshold",
	WeightIncreaseAmnt_val DOUBLE DEFAULT 0.1,
	WeightIncreaseAmnt_desc CHAR(100) DEFAULT "Weight increase amount",
	WeightDecreaseAmnt_val DOUBLE DEFAULT 0.1,
	WeightDecreaseAmnt_desc CHAR(100) DEFAULT "Weight decrease amount",
	DriftThreshold_val DOUBLE DEFAULT 0.5,
	DriftThreshold_desc CHAR(100) DEFAULT "Drift threshold",
	PositiveDrift_val DOUBLE DEFAULT 0.0,
	PositiveDrift_desc CHAR(100) DEFAULT "Positive drift",
	NegativeDrift_val DOUBLE DEFAULT 0.0,
	NegativeDrift_desc CHAR(100) DEFAULT "Negative drift",
	MaximumDrift_val DOUBLE DEFAULT 1.0,
	MaximumDrift_desc CHAR(100) DEFAULT "Maximum drift",
	MinimumDrift_val DOUBLE DEFAULT 0.0,
	MinimumDrift_desc CHAR(100) DEFAULT "Minimum drift",

	PRIMARY KEY (ConnectionGroupID),
	FOREIGN KEY ConnectionGroupID_FK(ConnectionGroupID) REFERENCES ConnectionGroups(ConnectionGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;




/* Add table for WeigthlessSynapse parameters
	Table containing the parameters for the STDO1 synapse type
	Each connection group created with this type will have an entry in this table.*/
CREATE TABLE WeightlessSynapseParameters (
	ConnectionGroupID SMALLINT UNSIGNED NOT NULL,
	Test_val BOOLEAN DEFAULT 0,
	Test_desc CHAR(100) DEFAULT "Test parameter",

	PRIMARY KEY (ConnectionGroupID),
	FOREIGN KEY ConnectionGroupID_FK(ConnectionGroupID) REFERENCES ConnectionGroups(ConnectionGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


