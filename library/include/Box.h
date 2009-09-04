#ifndef BOX_H
#define BOX_H

namespace spikestream {

    class Box {
	public:
	    Box();
	    Box(float x1, float y1, float z1,float x2, float y2, float z2);
	    Box(const Box& box);
	    ~Box();
	    void translate(float dx, float dy, float dz);

	    float getX1() {return x1;}
	    float getY1() {return y1;}
	    float getZ1() {return z1;}
	    float getX2() {return x2;}
	    float getY2() {return y2;}
	    float getZ2() {return z2;}

	private:
	    //========================  VARIABLES  =========================
	    float x1;
	    float y1;
	    float z1;
	    float x2;
	    float y2;
	    float z2;
    };

}

#endif//BOX_H

