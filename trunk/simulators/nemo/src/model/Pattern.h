#ifndef PATTERN_H
#define PATTERN_H

//SpikeStream includes
#include "Box.h"
#include "Point3D.h"

//Qt includes
#include <QString>

namespace spikestream {

	/*! Holds information about and data associated with a particular pattern.
		Patterns can be injected into neuron groups in Nemo.
		Data in the pattern is stored as a list of boxes and points. */
	class Pattern {
		public:
			Pattern();
			Pattern(const Pattern& patt);
			~Pattern();
			Pattern& operator=(const Pattern& rhs);
			bool operator==(const Pattern& rhs);
			void addBox(const Box& box);
			void addPoint(const Point3D& point);
			bool contains(const Point3D& point);
			Pattern getAlignedPattern(const Box& box) const;
			QString getName(){ return name; }
			void reset();
			void setName(const QString& name) { this->name = name; }
			void translate(float dx, float dy, float dz);

		private:
			//===================  VARIABLES  ===================
			/*! The name of the pattern. */
			QString name;

			/*! List of boxes defining the pattern. */
			QList<Box> boxList;

			/*! List of points defining the pattern. */
			QList<Point3D> pointList;
	};

}

#endif//PATTERN_H
