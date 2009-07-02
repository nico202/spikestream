#/***************************************************************************
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
#include "ArchiveStatistic.h"
#include "Debug.h"

//Other includes
#include <iostream>
using namespace std;


//Local version of debug defines
//#define ARCHIVE_STATISTICS_DEBUG


//Declare static variables
dense_hash_map<unsigned int, bool, hash<unsigned int> >* ArchiveStatistic::neuronIDHashMap;


//-----------------------------------------------------------------------------
//----------------             ARCHIVE STATISTIC          ---------------------
//-----------------------------------------------------------------------------

/*! Constructor. */
ArchiveStatistic::ArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal){
	//Store references and the ID
	firingNeuronCount = fNeurCount;
	firingNeuronTotal = fNeurTotal;

	//Set the default type
	type = NEURON_ID;
}


/*! Destructor. */
ArchiveStatistic::~ArchiveStatistic(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING ARCHIVE STATISTIC"<<endl;
	#endif//MEMORY_DEBUG
}


/*! Returns the ID given to the class. */
unsigned int ArchiveStatistic::getID(){
	return ID;
}


/*! Returns the type of the class. */
unsigned int ArchiveStatistic::getType(){
	return type;
}


/*! Default implementation of this method, which gets called if it is not overridden
	by a subclass. */
void ArchiveStatistic::recalculate(){
}


/*! Default implementation of this method, which gets called if it is not overridden
	by a subclass. */
void ArchiveStatistic::recalculate(unsigned int){
}


/*! Default implementation of this method, which gets called if it is not overridden
	by a subclass. */
void ArchiveStatistic::recalculateNeuronGrp(){
}


/*! Resets the firing neuron count. Generally called when starting to load the 
	data for a timestep. */
void ArchiveStatistic::resetFiringNeuronCount(){
	*firingNeuronCount = 0;
}


/*! Resets the firing neuron total. Generally called when rewinding the archive
	or after editing the statistics. */
void ArchiveStatistic::resetFiringNeuronTotal(){
	*firingNeuronTotal = 0;
}


/*! Sets the id. */
void ArchiveStatistic::setID(unsigned int id){
	ID = id;
}


//-----------------------------------------------------------------------------
//---------             NEURON GROUP ARCHIVE STATISTIC          ---------------
//-----------------------------------------------------------------------------

/*! Constructor. */
NeuronGrpArchiveStatistic::NeuronGrpArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int nGrpID) : ArchiveStatistic(fNeurCount, fNeurTotal) {
	type = NEURON_GROUP;
	neuronGrpID = nGrpID;
}


/*! Destructor. */
NeuronGrpArchiveStatistic::~NeuronGrpArchiveStatistic(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING NEURON GROUP ARCHIVE STATISTIC"<<endl;
	#endif//MEMORY_DEBUG
}


/*! Accessor for the neuron group id. */
unsigned int NeuronGrpArchiveStatistic::getNeuronGrpID(){
	return neuronGrpID;
}


/*! Recalculates the statistics being gathered by this class. The neuron group
	handling should be carried out by the networkDataXmlHandler, so will assume
	that we don't need to check that neuron group id is in range. This method should
	be called for each neuron found in the neuron group. */
void NeuronGrpArchiveStatistic::recalculateNeuronGrp(){
	++(*firingNeuronCount);
	++(*firingNeuronTotal);

	#ifdef ARCHIVE_STATISTICS_DEBUG
		cout<<"NeuronGrpArchiveStatistic: Recalculated neuron group. firingNeuronCount = "<<*firingNeuronCount<<"; firingNeuronTotal = "<<*firingNeuronTotal<<endl;
	#endif//ARCHIVE_STATISTICS_DEBUG
}


/*! Outputs a string that would have been used to create this class. In this
	case, no string was used, so returns an error message. */
QString NeuronGrpArchiveStatistic::toQString(){
	return QString("Neuron ID Error! This class was not created with a string.");//This class is not created with a string so return an error.
}

//-----------------------------------------------------------------------------
//-----------             RANGE ARCHIVE STATISTIC                --------------
//-----------------------------------------------------------------------------

/*! Constructor. */
RangeArchiveStatistic::RangeArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int rLow, unsigned int rHigh) : ArchiveStatistic(fNeurCount, fNeurTotal) {
	rangeLow = rLow;
	rangeHigh = rHigh;
}


/*! Destructor. */
RangeArchiveStatistic::~RangeArchiveStatistic(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING RANGE ARCHIVE STATISTIC"<<endl;
	#endif//MEMORY_DEBUG
}


/*! Examines the hash map for neuron ids that are in range. */
void RangeArchiveStatistic::recalculate(){
	for(unsigned int i=rangeLow; i <= rangeHigh; ++i){
		if(neuronIDHashMap->count(i)){
			++(*firingNeuronCount);
			++(*firingNeuronTotal);
		}
	}
	#ifdef ARCHIVE_STATISTICS_DEBUG
		cout<<"RangeArchiveStatistic: Recalculated neuron group. firingNeuronCount = "<<*firingNeuronCount<<"; firingNeuronTotal = "<<*firingNeuronTotal<<endl;
	#endif//ARCHIVE_STATISTICS_DEBUG
}


