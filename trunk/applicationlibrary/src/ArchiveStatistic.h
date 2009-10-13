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

#ifndef ARCHIVESTATISTIC_H
#define ARCHIVESTATISTIC_H

//Qt includes
#include <qstring.h>

//Other includes
#include <string>
#include <vector>
#include <google/dense_hash_map>
using namespace std;
using HASH_NAMESPACE::hash;
using GOOGLE_NAMESPACE::dense_hash_map;

//-------------------------- Archive Statistic -----------------------------
/*! Abstract class used to gather statistics about an archive. 
	Inheriting classes add up information about neuron groups or neuron ids.*/
//--------------------------------------------------------------------------
class ArchiveStatistic {

	public:
		ArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal);
		virtual ~ArchiveStatistic();
		virtual void recalculate();
		virtual void recalculate(unsigned int neurID);
		virtual void recalculateNeuronGrp();
		virtual QString toQString() = 0;
		unsigned int getID();
		unsigned int getType();
		void resetFiringNeuronCount();
		void resetFiringNeuronTotal();
		void setID(unsigned int id);

		/* Static types of neuron statistic. These two types are processed
			separately for efficiency reasons.*/
		/*! This statistic is monitoring an entire neuron group.*/
		static const unsigned int NEURON_GROUP = 10;

		/*! This statistic is monitoring a selection of neuron ids.*/
		static const unsigned int NEURON_ID = 20;
		
		/* Make NetworkDataXmlHandler a friend so it can set the reference
			to the hash map.*/
		friend class NetworkDataXmlHandler;


	protected:
		//============================== VARIABLES =============================
		/*! Is this monitoring a neuron group or a set of neuron ids?.*/
		unsigned int type;

		/*! Pointer to the hash map in the NetworkDataXmlHandler. At each timestep
			the firing neuron ids are placed in this map and then the archive
			statistics are invoked to carry out their analysis. */
		static dense_hash_map<unsigned int, bool, hash<unsigned int> >* neuronIDHashMap;

		/*! The number of neurons firing at the current timestep. This is a pointer
			to the variable in the ArchiveStatisticsHolder struct, which contains one
			or more ArchiveStatistics.*/
		unsigned int* firingNeuronCount;

		/*! The total number of neurons that have fired so far. This is a pointer
			to the variable in the ArchiveStatisticsHolder struct, which contains one
			or more ArchiveStatistics.*/
		unsigned int* firingNeuronTotal;


	private:
		//============================= VARIABLES =============================
		/*! The unique id of this archive statistic. Used for deleting them.*/
		unsigned int ID;

};


//--------------------- Archive Statistics Holder --------------------------
/*! Holds a collection of archive statistics. This is created by the 
	ArchiveStatisticsDialog and displayed in the ArchiveWidget. Each of the 
	ArchiveStatistics in the vector should have been created with references 
	to the firingNeuronCount and the firingNeuronTotal in this struct.*/
//--------------------------------------------------------------------------
struct ArchiveStatisticsHolder {
	string description;
	unsigned int ID;
	vector<ArchiveStatistic*> archStatVector;
	unsigned int firingNeuronCount;
	unsigned int firingNeuronTotal;
};


//--------------------- Neuron Group Archive Statistic ---------------------
/*! Totals up the neuron firing events for a neuron group. */
//--------------------------------------------------------------------------
class NeuronGrpArchiveStatistic : public ArchiveStatistic {

	public:
		NeuronGrpArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int nGrpID);
		~NeuronGrpArchiveStatistic();
		unsigned int getNeuronGrpID();
		void recalculateNeuronGrp();
		QString toQString();


	private:
		//============================== VARIABLES =============================
		/*! The id of the neuron group that is being monitored.*/
		unsigned int neuronGrpID;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NeuronGrpArchiveStatistic(const NeuronGrpArchiveStatistic&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NeuronGrpArchiveStatistic operator = (const NeuronGrpArchiveStatistic&);

};


//---------------------- Range Archive Statistic ---------------------------
/*! Totals up the number of times neurons within a particular range fire. 
	The range here is inclusive of both high and low values. */
//--------------------------------------------------------------------------
class RangeArchiveStatistic : public ArchiveStatistic {

	public:
		RangeArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int rLow, unsigned int rHigh);
		~RangeArchiveStatistic();
		void recalculate();
		QString toQString();


	private:
		//============================== VARIABLES =============================
		/*! The low point of the range.*/
		unsigned int rangeLow;

		/*! The high point of the range.*/
		unsigned int rangeHigh;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		RangeArchiveStatistic(const RangeArchiveStatistic&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		RangeArchiveStatistic operator = (const RangeArchiveStatistic&);

};


//------------------------ And Archive Statistic ---------------------------
/*! Totals up the number of times the two specified neuron ids occur 
	together. */
//--------------------------------------------------------------------------
class AndArchiveStatistic : public ArchiveStatistic {

	public:
		AndArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int fNeurID, unsigned int sNeurID);
		~AndArchiveStatistic();
		void recalculate();
		QString toQString();


	private:
		//============================== VARIABLES =============================
		/*! The first neuron id.*/
		unsigned int firstNeuronID;

		/*! The second neuron id.*/
		unsigned int secondNeuronID;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		AndArchiveStatistic(const AndArchiveStatistic&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		AndArchiveStatistic operator = (const AndArchiveStatistic&);

};


//------------------------ Or Archive Statistic ----------------------------
/*! Totals up the number of times that either one or the other neuron id
	occurs. Note that if both occur, the count will only be increased by 
	one.*/
//--------------------------------------------------------------------------
class OrArchiveStatistic : public ArchiveStatistic {

	public:
		OrArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int fNeurID, unsigned int sNeurID);
		~OrArchiveStatistic();
		void recalculate();
		QString toQString();


	private:
		//============================== VARIABLES =============================
		/*! The first neuron id.*/
		unsigned int firstNeuronID;

		/*! The second neuron id.*/
		unsigned int secondNeuronID;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		OrArchiveStatistic(const OrArchiveStatistic&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		OrArchiveStatistic operator = (const OrArchiveStatistic&);

};


//--------------------- Neuron ID Archive Statistic ------------------------
/*! Totals up the number of times that a single neuron id occurs. */
//--------------------------------------------------------------------------
class NeuronIDArchiveStatistic : public ArchiveStatistic {

	public:
		NeuronIDArchiveStatistic(unsigned int* fNeurCount, unsigned int* fNeurTotal, unsigned int neurID);
		~NeuronIDArchiveStatistic();
		void recalculate();
		QString toQString();


	private:
		//============================== VARIABLES =============================
		/*! The neuron id.*/
		unsigned int neuronID;


		//=========================== METHODS =================================
		/*! Declare copy constructor private so it cannot be used inadvertently.*/
		NeuronIDArchiveStatistic(const NeuronIDArchiveStatistic&);

		/*! Declare assignment private so it cannot be used inadvertently.*/
		NeuronIDArchiveStatistic operator = (const NeuronIDArchiveStatistic&);

};


#endif//ARCHIVESTATISTIC_H


