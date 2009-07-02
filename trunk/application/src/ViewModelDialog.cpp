/***************************************************************************
 *   SpikeStream Application                                               *
 *   Copyright (C) 2007 by David Gamez                                     *
 *   david@davidgamez.eu                                                   *
 *   Version 0.1                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//SpikeStream includes
#include "ViewModelDialog.h"
#include "Debug.h"

//Qt includes
#include <qlayout.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>


/*! Constructor. */
ViewModelDialog::ViewModelDialog(QWidget* parent, QString* nwModelStr) : QDialog(parent) {
	//Store reference to network model string
	networkModelString = nwModelStr;

	//Create vertical layout to organise widget
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "vertical1");


	//Add text area to display model
	modelTextEdit = new Q3TextEdit(this);
	modelTextEdit->setTextFormat(Qt::PlainText);
	modelTextEdit->setText(*networkModelString);
	modelTextEdit->setReadOnly(true);
	mainVerticalBox->addWidget(modelTextEdit);


	//Set up ok and cancel buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton* okButton = new QPushButton("Ok", this, "okButton");
	QPushButton* cancelButton = new QPushButton("Cancel", this, "cancelButton");	
	buttonBox->addWidget(okButton);
	buttonBox->addWidget(cancelButton);
	mainVerticalBox->addLayout(buttonBox);
	
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));

	//Set dialog size
	this->resize(600, 400);
}


/*! Destructor. */
ViewModelDialog::~ViewModelDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING VIEW MODEL DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//-----------------------------------------------------------------------------
//----------------            PUBLIC METHODS            -----------------------
//-----------------------------------------------------------------------------

/*! Reloads the network model. */
void ViewModelDialog::reloadText(){
	modelTextEdit->setText(*networkModelString);
}


//-----------------------------------------------------------------------------
//----------------             PRIVATE SLOTS            -----------------------
//-----------------------------------------------------------------------------

/*! Hides the dialog when the cancel button is pressed. */
void ViewModelDialog::cancelButtonClicked(){
	this->hide();
}


/*! Hides the dialog when the ok button is pressed. */
void ViewModelDialog::okButtonClicked(){
	this->hide();
}




