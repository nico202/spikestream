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
#include "BusyDialog.h"
#include "Debug.h"

//Other includes
#include <iostream>
//Added by qt3to4:
#include <QLabel>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
using namespace std;


/*! Constructor. */
BusyDialog::BusyDialog(QWidget *parent, const QString &title) : QDialog(parent){
	//Set the caption
	setCaption(title);

	//Create box to organise vertical layout of dialog
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 10, 10, "Main vertical Box");

	//Add a label to hold the busy message
	Q3HBoxLayout* labelLayout = new Q3HBoxLayout();
	busyMessage = new QLabel("Untitled", this);
	labelLayout->addStretch(2);
	labelLayout->addWidget(busyMessage);
	labelLayout->addStretch(2);
	mainVerticalBox->addLayout(labelLayout);
}


/*! Destructor. */
BusyDialog::~BusyDialog(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING BUSY DIALOG"<<endl;
	#endif//MEMORY_DEBUG
}


//---------------------------------------------------------------------
//------------------------ PUBLIC METHODS -----------------------------
//---------------------------------------------------------------------

/*! Shows the dialog with the text. */
void BusyDialog::showDialog(const QString &text){
	busyMessage->setText(text);
	this->show();
}


