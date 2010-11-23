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
			QList<NeuronGroup*> getExcitatoryNeuronGroups() { return excitNeurGrpList; }
			QList<NeuronGroup*> getInhibitoryNeuronGroups() { return inhibNeurGrpList; }

		signals:
			void progress(int stepsCompleted, int totalSteps, QString message);


		private:
			//======================  VARIABLES  =========================
			/*! List of excitatory neuron groups - stored for use creating connections. */
			QList<NeuronGroup*> excitNeurGrpList;

			/*! List of inhibitory neuron groups - stored for use creating connections. */
			QList<NeuronGroup*> inhibNeurGrpList;

			/*! List of node names */
			QList<QString> nodeNameList;

			/*! Default neuron parameters. The key is the neuron type ID. The value is the map of default parameters. */
			QHash<unsigned, QHash<QString, double> > defaultParameterMaps;

			/*! Number of neurons in each node */
			unsigned numberNeuronsPerNode;

			/*! Proportion of excitatory neurons in each group. */
			double proportionExcitatoryNeurons;

			/*! Factor by which space enclosing neuron groups is reduced to create space around neuron groups.
				Set to 1 to make neuron groups adjacent; set to a low number to increase spacing. */
			float nodeSpacingFactor;


			//======================  METHODS  ===========================
			void addNeurons(NeuronGroup* exNeurGrp, NeuronGroup* inibNeurGrp, float neurGrpDimen, const Point3D& cartCoord);
			QList<Point3D> getCartesianCoordinates(const QString& coordinatesFile);
			float getNeuronGroupDimension(const QList<Point3D>& cartesianCoordinatesList);
			Point3D getTalairachCoordinate(const QString& line);
			void loadDefaultParameters();
			void loadNodeNames(const QString& nodeNameFileLocation);
			QList<Point3D> loadTalairachCoordinates(const QString& coordinatesFileLocation);
			void storeParameters(QHash<QString, double>& parameterMap);
	};

}

#endif//NEURONGROUPBUILDER_H
