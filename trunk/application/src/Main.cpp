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
#include "SpikeStreamMainWindow.h"
#include "SpikeStreamApplication.h"

#include "testQApplication.h"

#include <QApplication>
 #include <QPushButton>

//-------------------------- Main ------------------------------------------
/*! Main method for simulator that launches the application. */
//--------------------------------------------------------------------------

int main( int argc, char ** argv ) {


     //QApplication spikeStrApp(argc, argv);
   /*  SpikeStreamApplication spikeStrApp(argc, argv);
    //testQApplication spikeStrApp(argc, argv);
     QPushButton hello("Hello world11!");
     hello.resize(100, 30);

     hello.show();
     return spikeStrApp.exec();

*/

	//Create QApplication
        SpikeStreamApplication spikeStrApp(argc, argv);

	//Start up main window of application
        SpikeStreamMainWindow *spikeStrMainWin = new SpikeStreamMainWindow(&spikeStrApp);
        spikeStrMainWin->setCaption( "SpikeStream" );
        spikeStrMainWin->show();

	//Listen for window closing events
        spikeStrApp.connect( &spikeStrApp, SIGNAL(lastWindowClosed()), &spikeStrApp, SLOT(quit()) );

	//Execute application
        return spikeStrApp.exec();
}