/*! Outputs a string that would have been used to create this class. */
QString RangeArchiveStatistic::toQString(){
	QString tempStr(QString::number(rangeLow));
	tempStr += "-";
	tempStr += QString::number(rangeHigh);
	return tempStr;
}


//-----------------------------------------------------------------------------
//-------------             AND ARCHIVE STATISTIC                --------------
//-----------------------------------------------------------------------------

/*! Constructor. */
AndArchiveStatistic::AndArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int fNeurID, unsigned int sNeurID) : ArchiveStatistic(fNeurCount, fNeurTotal) {
	firstNeuronID = fNeurID;
	secondNeuronID = sNeurID;
}


/*! Destructor. */
AndArchiveStatistic::~AndArchiveStatistic(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING AND ARCHIVE STATISTIC"<<endl;
	#endif//MEMORY_DEBUG
}


/*! Examines the hash map to see if both neuron IDs are present. */
void AndArchiveStatistic::recalculate(){
	if(neuronIDHashMap->count(firstNeuronID) && neuronIDHashMap->count(secondNeuronID)){
		++(*firingNeuronCount);
		++(*firingNeuronTotal);
	}
	#ifdef ARCHIVE_STATISTICS_DEBUG
		cout<<"AndArchiveStatistic: Recalculated neuron group. firingNeuronCount = "<<*firingNeuronCount<<"; firingNeuronTotal = "<<*firingNeuronTotal<<endl;
	#endif//ARCHIVE_STATISTICS_DEBUG
}


/*! Outputs a string that would have been used to create this class. */
QString AndArchiveStatistic::toQString(){
	QString tempStr(QString::number(firstNeuronID));
	tempStr += "&";
	tempStr += QString::number(secondNeuronID);
	return tempStr;
}


//-----------------------------------------------------------------------------
//-------------              OR ARCHIVE STATISTIC                --------------
//-----------------------------------------------------------------------------

/*! Constructor. */
OrArchiveStatistic::OrArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int fNeurID, unsigned int sNeurID) : ArchiveStatistic(fNeurCount, fNeurTotal) {
	firstNeuronID = fNeurID;
	secondNeuronID = sNeurID;
}


/*! Destructor. */
OrArchiveStatistic::~OrArchiveStatistic(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING OR ARCHIVE STATISTIC"<<endl;
	#endif//MEMORY_DEBUG
}


/*! Examines the hash map to see if the first or second neuron IDs are present. */
void OrArchiveStatistic::recalculate(){
	if(neuronIDHashMap->count(firstNeuronID) || neuronIDHashMap->count(secondNeuronID)){
		++(*firingNeuronCount);
		++(*firingNeuronTotal);
	}
	#ifdef ARCHIVE_STATISTICS_DEBUG
		cout<<"OrArchiveStatistic: Recalculated neuron group. firingNeuronCount = "<<*firingNeuronCount<<"; firingNeuronTotal = "<<*firingNeuronTotal<<endl;
	#endif//ARCHIVE_STATISTICS_DEBUG
}


/*! Outputs a string that would have been used to create this class. */
QString OrArchiveStatistic::toQString(){
	QString tempStr(QString::number(firstNeuronID));
	tempStr += "|";
	tempStr += QString::number(secondNeuronID);
	return tempStr;
}


//-----------------------------------------------------------------------------
//-----------             NEURON ID ARCHIVE STATISTIC              ------------
//-----------------------------------------------------------------------------

/*! Constructor. */
NeuronIDArchiveStatistic::NeuronIDArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int neurID) : ArchiveStatistic(fNeurCount, fNeurTotal) {
	neuronID = neurID;
}


/*! Destructor. */
NeuronIDArchiveStatistic::~NeuronIDArchiveStatistic(){
	#ifdef MEMORY_DEBUG
		cout<<"DESTROYING NEURON ID ARCHIVE STATISTIC"<<endl;
	#endif//MEMORY_DEBUG
}


/*! Examines the hash map to see if the neuron ID is present. */
void NeuronIDArchiveStatistic::recalculate(){
	if(neuronIDHashMap->count(neuronID)){
		++(*firingNeuronCount);
		++(*firingNeuronTotal);
	}
	#ifdef ARCHIVE_STATISTICS_DEBUG
		cout<<"NeuronIDArchiveStatistic: Recalculated neuron group. firingNeuronCount = "<<*firingNeuronCount<<"; firingNeuronTotal = "<<*firingNeuronTotal<<endl;
	#endif//ARCHIVE_STATISTICS_DEBUG
}


/*! Outputs a string that would have been used to create this class. */
QString NeuronIDArchiveStatistic::toQString(){
	return QString::number(neuronID);
}


