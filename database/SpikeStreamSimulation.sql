DROP DATABASE IF EXISTS SpikeStreamSimulation;
CREATE DATABASE SpikeStreamSimulation;
USE SpikeStreamSimulation;

/* Records which tasks are simulating which neuron groups 
*/
CREATE TABLE Tasks (
	NeuronGroupID SMALLINT NOT NULL,
	TaskID,

	PRIMARY KEY (NeuronGroupID),
	INDEX (NeuronGroupID),
    FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES SpikeStreamNetwork.NeuralNetworks(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;

