#include "RGBColor.h"
using namespace spikestream;

//Declare and initialize static colors
RGBColor RGBColor::BLACK = RGBColor(0.0f, 0.0f, 0.0f);


/*! Default constructor */
RGBColor::RGBColor(){
	this->red = 0.0f;
	this->green = 0.0f;
	this->blue = 0.0f;
}


/*! Standard constructor */
RGBColor::RGBColor(float red, float green, float blue){
	this->red = red;
	this->green = green;
	this->blue = blue;
}


/*! Copy constructor */
RGBColor::RGBColor(const RGBColor& rgbColor){
	this->red = rgbColor.red;
	this->green = rgbColor.green;
	this->blue = rgbColor.blue;
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Assignment operator */
RGBColor& RGBColor::operator=(const RGBColor& rhs){
	if(this == &rhs)
	return *this;

	this->red = rhs.red;
	this->green = rhs.green;
	this->blue = rhs.blue;

	return *this;
}


/*! Sets the colour. */
void RGBColor::set(float red, float green, float blue){
	this->red = red;
	this->green = green;
	this->blue = blue;
}

