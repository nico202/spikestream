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
#include "HighlightDialog.h"
#include "Utilities.h"

//Qt includes
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qicon.h>
#include <qmessagebox.h>
#include <qcolordialog.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include <iostream>
using namespace std;


/*! Constructor. */
HighlightDialog::HighlightDialog(QWidget* parent, NetworkViewer_V2* nwViewer) : QDialog(parent) {
	//Store references
	networkViewer = nwViewer;

	//Create a vertical box to organise the layout
	Q3VBoxLayout* mainVBox = new Q3VBoxLayout(this, 5, 10, "vertical");

	//Add controls to select the colour of the highlight
	Q3HBoxLayout* colorBox = new Q3HBoxLayout();
	colorBox->addWidget(new QLabel("Highlight colour:", this));
	colorPixmap = new QPixmap(40, 20);
	highlightColor = Qt::yellow;
	colorPixmap->fill(highlightColor);
	colorButt = new QPushButton("", this);
	colorButt->setPixmap(*colorPixmap);
	colorButt->setBaseSize(40, 20);
	colorButt->setMinimumSize(40, 20);
	colorButt->setMaximumSize(40, 20);
	colorButt->setFlat(true);
	connect (colorButt, SIGNAL(clicked()), this, SLOT(colorButtonPressed()));
	colorBox->addWidget(colorButt);
	colorBox->addStretch(5);
	mainVBox->addLayout(colorBox);

	//Add text area for the neuron ids.
	neuronIDText = new Q3TextEdit(this);
	mainVBox->addWidget(neuronIDText);

	//Set up buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *addHighlightButt = new QPushButton("Add Highlight", this);
	buttonBox->addWidget(addHighlightButt);
	connect (addHighlightButt, SIGNAL(clicked()), this, SLOT(addHighlightButtonPressed()));

	QPushButton *clearButton = new QPushButton("Clear Highlights", this);
	buttonBox->addWidget(clearButton);
	connect (clearButton, SIGNAL(clicked()), this, SLOT(clearButtonPressed()));

	QPushButton *closeButton = new QPushButton("Close", this);	
	buttonBox->addWidget(closeButton);
	connect (closeButton, SIGNAL(clicked()), this, SLOT(closeButtonPressed()));
	mainVBox->addLayout(buttonBox);

	//Set size	
	this->resize(600, 400);

}


/*! Destructor. */
HighlightDialog::~HighlightDialog(){

}


//-------------------------------------------------------------------------
//---------                  PRIVATE SLOTS                        ---------
//-------------------------------------------------------------------------

/*! Adds a highlight to the network viewer. */
void HighlightDialog::addHighlightButtonPressed(){
	//Convert the colour to a suitable form for open gl painting
	RGBColor* tmpColor = new RGBColor;
	tmpColor->red = highlightColor.red() / 255.0;
	tmpColor->green = highlightColor.green() / 255.0;
	tmpColor->blue = highlightColor.blue() / 255.0;

	//Extract the neuron ids and add them to the network viewer
	QString neurIDStr = neuronIDText->text();

	//Get rid of all white space
	for(unsigned int i=0; i<neurIDStr.length(); ++i){
		if(neurIDStr.at(i).isSpace()){
			neurIDStr.remove(i, 1);
			--i;
		}
	}
	QStringList tempStringList = QStringList::split(",", neurIDStr);
	
	//Add neuron IDs to the highlight map in the network viewer
	try{
		unsigned int tmpNeuronID;
		for(unsigned int i=0; i< tempStringList.size(); ++i){
			tmpNeuronID = Utilities::getUInt(tempStringList[i].ascii());
			networkViewer->addHighlight(tmpNeuronID, tmpColor);
		}
	}
	catch(std::exception& ex){// Catch-all for std exceptions
		QString errorString = "HighlightDialog: STD EXCEPTION THROWN EXTRACTING NEURON IDS: \"";
		errorString += ex.what();
		errorString += "\"";
		showError(errorString);
	}

	//Refresh the display
	networkViewer->refresh();
}


/*! Clears all of the current highlights. */
void HighlightDialog::clearButtonPressed(){
	networkViewer->clearHighlights();
	neuronIDText->setText("");
}


/*! Closes the dialog. */
void HighlightDialog::closeButtonPressed(){
	this->hide();
}


/*! Sets the highlight colour. */
void HighlightDialog::colorButtonPressed(){
	QColor tmpColor = QColorDialog::getColor(highlightColor, this, "color dialog" );
	if ( tmpColor.isValid() ) {
		colorPixmap->fill(tmpColor);
		colorButt->setPixmap(*colorPixmap);
		highlightColor = tmpColor;
	}
}


//-------------------------------------------------------------------------
//---------                  PRIVATE METHODS                      ---------
//-------------------------------------------------------------------------

/*! Displays an error message. */
void HighlightDialog::showError(const QString& errMsg){
        cerr<<errMsg.toStdString()<<endl;
	QMessageBox::critical(this, "Highlight Dialog Error", errMsg);
}


