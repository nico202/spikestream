#ifndef SPIKESTREAMVOLUME_H
#define SPIKESTREAMVOLUME_H



class SpikeStreamVolume {
    public:
	SpikeStreamVolume(const float& x1, const float& y1, const float& z1,const float& x2, const float& y2, const float& z2);
	~SpikeStreamVolume();
	void translate(const float& dx, const float& dy, const float& dz);

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


#endif//SPIKESTREAMVOLUME_H

