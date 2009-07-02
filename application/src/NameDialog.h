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

#ifndef NAMEDIALOG_H
#define NAMEDIALOG_H

//Qt includes
#include <qdialog.h>
#include <qlineedit.h>


//------------------------- Name Dialog ------------------------------------
/*! Simple dialog used for gathering a name.*/
//--------------------------------------------------------------------------

class NameDialog : public QDialog {
	Q_OBJECT

	public:
		NameDialog(QString name, QWidget *parent);
		~NameDialog();
		QString getName();


	private slots:
		void okButtonPressed();


	private:
		//============================== VARIABLES ===========================
		//Qt variables
		QLineEdit *nameText;


		//============================== METHODS =============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NameDialog (const NameDialog&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NameDialog operator = (const NameDialog&);

};


#endif //NAMEDIALOG_H


