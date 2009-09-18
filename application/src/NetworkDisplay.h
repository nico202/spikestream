#ifndef NETWORKDISPLAY_H
#define NETWORKDISPLAY_H

//SpikeStream includes
#include "RGBColor.h"
using namespace spikestream;

//Qt includes
#include <QHash>
#include <QList>

namespace spikestream {

    /*! Holds display information about the current network in global scope,
	including which neuron and connection groups are displayed, colours etc. */
    class NetworkDisplay : public QObject {
	Q_OBJECT

	public:
	    NetworkDisplay();
	    ~NetworkDisplay();
	    QHash<unsigned int, RGBColor*>& getNeuronColorMap() { return neuronColorMap; }
	    RGBColor getDefaultNeuronColor() { return defaultNeuronColor; }
	    QList<unsigned int> getVisibleConnectionGroupIDs() { return connGrpDisplayMap.keys(); }
	    QList<unsigned int> getVisibleNeuronGroupIDs() { return neurGrpDisplayMap.keys(); }
	    void setDefaultNeuronColor(RGBColor& color) { defaultNeuronColor = color; }
	    void setVisibleConnectionGroupIDs(const QList<unsigned int>& connGrpIDs);
	    void setVisibleNeuronGroupIDs(const QList<unsigned int>& neurGrpIDs);


	signals:
	    void networkDisplayChanged();

	public slots:
	    void networkChanged();

	private:
	    //========================  VARIABLES  ========================
	    /*! List of visible connection groups */
	    QHash<unsigned int, bool> connGrpDisplayMap;

	    /*! List of visible neuron groups */
	    QHash<unsigned int, bool> neurGrpDisplayMap;

	    /*! Map specifying the color of each neuron */
	    QHash<unsigned int, RGBColor*> neuronColorMap;

	    /*! Default color of a neuron */
	    RGBColor defaultNeuronColor;

	    //=========================  METHODS  =========================


    };

}

#endif//NETWORKDISPLAY_H

