#ifndef COMPLEX_H
#define COMPLEX_H

//Qt includes
#include <QList>
#include <QString>

namespace spikestream {

    /*! Holds information about a complex */
    class Complex {
	public:
	    Complex(unsigned int id, unsigned int timeStep, double phi, QList<unsigned int>& neuronIDs);
	    Complex();
	    Complex(const Complex& cmplx);
	    ~Complex();
	    Complex& operator=(const Complex& rhs);

	    unsigned int getID() const { return id; }
	    unsigned int getTimeStep() const { return timeStep; }
	    double getPhi() const { return phi; }
	    const QList<unsigned int>& getNeuronIDs() const { return neuronIDs; }
	    QString getNeuronIDString() const;

	    void setNeuronIDs(QList<unsigned int>& neuronIDs) { this->neuronIDs = neuronIDs; }
	    void setPhi(double phi) { this->phi = phi; }

	private:
	    //===================  VARIABLES  ===================
	    unsigned int id;
	    unsigned int timeStep;
	    double phi;
	    QList<unsigned int> neuronIDs;



    };

}

#endif//COMPLEX_H

