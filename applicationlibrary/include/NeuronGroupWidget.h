#ifndef NEURONGROUPWIDGET_H
#define NEURONGROUPWIDGET_H

//Qt includes
#include <QWidget>

namespace spikestream {

	class NeuronGroupWidget : public QWidget {
		Q_OBJECT

		public:
			NeuronGroupWidget(QWidget* parent = 0);
			~NeuronGroupWidget();


		private slots:
			void addNeurons();

	};

}

#endif//NEURONGROUPWIDGET_H


