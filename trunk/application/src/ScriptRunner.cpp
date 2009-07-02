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
#include "ScriptRunner.h"
#include "Debug.h"

//Qt includes
#include <qlayout.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
using namespace std;


/*! Constructor. */
ScriptRunner::ScriptRunner(QWidget* parent, QString scriptName, QString mainWorkDir) : QDialog(parent, "NeurParamDlg"){
	workingDirectory += mainWorkDir + "/scripts/";

	//Set up QProcess for running the script
	process = new Q3Process();

	/* Set the working directory to the scripts directory so that scripts 
		can easily invoke other scripts. */
	process->setWorkingDirectory(QDir(workingDirectory));

	//Connect process to slots
	connect( process, SIGNAL(readyReadStdout()), this, SLOT(readFromStdout()) );
	connect( process, SIGNAL(readyReadStderr()), this, SLOT(readFromStderr()) );
	connect( process, SIGNAL(processExited()), this, SLOT(processExited()) );

	//Create box to organise vertical layout of dialog
	Q3VBoxLayout *mainVerticalBox = new Q3VBoxLayout(this, 5, 10, "Main vertical Box");

	//Add a text area to hold the output from the script
	scriptMessages = new Q3TextEdit(this);
	mainVerticalBox->addWidget(scriptMessages);

	//Set up ok and cancel buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	okButton = new QPushButton("Ok", this, "okButton");
	QPushButton *stopButton = new QPushButton("Stop", this, "stopButton");	
	buttonBox->addWidget(okButton);
	buttonBox->addWidget(stopButton);
	mainVerticalBox->addLayout(buttonBox);
	
	connect (okButton, SIGNAL(clicked()), this, SLOT(okButtonPressed()));
	connect (stopButton, SIGNAL(clicked()), this, SLOT(stopButtonPressed()));

	//Set dialog size
	this->resize(600, 300);

	//Run the script
	runScript(scriptName);
}


/*! Destructor. */
ScriptRunner::~ ScriptRunner(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING SCRIPT RUNNER"<<endl;
	#endif//MEMORY_DEBUG

	delete process;
}


//---------------------------------------------------------------------------
//-------------------------- PUBLIC METHODS ---------------------------------
//---------------------------------------------------------------------------

/*! Returns true if there has been an error running the script. */
bool ScriptRunner::processError(){
	return procErr;
}


/*! Returns true if process is running. */
bool ScriptRunner::processRunning(){
	return process->isRunning();
}


//---------------------------------------------------------------------------
//------------------------------- SLOTS -------------------------------------
//---------------------------------------------------------------------------

/*! Slot for when ok button is pressed. */
void ScriptRunner::okButtonPressed(){
	accept();
}


/*! Slot called when process exits. */
void ScriptRunner::processExited(){
	#ifdef RUN_SCRIPT_DEBUG
		cout<<"Process finished"<<endl;
	#endif//RUN_SCRIPT_DEBUG
	scriptMessages->insertParagraph("--------------- Script finished ---------------", -1);
	okButton->setEnabled(true);
}


/*! Prints out error from process. */
void ScriptRunner::readFromStderr(){
	while(process->canReadLineStderr()){
		QString processString = process->readLineStderr();
                cerr<<processString.toStdString()<<endl;
		scriptMessages->insertParagraph(processString, -1);
	}
}


/*! Slot called when there is something to read from standard out. */
void ScriptRunner::readFromStdout(){
	while(process->canReadLineStdout()){
		QString processString = process->readLineStdout();
		#ifdef RUN_SCRIPT_DEBUG
			cout<<processString<<endl;
		#endif//RUN_SCRIPT_DEBUG
		scriptMessages->insertParagraph(processString, -1);
	}
}


/*! Slot for when stop button is pressed. */
void ScriptRunner::stopButtonPressed(){
	stopScript();
	okButton->setEnabled(true);
}


//---------------------------------------------------------------------------
//-------------------------- PRIVATE METHODS --------------------------------
//---------------------------------------------------------------------------

/*! Overloaded run script method. */
void ScriptRunner::runScript(const char* scriptName){
	runScript(QString(scriptName));
}


/*! Runs the named script. */
void ScriptRunner::runScript(QString scriptName){
	//Initialise error
	procErr = false;

	//Set the caption
	setCaption(scriptName);

	//Check script file exists
	if(!QFile::exists(workingDirectory + scriptName)){
	//	errorString += ;
		scriptMessages->insertParagraph("Script file does not exist", -1);
		procErr = true;
		return;
	}

	//Set up the process
	process->clearArguments();	
	process->addArgument(workingDirectory + scriptName);

	//Start the process
	if ( process->start() ) {
		scriptMessages->insertParagraph("---------------- Script started ----------------", -1);
		okButton->setEnabled(false);
	}
	else{
		scriptMessages->insertParagraph("Script will not start.", -1);
		procErr = true;
	}
}


/*! Runs a script with a given set of parameters. */
void ScriptRunner::runScript(QString scriptName, QStringList scriptParameters){
	//Initialise error
	procErr = false;

	//Set the caption
	setCaption(scriptName);

	//Check script file exists
	if(!QFile::exists(workingDirectory + scriptName)){
		scriptMessages->insertParagraph("Script file does not exist", -1);
		procErr = true;
		return;
	}

	//Set up the process
	process->clearArguments();	
	process->addArgument(workingDirectory + scriptName);
	for(unsigned int i=0; i<scriptParameters.size(); ++i)
		process->addArgument(scriptParameters[i]);

	//Start the process
	if ( process->start() ) {
		scriptMessages->insertParagraph("---------------- Script started ----------------", -1);
		okButton->setEnabled(false);
	}
	else{
		scriptMessages->insertParagraph("Script will not start.", -1);
		procErr = true;
	}
}


/*! Politely ask process to quit and kill it if it does not respond. */
bool ScriptRunner::stopScript(){
	//First politely ask process to quit
	process->tryTerminate();

	int timeoutCount = 0;
	while (timeoutCount < 100){
		if(!process->isRunning())
			return true;
		else{
			usleep(10000);//Sleep for 10 ms
		}
		++timeoutCount;
	}
	//Try to kill process
	process->kill();

	//Wait to see if it has stopped
	timeoutCount = 0;
	while (timeoutCount < 10){
		if(!process->isRunning())
			return true;
		else{
			usleep(10000);//Sleep for 10 ms
		}
		++timeoutCount;
	}
	return false;
}



