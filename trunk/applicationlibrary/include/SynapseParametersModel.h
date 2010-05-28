#ifndef SYNAPSEPARAMETERSMODEL_H
#define SYNAPSEPARAMETERSMODEL_H

//SpikeStream includes
#include "ConnectionGroupInfo.h"
#include "ParameterInfo.h"

//Qt includes
#include <QAbstractTableModel>

namespace spikestream {

	/*! Model of the current synapse parameters for the current network.
		Synchronized with the database.
		NOTE: Not thread safe.
		NOTE: Can only handle parameters or a single type of synapse. */
	class SynapseParametersModel : public QAbstractTableModel  {
		Q_OBJECT

		public:
			SynapseParametersModel(unsigned int synapseTypeID);
			~SynapseParametersModel();
			int columnCount(const QModelIndex& parent = QModelIndex()) const;
			QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
			ConnectionGroupInfo getConnectionGroupInfo(int row);
			QList<ParameterInfo> getParameterInfoList();
			QHash<QString, double> getParameterValues(int row);
			QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
			void reload();
			int rowCount(const QModelIndex& parent = QModelIndex()) const;
			bool setData(const QModelIndex& index, const QVariant& value, int role=Qt::EditRole);


		private slots:
			void networkChanged();


		private:
			//======================  VARIABLES  =======================
			/*! List containing the information about the connection groups in the current network
				of the type supported by this class.
				This list is automatically refreshed when the network changes. */
			QList<ConnectionGroupInfo> conGrpInfoList;

			/*! The type of synapse being handled by this model */
			unsigned int synapseTypeID;

			/*! Map linking each connection group ID with the parameter map for that connection group*/
			QHash<unsigned int, QHash<QString, double> > parameterMap;

			/*! List of available parameters.
				NOTE: This class can only handle parameters for a single connection type. */
			QList<ParameterInfo> parameterInfoList;


			//=======================  METHODS  =========================
			void checkParameters();
			void loadParameters();

	};

}

#endif//SYNAPSEPARAMETERSMODEL_H
