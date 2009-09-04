/*------------------------------ Add Devices ------------------------------*/
/*  Adds the current devices. NOTE: Unchanged from SpikeStream 0.1 */
/*-------------------------------------------------------------------------*/

/* SQL to create a couple of example devices. */
INSERT INTO Devices (Description, Type, IPAddress, Port, TotalNumColumns, TotalNumRows) VALUES ("Example Device", 1, "155.245.23.250", 2235, 10, 7);


/* SQL to create an example entries for the current SIMNOS receptors. */
INSERT INTO SIMNOSSpikeReceptors (Name, DeviceDescription, StartColumn, StartRow, NumColumns, NumRows) VALUES ("Example receptor 0", "Example Device", 0, 0, 10, 1);
INSERT INTO SIMNOSSpikeReceptors (Name, DeviceDescription, StartColumn, StartRow, NumColumns, NumRows) VALUES ("Example receptor 1", "Example Device", 0, 1, 10, 1);
INSERT INTO SIMNOSSpikeReceptors (Name, DeviceDescription, StartColumn, StartRow, NumColumns, NumRows) VALUES ("Example receptor 2", "Example Device", 0, 2, 10, 1);
INSERT INTO SIMNOSSpikeReceptors (Name, DeviceDescription, StartColumn, StartRow, NumColumns, NumRows) VALUES ("Example receptor 3", "Example Device", 0, 3, 10, 1);
INSERT INTO SIMNOSSpikeReceptors (Name, DeviceDescription, StartColumn, StartRow, NumColumns, NumRows) VALUES ("Example receptor 4", "Example Device", 0, 4, 10, 1);
INSERT INTO SIMNOSSpikeReceptors (Name, DeviceDescription, StartColumn, StartRow, NumColumns, NumRows) VALUES ("Example receptor 5", "Example Device", 0, 5, 10, 1);
INSERT INTO SIMNOSSpikeReceptors (Name, DeviceDescription, StartColumn, StartRow, NumColumns, NumRows) VALUES ("Example receptor 6", "Example Device", 0, 6, 10, 1);


/* SQL to create entries for the current SIMNOS components. */
INSERT INTO SIMNOSComponents (Name, ReceptorIDs, Width, Length) VALUES ("Example component description", "1,3,4", 10, 3);


