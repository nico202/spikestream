#include "SpikeStreamVolume.h"


SpikeStreamVolume::SpikeStreamVolume(const float& x1, const float& y1, const float& z1,const float& x2, const float& y2, const float& z2){
    this->x1 = x1;
    this->y1 = y1;
    this->z1 = z1;
    this->x2 = x2;
    this->y2 = y2;
    this->z2 = z2;
}

SpikeStreamVolume::~SpikeStreamVolume(){
}


void SpikeStreamVolume::translate(const float& dx, const float& dy, const float& dz){
    x1 += dx;
    x2 += dx;
    y1 += dy;
    y2 += dy;
    z1 += dz;
    z2 += dz;
}


