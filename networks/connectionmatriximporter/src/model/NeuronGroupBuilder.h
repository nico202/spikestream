#ifndef NEURONGROUPBUILDER_H
#define NEURONGROUPBUILDER_H

//SpikeStream includes
#include "Network.h"
#include "Point3D.h"

//Qt includes
#include <QObject>

namespace spikestream {

	/*! Adds neurons to the supplied network using the loaded talairach coordinates. */
	class NeuronGroupBuilder : public QObject {
		Q_OBJECT

		public:
			NeuronGroupBuilder();
			~NeuronGroupBuilder();
			void addNeuronGroups(Network* network, const QString& coordinatesFilePath, const QString& nodeNamesFilePath, QHash<QString, double> parameterMap);
			QList<QString> getNodeNames() { return nodeNamesList; }

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);


		private:
			//======================  VARIABLES  =========================
			/*! List of node names - stored here so that it can be passed to the connection builder. */
			QList<QString> nodeNamesList;

			//======================  METHODS  ===========================
			void addNeurons(NeuronGroup* exNeurGrp, NeuronGroup* inibNeurGrp, unsigned numNeurPerGroup, double proportionExcitatoryNeur, float neurGrpDimen, const Point3D& cartCoord);
			QList<Point3D> getCartesianCoordinates(const QString& coordinatesFile);
			float getNeuronGroupDimension(const QList<Point3D>& cartesianCoordinatesList);
			Point3D getTalairachCoordinate(const QString& line);
			void loadNodeNames(const QString& nodeNameFileLocation);
			QList<Point3D> loadTalairachCoordinates(const QString& coordinatesFileLocation);
	};

}

#endif//NEURONGROUPBUILDER_H
