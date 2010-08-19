//SpikeStream includes
#include "ViewParametersDialog.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>
#include <QPushButton>

/*! Constructor */
ViewParametersDialog::ViewParametersDialog(QHash<QString, double> parameterMap, QWidget *parent) : QDialog(parent){
	QGridLayout* gridLayout = new QGridLayout(this);

	//Add parameters
	int cntr = 0;
	for(QHash<QString, double>::iterator iter = parameterMap.begin(); iter != parameterMap.end(); ++iter){
		gridLayout->addWidget( new QLabel( iter.key() ), cntr, 0 );
		gridLayout->addWidget( new QLabel( QString::number( iter.value() ) ), cntr, 1 );
		++cntr;
	}

	//Add buttons
	QPushButton* cancelButton = new QPushButton("Cancel");
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	gridLayout->addWidget(cancelButton, cntr, 0);
	QPushButton* okButton = new QPushButton("Ok");
	connect (okButton, SIGNAL(clicked()), this, SLOT(accept()));
	gridLayout->addWidget(okButton, cntr, 1);
}


/*! Destructor */
ViewParametersDialog::~ViewParametersDialog(){
}

