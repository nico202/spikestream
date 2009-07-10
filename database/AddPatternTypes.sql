/*--------------------------- Add Pattern Types ---------------------------*/
/*  Adds the available pattern types to the database. 
	Temporal patterns are either -1 for non-firing neurons or a delay value 
	between 0 and up to around 200.
	Static patterns consist of 1s and 0s. */
/*-------------------------------------------------------------------------*/

/* Use SpikeStreamPatterns database */
USE SpikeStreamPattern;

/* Add entries to the PatternTypes table */
INSERT INTO PatternTypes(PatternTypeID, Description) VALUES (1, "Temporal");
INSERT INTO PatternTypes(PatternTypeID, Description) VALUES (2, "Static");


