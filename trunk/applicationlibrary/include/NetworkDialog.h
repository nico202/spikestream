#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

//Qt includes
#include <QDialog>
#include <QLineEdit>

namespace spikestream {

	/*! Dialog for adding a new network to the database. */
	class NetworkDialog : public QDialog {
		Q_OBJECT

		public:
			NetworkDialog(QWidget* parent=0);
			NetworkDialog(const QString& name, const QString& description, QWidget* parent=0);
			~NetworkDialog();
			QString getName();
			QString getDescription();


		private slots:
			void okButtonPressed();
			void cancelButtonPressed();


		private:
			//====================  VARIABLES ====================
			/*! Where user enters the name of the network */
			QLineEdit* nameLineEdit;

			/*! Where user enters a description of the network */
			QLineEdit* descLineEdit;

			/*! Controls whether network is added to database. */
			bool addNetworkToDatabase;


			//====================  METHODS  ====================
			void buildGUI(const QString& name, const QString& description);
	};
}

#endif//NETWORKDIALOG_H

