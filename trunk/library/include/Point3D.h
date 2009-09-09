#ifndef POINT3D_H
#define POINT3D_H

namespace spikestream {
    class Point3D {
	public:
	    Point3D(float x, float y, float z){ this->x = x; this->y = y; this->z = z;}

	    float x;
	    float y;
	    float z;
    };
}

#endif//POINT3D_H
