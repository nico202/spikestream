
/*--------------------------- Patterns Database ----------------------------
	Database to hold patterns that are applied to the network for testing
	and training.
---------------------------------------------------------------------------*/

/* Create and use the database. */
DROP DATABASE IF EXISTS Patterns;
CREATE DATABASE Patterns;
USE Patterns;


/* PatternDescriptions
	Holds information about a set of patterns. */
CREATE TABLE PatternDescriptions (
	PatternGrpID SMALLINT UNSIGNED NOT NULL AUTO_INCREMENT,
	Description CHAR(200),
	PatternType TINYINT UNSIGNED NOT NULL,
	Width SMALLINT UNSIGNED NOT NULL,
	Length SMALLINT UNSIGNED NOT NULL,
	File CHAR(200) NOT NULL,
	NumberOfPatterns INT UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (PatternGrpID)
)
ENGINE=InnoDB;


/* PatternData
	The data associated with a pattern description. */
CREATE TABLE PatternData (
	PatternID MEDIUMINT UNSIGNED NOT NULL AUTO_INCREMENT,
	PatternGrpID SMALLINT UNSIGNED NOT NULL,
	Pattern BLOB NOT NULL,
	PRIMARY KEY (PatternID),
	INDEX IDIndex(PatternGrpID)
)
ENGINE=InnoDB;



