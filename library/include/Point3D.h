#ifndef POINT3D_H
#define POINT3D_H

namespace spikestream {
    class Point3D {
	public:
	    Point3D(float xPos, float yPos, float zPos){ this->xPos = xPos; this->yPos = yPos; this->zPos = zPos;}

	    float xPos;
	    float yPos;
	    float zPos;
    };
}

#endif//POINT3D_H
