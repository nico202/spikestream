#ifndef NEURONPARAMETERSMODEL_H
#define NEURONPARAMETERSMODEL_H

//SpikeStream includes
#include "NeuronGroupInfo.h"
#include "ParameterInfo.h"

//Qt includes
#include <QAbstractTableModel>

namespace spikestream {

	/*! Model of the current neuron parameters for the current network.
		Synchronized with the database.
		NOTE: Not thread safe.
		NOTE: Can only handle parameters or a single type of neuron. */
	class NeuronParametersModel : public QAbstractTableModel  {
		Q_OBJECT

		public:
			NeuronParametersModel();
			~NeuronParametersModel();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);


		private slots:
			void networkChanged();


		private:
			//======================  VARIABLES  =======================
			/*! List containing the information about the neuron groups in the current network
				This list is automatically refreshed when the network changes. */
			QList<NeuronGroupInfo> neurGrpInfoList;

			/*! The type of neuron being handled by this model */
			unsigned int neuronTypeID;

			/*! Map linking each neuron group ID with the parameter map for that neuron group*/
			QHash<unsigned int, QHash<QString, double> > parameterMap;

			/*! List of available parameters.
				NOTE: This class can only handle parameters for a single neuron type. */
			QList<ParameterInfo> parameterInfoList;


			//=======================  METHODS  =========================
			void checkParameters();
			void loadParameters();

	};

}

#endif//NEURONPARAMETERSMODEL_H
