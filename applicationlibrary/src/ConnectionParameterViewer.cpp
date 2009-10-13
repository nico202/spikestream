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
#include "ConnectionParameterViewer.h"
#include "Debug.h"

//Qt includes
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <string>
#include <mysql++.h>
using namespace std;
using namespace mysqlpp;

//Local debug defines
//#define XML_PARSE_DEBUG


/*! Constructor. */
ConnectionParameterViewer::ConnectionParameterViewer(QWidget* parent, DBInterface* netDBInter, unsigned int cGrpID) : QDialog(parent){
	//Store references and information passed in constructor
	networkDBInterface = netDBInter;
	connGrpID = cGrpID;

	//Set the caption
	QString captionStr = "Connection Group Parameters [";
	captionStr += QString::number(connGrpID);
	captionStr += "]";
	this->setCaption(captionStr);

	//Create a vertical box to organise the layout
	mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "vertical");

	//Error state is used to track any errors during start up
	errorState = false;

	//Make it non modal
	this->setModal(false);
	
	//Load up XML string from database and parse it if it is not null
	string paramXmlString("");
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT Parameters FROM ConnectionGroups WHERE ConnGrpID = "<<connGrpID;
                StoreQueryResult paramRes = query.store();
		Row paramRow(*paramRes.begin());//ConnGrpID is unique so will be only one row.
		paramXmlString = (std::string) paramRow["Parameters"];
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionParameterViewer: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query selecting connection parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Parameter Error", errorString);
		errorState = true;
		return;
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ConnectionParameterViewer: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown selecting network parameters: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Parameter Error", errorString);
		errorState = true;
		return;
    }

	QXmlSimpleReader xmlReader;
	QXmlInputSource xmlInput;
	xmlReader.setContentHandler(this);
	xmlReader.setErrorHandler(this);

	if(paramXmlString != "NULL" && paramXmlString != ""){
                xmlInput.setData(QString(paramXmlString.data()));
		xmlReader.parse(xmlInput);
		if(parseError){
			QMessageBox::critical( 0, "Error Parsing Connection Parameters", parseErrorString);
			errorState = true;
			return;
		}
	}
	else{
		currentHBox = new Q3HBoxLayout();
		currentHBox->addWidget(new QLabel("No parameters stored.", this));
		currentHBox->addStretch(5);
		mainVerticalBox->addLayout(currentHBox);
	}
	mainVerticalBox->addStretch(5);

	//Set up buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	QPushButton *okButton = new QPushButton("Ok", this);
	buttonBox->addWidget(okButton);
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));

	QPushButton *cancelButton = new QPushButton("Cancel", this);	
	buttonBox->addWidget(cancelButton);
	connect (cancelButton, SIGNAL(clicked()), this, SLOT(cancelButtonPressed()));
	mainVerticalBox->addLayout(buttonBox);

	//Make viewer visible		
	this->resize(400, 200);
}


/*! Destructor. */
ConnectionParameterViewer::~ConnectionParameterViewer(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING CONNECTION PARAMETER VIEWER"<<endl;
	#endif//MEMORY_DEBUG
}


//-------------------------------------------------------------------------
//-------------------------   PUBLIC METHODS  -----------------------------
//-------------------------------------------------------------------------

bool ConnectionParameterViewer::loadError(){
	return errorState;
}


//-------------------------------------------------------------------------
//------------------------- PROTECTED METHODS -----------------------------
//-------------------------------------------------------------------------

/*! Called when parser encounters characters. */
bool ConnectionParameterViewer::characters(const QString& chars){
	#ifdef XML_PARSE_DEBUG
		cout<<"Characters: "<<chars<<" in element "<<currentElement<<endl;
	#endif//XML_PARSE_DEBUG
	if(currentElement == "description"){
		currentHBox->addWidget(new QLabel(chars, this));
	}
	else if(currentElement == "value"){
		currentHBox->addWidget(new QLabel(chars, this));
		currentHBox->addStretch(5);
		mainVerticalBox->addLayout(currentHBox);
	}
	else if(currentElement == "min_delay"){
		currentHBox = new Q3HBoxLayout();
		currentHBox->addWidget(new QLabel("Delay range: Min", this));
		currentHBox->addWidget(new QLabel(chars, this));
	}
	else if(currentElement == "max_delay"){
		currentHBox->addWidget(new QLabel("Max", this));
		currentHBox->addWidget(new QLabel(chars, this));
		currentHBox->addStretch(5);
		mainVerticalBox->addLayout(currentHBox);
	}
	else{
		parseError = true;
		parseErrorString += "Unrecognized element.";
		cerr<<"ConnectionParameterViewer: UNRECOGNIZED ELEMENT"<<endl;
	}
	return true;
}


/*! Called when the parser encounters the end of an element. */
bool ConnectionParameterViewer::endElement( const QString&, const QString&, const QString& qName){
	#ifdef XML_PARSE_DEBUG
		cout<<"End element: "<<qName<<endl;
	#endif//XML_PARSE_DEBUG
	return true;
}


/*! Called when the parser generates an error. */
bool ConnectionParameterViewer::error ( const QXmlParseException& parseEx){
	cerr<<"ConnectionParameterViewer: PARSING ERROR"<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Returns a default error string. */
QString ConnectionParameterViewer::errorString (){
	return QString("ConnectionParameterViewer: Default error string");

}


/*! Called when the parser generates a fatal error. */
bool ConnectionParameterViewer::fatalError ( const QXmlParseException& parseEx){
	cerr<<"ConnectionParameterViewer: PARSING FATAL ERROR"<<endl;
	parseError = true;
	parseErrorString += parseEx.message();
	return true;
}


/*! Called when parser reaches the start of the document. */
bool ConnectionParameterViewer::startDocument(){
	parseError = false;
	parseErrorString = "";
	return true;
}


/*! Called when parser reaches the start of an element. */
bool ConnectionParameterViewer::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes&){
	#ifdef XML_PARSE_DEBUG
		cout<<"Start element: "<<qName<<endl;
	#endif//XML_PARSE_DEBUG
	currentElement = qName;
	if(currentElement == "parameter"){
		currentHBox = new Q3HBoxLayout();
	}
	return true;
}


/*! Called when the parser generates a warning. */
bool ConnectionParameterViewer::warning ( const QXmlParseException& ){
	cerr<<"ConnectionParameterViewer: PARSING WARNING"<<endl;
	return true;
}


//-------------------------------------------------------------------------
//-------------------------   PRIVATE SLOTS   -----------------------------
//-------------------------------------------------------------------------

/*! Hides the dialog. */
void ConnectionParameterViewer::cancelButtonPressed(){
	this->accept();
}


/*! Hides the dialog. */
void ConnectionParameterViewer::okButtonPressed(){
	this->accept();
}



