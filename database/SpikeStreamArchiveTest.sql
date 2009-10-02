/*------------------ SpikeStreamArchive -----------------------*/
/*        Stores information about recorded simulation runs    */
/*-------------------------------------------------------------*/

/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;

/* SimulationRun
    Holds general information about each simulation run.
    Entries will be automatically deleted if there is no associated simulation data.
*/
CREATE TABLE Archives (
	ArchiveID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
	StartTime BIGINT NOT NULL, /*When the simulation was initialized */
	NetworkID SMALLINT NOT NULL, /* References a neural network in the SpikeStreamNetwork table. This network should not be editable if it is associated with simulation data. */
	Description CHAR(100),/* Brief description of the run */

	PRIMARY KEY (ArchiveID),
	FOREIGN KEY NetworkID_FK(NetworkID) REFERENCES SpikeStreamNetworkTest.Networks(NetworkID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* SimulationData
	The data for a particular simulation run. An entry for each recorded time
	step containing the firing pattern for that point in the simulation. 
*/
CREATE TABLE ArchiveData (
	ArchiveID MEDIUMINT UNSIGNED NOT NULL, /* References the meta data about the simulation run in the SimulationRun table. */
	TimeStep INT UNSIGNED NOT NULL, /* The time step of the firing pattern. */
	FiringNeurons LONGTEXT NOT NULL, /* Comma separated list of neuron ids that were firing at the time step. */

	PRIMARY KEY (ArchiveID, TimeStep), /* Each simulation run is associated with a number of time steps, which should all be different. */
	FOREIGN KEY ArchiveID_FK(ArchiveID) REFERENCES Archives(ArchiveID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Re-enable foreign key checks */
SET foreign_key_checks = 1;

