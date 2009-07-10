/*---------------------------- Devices Database ----------------------------*/
/* Database to hold information about all of the devices that the 
	simulation can connect to.
	NOTE: This database has been left largely 'as is' for the moment. */
/* ----------------------------------------------------------------------------*/

/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;


/* Create and use the Devices database. */
DROP DATABASE IF EXISTS SpikeStreamDevice;
CREATE DATABASE SpikeStreamDevice;
USE SpikeStreamDevice;


/* Devices
	Table holding information about the devices. */
CREATE TABLE Devices (
	DeviceID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	Description CHAR(100) NOT NULL,
	Type TINYINT UNSIGNED NOT NULL,
	IPAddress CHAR(100) NOT NULL,
	Port INT UNSIGNED DEFAULT 0,
	TotalNumColumns SMALLINT UNSIGNED NOT NULL,
	TotalNumRows SMALLINT UNSIGNED NOT NULL,
	PRIMARY KEY (DeviceID),
	UNIQUE (Description),
	INDEX IDIndex (DeviceID)
)
ENGINE=InnoDB;


/* SIMNOSReceptors
	Table used for SIMNOS to record all of its input and output receptors
	These are positions in the main Devices tables for SIMNOS input and output.
	DeviceDescription references the Description of the device in the Devices table */
CREATE TABLE SIMNOSSpikeReceptors (
	ReceptorID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	Name CHAR(255) NOT NULL,
	DeviceDescription CHAR(255) NOT NULL,
	NumColumns SMALLINT NOT NULL,
	NumRows SMALLINT NOT NULL,
	StartColumn SMALLINT NOT NULL,
	StartRow SMALLINT NOT NULL,
	PRIMARY KEY (ReceptorID),
	UNIQUE (Name),
	INDEX IDIndex (ReceptorID)
)
ENGINE=InnoDB;


/* SIMNOSComponents
	Records groupings of the SIMNOS data so that it can be connected more
	conveniently to the simulator. */
CREATE TABLE SIMNOSComponents (
	ComponentID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	Name CHAR(255) NOT NULL,
	ReceptorIDs TEXT NOT NULL,
	Width SMALLINT UNSIGNED NOT NULL,
	Length SMALLINT UNSIGNED NOT NULL,
	PRIMARY KEY (ComponentID),
	UNIQUE (Name),
	INDEX IDIndex (ComponentID)
)
ENGINE=InnoDB;


/* Records whether a neuron group is delaying itself. 
	Has to be in the database because tasks can be distributed across several machines 
	Each task adds itself to this table if it is delaying and removes itself when it
	stops delaying. Other tasks can then see if there is a synchronization delay by 
	counting the number of rows. 
	FIXME THIS APPROACH COULD RUN INTO TROUBLE WHEN THERE ARE MORE THAN 1 DEVICES PROVIDING
	DATA
*/
CREATE TABLE SynchronizationDelay (
	NeuronGroupID SMALLINT UNSIGNED NOT NULL,
	TaskID SMALLINT UNSIGNED NOT NULL,
	PRIMARY KEY (NeuronGroupID),
	INDEX NeuronGroupIDIndex (NeuronGroupID),
    FOREIGN KEY NeuronGroupID_FK(NeuronGroupID) REFERENCES SpikeStreamNetwork.NeuronGroups(NeuronGroupID) ON DELETE NO ACTION
)
ENGINE=InnoDB;


/* Re-enable foreign key checks */
SET foreign_key_checks = 1;



