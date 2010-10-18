#ifndef CONNECTIONGROUPDIALOG_H
#define CONNECTIONGROUPDIALOG_H

//SpikeStream includes
#include "ConnectionGroupInfo.h"

//Qt includes
#include <QDialog>
#include <QLineEdit>

namespace spikestream {

	/*! Dialog for editing the properties of a connection group. */
	class ConnectionGroupDialog : public QDialog {
		Q_OBJECT

		public:
			ConnectionGroupDialog(const ConnectionGroupInfo& conGrpInfo, QWidget* parent=0);
			~ConnectionGroupDialog();


		private slots:
			void okButtonPressed();
			void cancelButtonPressed();


		private:
			//====================  VARIABLES ====================
			/*! Where user enters a description of the connection group */
			QLineEdit* descLineEdit;

			/*! Information about the connection group being edited. */
			ConnectionGroupInfo connectionGroupInfo;


			//====================  METHODS  ====================
			void buildGUI();
			QString getDescription();
	};
}

#endif//CONNECTIONGROUPDIALOG_H
