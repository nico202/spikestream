#ifndef NRMCONSTANTS_H
#define NRMCONSTANTS_H


#define FRAME_WIN 0


//Input panel
#define MULTI_WIN 1

//Input that is a single frame within an image
#define FRAME_WIN_2 15

//Input that is a double frame within an image
#define _FRAME_WIN 16

//A neural layer associated with a previous neural layer
#define MAGNUS_WIN_PREV 6
#define NEURAL_LAYER 0

/* Several neural layers can be inside the same frame
	This constant defines whether a layer needs its own frame or a different frame */
#define FRAME_REQ 0
#define NO_FRAME_REQ 1

/*! Neural layer types */
#define MAGNUS_IN_OB 0
#define MAGNUS_PREV_OB 1
#define MAGNUS_STATE_OB 2

/*! Input layer. NOTE: 55 is also used to code input layers */
#define IMG_INPUT_OB 5

/*! Hamming spreading */
#define CM_HAMMING 3361

/*! Alt spreading */
#define CM_ALT_SPREAD 3351

/*! Large network */
#define BIG_NET 0

/*! Small network */
#define SMALL_NET 1000

#define MULTI_WIN_T 2


#define MAGNUS_WIN 3
#define MAGNUS_WIN_OUT 4
#define MAGNUS_WIN_OUT_T 5



#define MAGNUS_WIN_PREV_T 7
#define INPUTS_FRAME 10
#define NEURAL_FRAME 11
#define MAGNUS_FRAME 12
#define MAGNUS_FRAME_NP 13

#define RAND_COLS 0
#define LIN_COLS 1
#define WHITE_COLS 2

#define FREE_TO_CHANGE_COL -1
#define FREE_TO_CHANGE_BW -2
#define INIT_COL_PLANES FREE_TO_CHANGE_COL


#endif//NRMCONSTANTS_H


