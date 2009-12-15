//SpikeStream includes
#include "StateBasedPhiAnalysisDaoDuck.h"

//Qt includes
#include <QDebug>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
StateBasedPhiAnalysisDaoDuck::StateBasedPhiAnalysisDaoDuck(){
}


/*! Destructor */
StateBasedPhiAnalysisDaoDuck::~StateBasedPhiAnalysisDaoDuck(){
}


/*! Instead of storing complex in database the complex is stored in a list for testing */
void StateBasedPhiAnalysisDaoDuck::addComplex(unsigned int, int, QList<unsigned int>& neuronIDList, double phi){
    Complex cmplx;
    cmplx.setNeuronIDs(neuronIDList);
    cmplx.setPhi(phi);
    complexList.append(cmplx);
}

/*! Resets information stored in the class */
void StateBasedPhiAnalysisDaoDuck::reset(){
    complexList.clear();
}

