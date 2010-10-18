#ifndef NEWNETWORKDIALOG_H
#define NEWNETWORKDIALOG_H

//Qt includes
#include <QDialog>
#include <QLineEdit>

namespace spikestream {

	/*! Dialog for adding a new network to the database. */
	class NewNetworkDialog : public QDialog {
		Q_OBJECT

		public:
			NewNetworkDialog(QWidget* parent=0);
			NewNetworkDialog(const QString& name, const QString& description, QWidget* parent=0);
			~NewNetworkDialog();
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

#endif//NEWNETWORKDIALOG_H

