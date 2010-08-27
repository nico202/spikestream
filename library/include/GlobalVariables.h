
//--------------------------  Global Variables  ----------------------------
/* Definitions of variables that apply across the whole application. */
//--------------------------------------------------------------------------

#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

//Other includes
#include <cstring>


/*! The maximum length of a name in the database.*/
#define MAX_DATABASE_NAME_LENGTH 250


/*! The maximum length of a description in the database.*/
#define MAX_DATABASE_DESCRIPTION_LENGTH 250


/*! File extension for saving archive file of databases.*/
#define NEURON_APPLICATION_FILE_EXTENSION ".sql.tar.gz"

//Starting and invalid database IDs
/*! First valid automatically generated ID in NeuralNetwork database */
#define START_NEURALNETWORK_ID 1

/*! A neural network ID that is not valid */
#define INVALID_NETWORK_ID 0

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_NEURONGROUP_ID 1

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_NEURON_ID 1

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_CONNECTION_ID 1

/*! First valid automatically generated ID in NeuralNetwork database */
#define START_CONNECTIONGROUP_ID 1

/*! First valid automatically generated ID in Archives database */
#define START_ARCHIVE_ID 1

/*! First valid automatically generated ID in Archives database */
#define START_ANALYSIS_ID 1

/*! First valid automatically generated ID for a weightless neuron pattern */
#define START_WEIGHTLESS_TRAINING_PATTERN_ID 1


//Connection mode flags
/*! Show connections to/from a single neuron */
#define CONNECTION_MODE_ENABLED 1

/*! Show connections between two neurons */
#define SHOW_BETWEEN_CONNECTIONS 4

/*! Show positive connections */
#define SHOW_POSITIVE_CONNECTIONS 8

/*! Show negative connections */
#define SHOW_NEGATIVE_CONNECTIONS 16

/*! Show all connections from a single neuron */
#define SHOW_FROM_CONNECTIONS 32

/*! Show all connections to a single neuron */
#define SHOW_TO_CONNECTIONS 64


//Weight render flags
/*! Render weights as polygons */
#define WEIGHT_RENDER_ENABLED 1

/*! Render temp weights */
#define RENDER_TEMP_WEIGHTS 2

/*! Render current weights */
#define RENDER_CURRENT_WEIGHTS 4


//Heat colour map colours
/*! Heat map colour 0 */
#define HEAT_COLOR_0 0, 0.5, 0

/*! Heat map colour 1 */
#define HEAT_COLOR_1 0, 0, 1.0

/*! Heat map colour 2 */
#define HEAT_COLOR_2 0.125, 0, 0.875

/*! Heat map colour 3 */
#define HEAT_COLOR_3 0.25, 0, 0.75

/*! Heat map colour 4 */
#define HEAT_COLOR_4 0.375, 0, 0.625

/*! Heat map colour 5 */
#define HEAT_COLOR_5 0.625, 0, 0.375

/*! Heat map colour 6 */
#define HEAT_COLOR_6 0.75, 0, 0.25

/*! Heat map colour 7 */
#define HEAT_COLOR_7 0.875, 0, 0.125

/*! Heat map colour 8 */
#define HEAT_COLOR_8 1.0, 0, 0

/*! Heat map colour 9 */
#define HEAT_COLOR_9 1.0, 0.5, 0

/*! Heat map colour 10 */
#define HEAT_COLOR_10 1.0, 1.0, 0


/*! Default value associated with heat colour 10.
	Heat colour 0 is always associated with 0. */
#define DEFAULT_MAX_HEAT_COLOR_VALUE 10.0


/*! A state based phi analysis */
#define STATE_BASED_PHI_ANALYSIS "statebasedphianalysis"


/*! A liveliness analysis */
#define LIVELINESS_ANALYSIS "livelinessanalysis"


#endif //GLOBALVARIABLES_H

