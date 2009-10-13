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

#ifndef CONNECTIONPARAMETERVIEWER_H
#define CONNECTIONPARAMETERVIEWER_H

//SpikeStream includes
#include "DBInterface.h"

//Qt includes
#include <qxml.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

//----------------------- Connection Parameter Viewer -----------------------
/*! Displays the parameters of connection group. */
/* FIXME IN THE FUTURE THIS SHOULD ENABLE THE USER TO CREATE A NEW CONNECTION
	GROUP BASED ON THESE PARAMETERS.
   FIXME THIS SHOULD DELETE ITSELF WHEN THE WINDOW IS CLOSED. */
//--------------------------------------------------------------------------

class ConnectionParameterViewer : public QDialog, public QXmlDefaultHandler {
	Q_OBJECT

	public:
		ConnectionParameterViewer(QWidget* parent, DBInterface* netDBInter, unsigned int cGrpID);
		~ConnectionParameterViewer();
		bool loadError();


	protected:
		//Parsing methods inherited from QXmlDefaultHandler
		bool characters ( const QString & ch );
    	bool endElement( const QString&, const QString&, const QString& );
    	bool startDocument();
    	bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );
		
		//Error handling methods inherited from QXmlDefaultHandler
		bool error ( const QXmlParseException & exception );
		QString errorString();
		bool fatalError ( const QXmlParseException & exception );
		bool warning ( const QXmlParseException & exception );


	private slots:
		void okButtonPressed();
		void cancelButtonPressed();


	private:
		//=========================== VARIABLES =============================
		/*! Database connection.*/
		DBInterface* networkDBInterface;

		/*! The connection group whose parameters are being displayed.*/
		unsigned int connGrpID;

		/*! Stores the current element. */
		QString currentElement;

		/*! Records whether there has been an error during parsing. */
		bool parseError;

		/*! Error messages generated during parsing. */
		QString parseErrorString;

		/*! The current horizontal box.*/
		Q3HBoxLayout* currentHBox;

		/*! The main vertical box for the widget.*/
		Q3VBoxLayout* mainVerticalBox;

		/*! Records if there has been an error during startup.*/
		bool errorState;


		//=========================== METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ConnectionParameterViewer (const ConnectionParameterViewer&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ConnectionParameterViewer operator = (const ConnectionParameterViewer&);

};

#endif//CONNECTIONPARAMETERVIEWER_H












