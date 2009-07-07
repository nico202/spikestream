/*------------------ SpikeStreamArchive -----------------------*/
/*        Stores information about recorded simulation runs    */
/*-------------------------------------------------------------*/

DROP DATABASE IF EXISTS SpikeStreamArchive;
CREATE DATABASE SpikeStreamArchive;
USE SpikeStreamArchive;


/* SimulationRun
    Holds general information about each simulation run.
    Entries will be automatically deleted if there is no associated simulation data.
*/
CREATE TABLE SimulationRun (
	SimulationRunID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
	StartTime BIGINT NOT NULL, /*When the simulation was initialized */
	NeuralNetworkID SMALLINT NOT NULL, /* References a neural network in the SpikeStreamNetwork table. This network should not be editable if it is associated with simulation data. */
	Name CHAR(50),/* Brief descriptive name. */

	PRIMARY KEY (SimulationRunID),
	INDEX SimulationRunIDIndex(SimulationRunID),
	FOREIGN KEY NeuralNetworkID_FK(NeuralNetworkID) REFERENCES SpikeStreamNetwork.NeuralNetworks(NeuralNetworkID) ON DELETE CASCADE
)
ENGINE=InnoDB;/*Allows foreign key constraints */


/* SimulationData
	The data for a particular simulation run. There is an entry for each recorded time
	step containing the firing pattern for that moment in the simulation. 
*/
CREATE TABLE SimulationData (
	SimulationRunID MEDIUMINT NOT NULL, /* References the meta data about the simulation run in the SimulationRun table. */
	TimeStep INT UNSIGNED NOT NULL, /* The time step of the firing pattern. */
	FiringNeurons LONGTEXT NOT NULL,/* Comma separated list of neuron ids that were firing at the time step. */

	PRIMARY KEY(SimulationRunID, TimeStep),
	INDEX TimeStepIndex(SimulationRunID, TimeStep),
	FOREIGN KEY SimulationRunID_FK(SimulationRunID) REFERENCES SimulationRun(SimulationRunID) ON DELETE CASCADE
)
ENGINE=InnoDB /*Allows foreign key constraints */;


Need the full connection and weight information in the archive
  person creates a network, records its behaviour, edits the network and records its behaviour. 
  Unless the full information is stored, will have lost all the connection and weight information from the first network.

So when the person clicks record, a new archive entry is created in the archive database in the network table.
When they de-initilize the simulation, ask if they want to keep what they have recorded. If they do, copy the current network into the archive with progress bar etc.

 
CREATE TABLE copyname SELECT * FROM originalname