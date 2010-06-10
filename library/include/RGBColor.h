#ifndef RGBCOLOR_H
#define RGBCOLOR_H

namespace spikestream {

	/*! A simple RGB color class. */
	class RGBColor {
		public:
			//=======================  METHODS  ========================
			RGBColor();
			RGBColor(float red, float green, float blue);
			RGBColor(const RGBColor& rgbColor);
			RGBColor& operator=(const RGBColor& rhs);
			void set(float red, float green, float blue);

			//=======================  COLORS  =========================
			static RGBColor BLACK;

			//======================  VARIABLES  =======================
			float red;
			float green;
			float blue;
		};

}

#endif//RGBCOLOR_H

