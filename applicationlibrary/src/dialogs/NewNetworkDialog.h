#ifndef NEWNETWORKDIALOG_H
#define NEWNETWORKDIALOG_H

//Qt includes
#include <QDialog>
#include <QLineEdit>

namespace spikestream {

	class NewNetworkDialog : public QDialog {
		Q_OBJECT

		public:
			NewNetworkDialog(QWidget* parent=0);
			~NewNetworkDialog();

		private slots:
			void okButtonPressed();
			void cancelButtonPressed();

		private:
			//====================  VARIABLES ====================
			/*! Where user enters the name of the network */
			QLineEdit* nameLineEdit;

			/*! Where user enters a description of the network */
			QLineEdit* descLineEdit;


	};
}

#endif//NEWNETWORKDIALOG_H

