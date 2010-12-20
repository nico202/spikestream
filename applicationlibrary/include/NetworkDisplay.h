#ifndef NETWORKDISPLAY_H
#define NETWORKDISPLAY_H

//SpikeStream includes
#include "ConfigLoader.h"
#include "Connection.h"
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
			void addDefaultColor(RGBColor* color);
			void addHighlightNeurons(const QList<unsigned int>& neuronIDs, RGBColor* color);
			void addHighlightNeurons(const QHash< unsigned int, RGBColor*>& colorMap);
			void removeHighlightNeurons(const QList<unsigned int>& neuronIDs);
			bool connectionGroupVisible(unsigned int connGrpID);
			unsigned int getConnectionMode() { return connectionMode; }
			unsigned getConnectionQuality() { return connectionQuality; }
			unsigned getConnectionThinningThreshold();
			QHash<unsigned int, RGBColor*>& getNeuronColorMap() { return *neuronColorMap; }
			RGBColor* getDefaultNeuronColor() { return &defaultNeuronColor; }
			RGBColor* getDefaultNeuronColorFullRender() { return &defaultNeuronColorFullRender; }
			RGBColor* getArchiveFiringNeuronColor() { return &archiveFiringNeuronColor; }
			RGBColor* getSimulationFiringNeuronColor() { return &simulationFiringNeuronColor; }
			RGBColor* getHighlightNeuronColor() { return &highlightNeuronColor; }
			float getMinimumConnectionRadius() { return minimumConnectionRadius; }
			RGBColor& getSingleNeuronColor() { return singleNeuronColor; }
			RGBColor& getToNeuronColor() { return toNeuronColor; }
			RGBColor* getNegativeConnectionColor(){ return &negativeConnectionColor; }
			float getNeuronTransparency() { return neuronTransparency; }
			RGBColor* getPositiveConnectionColor(){ return &positiveConnectionColor; }
			unsigned getSphereQuality() { return sphereQuality; }
			float getSphereRadius() { return sphereRadius; }
			float getVertexSize() { return vertexSize; }
			QList<unsigned int> getVisibleConnectionGroupIDs() { return connGrpDisplayMap.keys(); }
			QList<Connection*>& getVisibleConnectionsList() { return visibleConnectionsList; }
			QList<unsigned int> getVisibleNeuronGroupIDs() { return neurGrpDisplayMap.keys(); }
			float getWeightRadiusFactor() { return weightRadiusFactor; }
			unsigned getWeightRenderMode() { return weightRenderMode; }
			unsigned int getZoomNeuronGroupID() { return zoomNeuronGroupID; }
			int getZoomStatus () { return zoomStatus; }
			bool isFullRenderMode() { return fullRenderMode; }
			bool isDrawAxes() { return drawAxes; }
			bool isZoomEnabled();
			void loadDisplaySettings(ConfigLoader* configLoader);
			void lockMutex();
			bool neuronGroupVisible(unsigned int neurGrpID);
			void disableWeightRender();
			void renderTempWeights();
			void renderCurrentWeights();
			void setConnectionModeFlag(unsigned int flag);
			void unsetConnectionModeFlag(unsigned int flag);
			void setConnectionGroupVisibility(unsigned int conGrpID, bool visible);
			void setDefaultNeuronColor(RGBColor& color) { defaultNeuronColor = color; }
			void setFullRenderMode(bool fullRenderMode);
			void setNeuronColorMap(QHash<unsigned int, RGBColor*>* newMap);
			void setNeuronGroupVisibility(unsigned int neurGrpID, bool visible);
			void setNeuronTransparency(float neuronTransparency);
			void setVisibleConnectionGroupIDs(const QList<unsigned int>& connGrpIDs, bool emitDisplayChangedSignal = true);
			void setVisibleNeuronGroupIDs(const QList<unsigned int>& neurGrpIDs, bool emitDisplayChangedSignal = true);
			void setZoom(unsigned int neurGrpID, int status);
			void unlockMutex();

			unsigned int getSingleNeuronID() { return singleNeuronID; }
			unsigned int getToNeuronID() { return toNeuronID; }
			void setSelectedNeuronID(unsigned int id, bool ctrlBtnDown=false);

			void clearWeightFiltering();
			void showNegativeConnections();
			void showPositiveConnections();

			void clearDirectionFiltering();
			void showFromConnections();
			void showToConnections();


			//=========================  VARIABLES  =========================
			/*! Zoom disabled */
			static const int NO_ZOOM = 0;

			/*! Zoom to side of network or neuron group */
			static const int ZOOM_SIDE = 1;

			/*! Zoom to top of neuron group */
			static const int ZOOM_ABOVE = 2;


		signals:
			void networkDisplayChanged();
			void neuronGroupDisplayChanged();
			void visibleConnectionsChanged();


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

			/*! Default color of a neuron in full render mode */
			RGBColor defaultNeuronColorFullRender;

			/*! Color of single neuron whose connections are being shown */
			RGBColor singleNeuronColor;

			/*! Color of to neuron when showing connections between neurons*/
			RGBColor toNeuronColor;

			/*! Postive connection color */
			RGBColor positiveConnectionColor;

			/*! Negative connection color */
			RGBColor negativeConnectionColor;

			/*! Color of a firing neuron during archive playback */
			RGBColor archiveFiringNeuronColor;

			/*! Color of a firing neuron during simulation */
			RGBColor simulationFiringNeuronColor;

			/*! Default color for highlighting a neuron */
			RGBColor highlightNeuronColor;

			/*! Map recording the addresses of default colors, so that they don't get deleted
			when the neuron color map is cleared. */
			QHash<RGBColor*, bool> defaultColorMap;

			/*! When zoom status is not NO_ZOOM this variable holds the neuron group that is zoomed.
			When set to zero the viewer zooms to show the whole network.*/
			unsigned int zoomNeuronGroupID;

			/*! Sets whether viewer should zoom in on a specific neuron group */
			int zoomStatus;

			/*! Connection mode. Each bit in this unsigned integer codes a different aspect
			of the mode. Need to AND it with the different modes to see if they are set or not */
			unsigned int connectionMode;

			/*! Weight render mode. Each bit controls different aspects of the weight rendering */
			unsigned weightRenderMode;

			/*! Neuron id used in SHOW_SINGLE_NEURON_CONNECTIONS mode */
			unsigned int singleNeuronID;

			/*! To neuron id used in SHOW_BETWEEN_NEURON_CONNECTIONS mode */
			unsigned int toNeuronID;

			/*! In full render mode, neurons are drawn as lighted spheres */
			bool fullRenderMode;

			/*! Size of vertices in the 3D Network Viewer */
			float vertexSize;

			/*! Whether axes should be drawn or not */
			bool drawAxes;

			/*! Radius of sphere in full render mode */
			float sphereRadius;

			/*! Quality of sphere in full render mode. 3-10 are reasonable settings. */
			unsigned sphereQuality;

			/*! Transparency of the neuron. Should be between 0 and 1. */
			float neuronTransparency;

			/*! Connection groups with more visible connections than this threshold
				in fast render mode are not shown by default. The thinning thresholds
				interact with this parameter.*/
			int connectionVisibilityThreshold_fast;

			/*! Connection groups with more visible connections than this threshold
				in full render mode are not shown by default. The thinning thresholds
				interact with this parameter.*/
			int connectionVisibilityThreshold_full;

			/*! Weight is multiplied by this this factor to get the radius of the connections
				when drawn in full render mode with weights rendered as thickness of connections*/
			float weightRadiusFactor;

			/*! Minimum radius of a connection when in full render with weights */
			float minimumConnectionRadius;

			/*! Quality of cones used to render a weighted connection */
			unsigned connectionQuality;

			/*! Threshold above which thinning of the drawn connections takes place
				in fast render mode. */
			unsigned connectionThinningThreshold_fast;

			/*! Threshold above which thinning of the drawn connections takes place
				in full render mode. */
			unsigned connectionThinningThreshold_full;

			/*! List of the currently visible connections -
				Loaded by NetworkViewer and used when in single connectiosn mode */
			QList<Connection*> visibleConnectionsList;


			//=========================  METHODS  =========================
			void checkConnectionModeFlag(unsigned int flag);
			void checkWeightRenderFlag(unsigned int flag);
			void clearNeuronColorMap();
			void setDefaultVisibleConnectionGroupIDs();
			void setWeightRenderFlag(unsigned flag);
			void unsetWeightRenderFlag(unsigned flag);
	};

}

#endif//NETWORKDISPLAY_H

