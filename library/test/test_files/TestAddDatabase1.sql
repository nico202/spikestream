/*------------------------  SpikeStreamNetwork ----------------------*/
/* Holds the neurons and connections. All of the databases are created using
	InnoDB so that we can use foreign keys. Performance looks roughly the same
	or better than MyISAM. */             
/*-------------------------------------------------------------------------*/
 
/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;


/* Create and use SpikeStreamNetwork database */
DROP DATABASE IF EXISTS SpikeStreamTest1;
CREATE DATABASE SpikeStreamTest1;
USE SpikeStreamTest1;


/* First test table depends on second */
CREATE TABLE Test2Table (
    NeuronGroupID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
    NetworkID SMALLINT NOT NULL,
    NeuronTypeID SMALLINT UNSIGNED NOT NULL,
    PRIMARY KEY (NeuronGroupID),
	FOREIGN KEY TestRef_FK(NetworkID) REFERENCES Test1Table(testID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Second test table referenced by the first
*/
CREATE TABLE Test1Table (
    testID SMALLINT NOT NULL AUTO_INCREMENT,
    testName CHAR(250) NOT NULL DEFAULT "Untitled",
    testDescription CHAR(250) NOT NULL DEFAULT "Untitled",
    PRIMARY KEY (testID)
)
ENGINE=InnoDB;


/* Re-enable foreign key checks on the tables */
SET foreign_key_checks = 1;

