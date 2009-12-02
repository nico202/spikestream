#ifndef NETWORKDISPLAY_H
#define NETWORKDISPLAY_H

//SpikeStream includes
#include "RGBColor.h"
using namespace spikestream;

//Qt includes
#include <QHash>
#include <QList>
#include <QMutex>

namespace spikestream {

    /*! Holds display information about the current network in global scope,
	including which neuron and connection groups are displayed, colours etc. */
    class NetworkDisplay : public QObject {
	Q_OBJECT

	public:
	    NetworkDisplay();
	    ~NetworkDisplay();
	    void clearNeuronColorMap();
	    bool connectionGroupVisible(unsigned int connGrpID);
	    QHash<unsigned int, RGBColor*>& getNeuronColorMap() { return *neuronColorMap; }
	    RGBColor* getDefaultNeuronColor() { return &defaultNeuronColor; }
	    RGBColor* getFiringNeuronColor() { return &firingNeuronColor; }
	    RGBColor* getNegativeConnectionColor(){ return &negativeConnectionColor; }
	    RGBColor* getPositiveConnectionColor(){ return &positiveConnectionColor; }
	    QList<unsigned int> getVisibleConnectionGroupIDs() { return connGrpDisplayMap.keys(); }
	    QList<unsigned int> getVisibleNeuronGroupIDs() { return neurGrpDisplayMap.keys(); }

	    unsigned int getZoomNeuronGroupID() { return zoomNeuronGroupID; }
	    bool isZoomEnabled();
	    int getZoomStatus () { return zoomStatus; }
	    void setZoom(unsigned int neurGrpID, int status);


	    void lockMutex();
	    bool neuronGroupVisible(unsigned int neurGrpID);
	    void setConnectionGroupVisibility(unsigned int conGrpID, bool visible);
	    void setDefaultNeuronColor(RGBColor& color) { defaultNeuronColor = color; }
	    void setNeuronColorMap(QHash<unsigned int, RGBColor*>* newMap);
	    void setNeuronGroupVisibility(unsigned int neurGrpID, bool visible);
	    void setVisibleConnectionGroupIDs(const QList<unsigned int>& connGrpIDs);
	    void setVisibleNeuronGroupIDs(const QList<unsigned int>& neurGrpIDs);
	    void unlockMutex();

	    //=========================  VARIABLES  =========================
	    /*! Zoom disabled */
	    static const int NO_ZOOM = 0;

	    /*! Zoom to side of network or neuron group */
	    static const int ZOOM_SIDE = 1;

	    /*! Zoom to top of neuron group */
	    static const int ZOOM_ABOVE = 2;


	signals:
	    void networkDisplayChanged();

	public slots:
	    void networkChanged();

	private slots:
	    void clearZoom();

	private:
	    //========================  VARIABLES  ========================
	    /*! Mutex preventing changes to the display whilst it is being rendered. */
	    QMutex mutex;

	    /*! Visible connection groups */
	    QHash<unsigned int, bool> connGrpDisplayMap;

	    /*! Visible neuron groups */
	    QHash<unsigned int, bool> neurGrpDisplayMap;

	    /*! Map specifying the color of each neuron */
	    QHash<unsigned int, RGBColor*>* neuronColorMap;

	    /*! Default color of a neuron */
	    RGBColor defaultNeuronColor;

	    /*! Postive connection color */
	    RGBColor positiveConnectionColor;

	    /*! Negative connection color */
	    RGBColor negativeConnectionColor;

	    /*! Color of a firing neuron during archive playback */
	    RGBColor firingNeuronColor;

	    /*! Map recording the addresses of default colors, so that they don't get deleted
		when the neuron color map is cleared. */
	    QHash<RGBColor*, bool> defaultColorMap;

	    unsigned int zoomNeuronGroupID;
	    int zoomStatus;


	    //=========================  METHODS  =========================


    };

}

#endif//NETWORKDISPLAY_H

