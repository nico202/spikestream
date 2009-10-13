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

#ifndef SCRIPTRUNNER_H
#define SCRIPTRUNNER_H

//Qt includes
#include <q3process.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdialog.h>
#include <q3textedit.h>
#include <qpushbutton.h>

 
//--------------------------- Script Runner --------------------------------
/*! Runs scripts in the /scripts directory. Can pass parameters to them
	but script is assumed to be self contained. Use the program 'expect'
	within the script if interaction is required with an external program. */
//--------------------------------------------------------------------------

class ScriptRunner : public QDialog {
	Q_OBJECT

	public: 
		ScriptRunner(QWidget* parent, QString scriptName, QString mainWorkDir);
		~ScriptRunner();
		bool processError();
		bool processRunning();


	private slots:
		void okButtonPressed();
		void processExited();
		void readFromStderr();
		void readFromStdout();
		void stopButtonPressed();


	private:
		//========================== VARIABLES ===========================
		/*! The main directory of the application. This should contain
			containing the scripts in a scripts subdirectory.*/
		QString workingDirectory;

		/*! Flag to record errors executing the script.*/
		bool procErr;

		/*! The process that executes the script.*/
		Q3Process* process;

		/*! QTextEdit to display the output from the script.*/
		Q3TextEdit* scriptMessages;

		/*! Hides the dialog. This is disabled whilst the script is running.*/
		QPushButton* okButton;


		//========================= METHODS ===============================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		ScriptRunner (const ScriptRunner&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		ScriptRunner operator = (const ScriptRunner&);

		void runScript(const char* scriptName);
		void runScript(QString scriptName);
		void runScript(QString scriptName, QStringList scriptParameters);
		bool stopScript();

};


#endif//SCRIPTRUNNER_H


