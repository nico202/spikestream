
/*--------------------------- Patterns Database ---------------------------*/
/*  Database to hold patterns that are applied to the network for testing
	and training. */
/*-------------------------------------------------------------------------*/

/* Disable foreign key checks whilst creating tables */
SET foreign_key_checks = 0;


/* Create and use the database. */
DROP DATABASE IF EXISTS SpikeStreamPattern;
CREATE DATABASE SpikeStreamPattern;
USE SpikeStreamPattern;


/* PatternDescriptions
	Holds information about a set of patterns. */
CREATE TABLE PatternGroups (
	PatternGroupID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	Description CHAR(200),
	PatternTypeID SMALLINT UNSIGNED NOT NULL,/* Type of pattern from PatternTypes table */
	Width SMALLINT UNSIGNED NOT NULL,
	Length SMALLINT UNSIGNED NOT NULL,
	File CHAR(200),

	PRIMARY KEY (PatternGroupID),
    FOREIGN KEY PatternTypeID_FK(PatternTypeID) REFERENCES PatternTypes(PatternTypeID) ON DELETE NO ACTION
)
ENGINE=InnoDB;


/* PatternData
	The data associated with a pattern description. */
CREATE TABLE PatternData (
	PatternDataID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
	PatternGroupID SMALLINT UNSIGNED NOT NULL,
	Pattern BLOB NOT NULL,

	PRIMARY KEY (PatternDataID),
	INDEX PatternGroupIDIndex(PatternGroupID),
    FOREIGN KEY PatternGroupID_FK(PatternGroupID) REFERENCES PatternGroups(PatternGroupID) ON DELETE CASCADE
)
ENGINE=InnoDB;


/* Types of pattern */
CREATE TABLE PatternTypes (
	PatternTypeID SMALLINT UNSIGNED NOT NULL,
	Description CHAR(100),
	PRIMARY KEY (PatternTypeID)
)
ENGINE=InnoDB;


/* Re-enable foreign key checks */
SET foreign_key_checks = 1;
