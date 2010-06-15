#ifndef NRMDATASET_H
#define NRMDATASET_H

//Qt includes
#include <QList>

namespace spikestream {

	/*! Set of NRM firing data */
    class NRMDataSet {
		public:
			NRMDataSet();
			~NRMDataSet();
				void addData(unsigned char* data);
			QList<unsigned char*> getDataList() { return dataList; }
			void print();
			void reset();
			int size() { return dataList.size(); }

			/*! The width of the data */
			int width;

			/*! The height of the data */
			int height;

			/*! The type of colour information in the data - see NRM code for further details. */
			int colorPlanes;

		private:
			//===========================  VARIABLES  ==========================
			/*! The data */
			QList<unsigned char*> dataList;
    };
}

#endif//NRMDATASET_H

