#ifndef POINT3D_H
#define POINT3D_H

//Qt includes
#include <QString>

namespace spikestream{

    class Point3D {
	public:
	    Point3D();
	    Point3D(float xPos, float yPos, float zPos);
	    Point3D(const Point3D& point);
	    virtual ~Point3D();
	    Point3D& operator=(const Point3D& point);
	    bool operator==(const Point3D& point);
	    bool operator!=(const Point3D& point);
	    float getXPos() { return xPos; }
	    float getYPos() { return yPos; }
	    float getZPos() { return zPos; }
	    void setXPos(float xPos) { this->xPos = xPos; }
	    void setYPos(float yPos) { this->yPos = yPos; }
	    void setZPos(float zPos) { this->zPos = zPos; }
	    QString toString() const;

	private:
	    //=================  VARIABLES  ==================
	    float xPos;
	    float yPos;
	    float zPos;
    };

}

#endif//POINT3D_H


