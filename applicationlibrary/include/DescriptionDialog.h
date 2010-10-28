#ifndef DESCRIPTIONDIALOG_H
#define DESCRIPTIONDIALOG_H

//Qt includes
#include <QDialog>
#include <QLineEdit>

namespace spikestream {

	/*! Dialog for editing a description. */
	class DescriptionDialog : public QDialog {
		Q_OBJECT

		public:
			DescriptionDialog(const QString& description, QWidget* parent=0);
			~DescriptionDialog();
			QString getDescription();


		private slots:
			void okButtonPressed();
			void cancelButtonPressed();


		private:
			//====================  VARIABLES ====================
			/*! Where user enters a description. */
			QLineEdit* descLineEdit;

			//====================  METHODS  ====================
			void buildGUI(const QString& description);
	};
}

#endif//DESCRIPTIONDIALOG_H

