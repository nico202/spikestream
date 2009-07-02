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
#include "NameDialog.h"
#include "Debug.h"
#include "GlobalVariables.h"

//Qt includes
#include <qregexp.h>
#include <qvalidator.h>
#include <q3accel.h>
#include <qpushbutton.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
NameDialog::NameDialog(QString name, QWidget *parent) : QDialog(parent) {
	//Create a validators
	QRegExp regExp( "([0-9]|[A-Z]|[a-z]|_|\\s){1,50}" );
    QValidator* nameValidator = new QRegExpValidator(regExp, this);

	//Create box to organise vertical layout of dialog
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "Main vertical Box");
	
	//Add text field to receive name
	nameText = new QLineEdit(name, this);
	nameText->setValidator(nameValidator);
	mainVerticalBox->addWidget(nameText);

	//Set up ok and cancel buttons
	Q3HBoxLayout *okCanButtonBox = new Q3HBoxLayout();
	QPushButton *okPushButton = new QPushButton("Ok", this, "okButton");
	QPushButton *cancelPushButton = new QPushButton("Cancel", this, "cancelButton");	
	okCanButtonBox->addWidget(okPushButton);
	okCanButtonBox->addWidget(cancelPushButton);
	mainVerticalBox->addLayout(okCanButtonBox);
	
	connect (okPushButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (cancelPushButton, SIGNAL(clicked()), this, SLOT(reject()));

	//Set up accelerator for return button
	Q3Accel *returnAccel = new Q3Accel( this );
    returnAccel->connectItem( returnAccel->insertItem( Qt::Key_Enter ), this, SLOT(okButtonPressed()));
}


/*! Destructor. */
NameDialog::~ NameDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING NAME DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//--------------------------------------------------------------------------------
//------------------------------ PUBLIC METHODS ----------------------------------
//--------------------------------------------------------------------------------

/*! Returns the name that has been entered
	Checks that archive name is sensible and it is the right length
		These are the same checks as are run in Simulation Widget where
		the name is first set. Any changes there should be matched here. */
QString NameDialog::getName(){
	QString name = nameText->text();

	//Check length of name is ok
	if(name.length() == 0)
		name = "Untitled";
	else if(name.length() > MAX_DATABASE_NAME_LENGTH)
		name.truncate(MAX_DATABASE_NAME_LENGTH);

	//Return the name
	return name;
}


//-------------------------------------------------------------------------------
//----------------------------- SLOTS -------------------------------------------
//-------------------------------------------------------------------------------

/*! Slot for when ok button is pressed. */
void NameDialog::okButtonPressed(){
	accept();
}


