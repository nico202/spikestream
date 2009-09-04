#ifndef RGBCOLOR_H
#define RGBCOLOR_H

namespace spikestream {

    /*! A simple RGB color struct used to hold the highlight colour.
	    Use this instead of QColor to minimise space. */
    struct RGBColor {
	    float red;
	    float green;
	    float blue;
    };

}

#endif//RGBCOLOR_H

