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
#ifndef HIGHLIGHTDIALOG_H
#define HIGHLIGHTDIALOG_H

//SpikeStream includes
#include "NetworkViewer.h"

//Qt includes
#include <qdialog.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <q3textedit.h>
#include <qcolor.h>

//------------------------ Highlight Dialog -------------------------------
/*! Controls highlights in the Network Viewer. */
//--------------------------------------------------------------------------

class HighlightDialog : public QDialog {
	Q_OBJECT

	public: 
		HighlightDialog(QWidget* parent, NetworkViewer* nwViewer);
		~HighlightDialog();


	private slots:
		void addHighlightButtonPressed();
		void clearButtonPressed();
		void closeButtonPressed();
		void colorButtonPressed();

	private:
		//========================= VARIABLES ==========================
		/*! The class that displays the highlighted neurons.*/
		NetworkViewer* networkViewer;

		/*! The current highlight colour. */
		QColor highlightColor;

		//Qt variables
		Q3TextEdit* neuronIDText;
		QPixmap* colorPixmap;
		QPushButton* colorButt;


		//========================= METHODS ============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		HighlightDialog(const HighlightDialog&);
		/*! Declare assignment private so it cannot be used inadvertently.*/
		HighlightDialog operator = (const HighlightDialog&);

		void showError(const QString& errMsg);

};


#endif//HIGHLIGHTDIALOG_H

