#ifndef CONNECTIONWIDGET_V2_H
#define CONNECTIONWIDGET_V2_H

//Qt includes
#include <QWidget>

namespace spikestream {

	class ConnectionWidget_V2 : public QWidget {
	Q_OBJECT

	public:
		ConnectionWidget_V2(QWidget* parent = 0);
		~ConnectionWidget_V2();


	private slots:
		void addConnections();

	};

}

#endif//CONNECTIONWIDGET_V2_H



