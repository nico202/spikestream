/* Disable foreign key checks whilst creating tables etc. */
SET foreign_key_checks = 0;

/* Create the SpikeStream databases. */
DROP DATABASE IF EXISTS SpikeStreamNetwork;
CREATE DATABASE SpikeStreamNetwork;

DROP DATABASE IF EXISTS SpikeStreamNetworkTest;
CREATE DATABASE SpikeStreamNetworkTest;

DROP DATABASE IF EXISTS SpikeStreamArchive;
CREATE DATABASE SpikeStreamArchive;

DROP DATABASE IF EXISTS SpikeStreamArchiveTest;
CREATE DATABASE SpikeStreamArchiveTest;

DROP DATABASE IF EXISTS SpikeStreamAnalysis;
CREATE DATABASE SpikeStreamAnalysis;

DROP DATABASE IF EXISTS SpikeStreamAnalysisTest;
CREATE DATABASE SpikeStreamAnalysisTest;

DROP DATABASE IF EXISTS SpikeStreamSimulation;
CREATE DATABASE SpikeStreamSimulation;

DROP DATABASE IF EXISTS SpikeStreamSimulationTest;
CREATE DATABASE SpikeStreamSimulationTest;

DROP DATABASE IF EXISTS SpikeStreamPattern;
CREATE DATABASE SpikeStreamPattern;

DROP DATABASE IF EXISTS SpikeStreamPatternTest;
CREATE DATABASE SpikeStreamPatternTest;

DROP DATABASE IF EXISTS SpikeStreamDevices;
CREATE DATABASE SpikeStreamDevices;

DROP DATABASE IF EXISTS SpikeStreamDevicesTest;
CREATE DATABASE SpikeStreamDevicesTest;

/* Re-enable foreign key checks */
SET foreign_key_checks = 1;