/*------------------------  SpikeStreamNeuralNetwork ----------------------*/
/* Holds information about the simulation. */             
/*-------------------------------------------------------------------------*/

/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;


/* Create the database */
DROP DATABASE IF EXISTS SpikeStreamSimulation;
CREATE DATABASE SpikeStreamSimulation;
USE SpikeStreamSimulation;


/* Records which tasks are simulating which neuron groups 
*/
CREATE TABLE Tasks (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	TaskID SMALLINT UNSIGNED NOT NULL,

	PRIMARY KEY (NeuronGroupID),
	INDEX (NeuronGroupID),
    FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES SpikeStreamNetwork.NeuronGroups(NeuronGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Re-enable foreign key checks */
SET foreign_key_checks = 1;

