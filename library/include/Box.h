#ifndef BOX_H
#define BOX_H

//SpikeStream includes
#include "Point3D.h"

//Qt includes
#include <QList>
#include <QString>

namespace spikestream {

	/*! Holds information about a three dimensional box with a few basic manipulation functions. */
    class Box {
		public:
			Box();
			Box(float x1, float y1, float z1, float x2, float y2, float z2);
			Box(const Box& box);
			Box& operator=(const Box& rhs);
			~Box();
			Point3D centre() const;
			bool contains(const Point3D& point) const;
			static Box getEnclosingBox(const QList<Box>& boxList);
			float getX1() const { return x1; }
			float getX2() const { return x2; }
			float getY1() const { return y1; }
			float getY2() const { return y2; }
			float getZ1() const { return z1; }
			float getZ2() const { return z2; }
			float getWidth() const;
			float getLength() const;
			float getHeight() const;
			void expand_percent(float percent);
			bool intersects(const Box& box) const;
			QString toString() const;
			void translate(float dx, float dy, float dz);

			float x1;
			float y1;
			float z1;
			float x2;
			float y2;
			float z2;

		private:
			//========================  VARIABLES  =========================

    };

}

#endif//BOX_H

