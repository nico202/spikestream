//SpikeStream includes
#include "PerformanceTimer.h"
using namespace spikestream;

//Other includes
#include <iostream>
using namespace std;


/*! Constructor */
PerformanceTimer::PerformanceTimer(){
	timerStart = QDateTime::currentDateTime();
}


/*! Destructor. */
PerformanceTimer::~PerformanceTimer(){
}


/*----------------------------------------------------------*/
/*------                PUBLIC METHODS                ------*/
/*----------------------------------------------------------*/

/*! Stores the current date time in seconds */
void PerformanceTimer::start(){
	timerStart = QDateTime::currentDateTime();
}


/*! Returns the number of seconds that have elapsed since the timer started */
void PerformanceTimer::printTime(const QString& taskName){
	QDateTime currDateTime = QDateTime::currentDateTime();

	//Later in the same day, can just subtract the milliseconds
	if(timerStart.daysTo(currDateTime) == 0){
		cout<<taskName.toStdString()<<" took "<<( timerStart.time().msecsTo(currDateTime.time()) )<<" ms."<<endl;
	}
	//1 day difference - still output result in milliseconds in case timing takes place over midnight
	else if (timerStart.daysTo(currDateTime) == 1){
		//Calculate the time left in day1
		unsigned int msecDay1 = timerStart.time().hour()*3600000 + timerStart.time().minute()*60000 + timerStart.time().second()*1000 + timerStart.time().msec();
		msecDay1 -= 86400000;

		//Calculate msec elapsed in day2
		unsigned int msecDay2 = currDateTime.time().hour()*3600000 + currDateTime.time().minute()*60000 + currDateTime.time().second()*1000 + currDateTime.time().msec();
		cout<<taskName.toStdString()<<" took "<<(msecDay1 + msecDay2)<<" ms."<<endl;
	}
	//More than 1 day difference - output result in seconds
	else{
		cout<<taskName.toStdString()<<" took "<<( currDateTime.toTime_t() - timerStart.toTime_t() )<<" seconds."<<endl;
	}
}


