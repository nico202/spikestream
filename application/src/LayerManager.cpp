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
#include "LayerManager.h"
#include "Debug.h"
#include "Utilities.h"
#include "NeuronGroupType.h"
#include "GlobalVariables.h"
#include "DeviceTypes.h"

//Other includes
#include <cmath>
#include <iostream>
using namespace std;
using namespace mysqlpp;


/*! Exception thrown when there is an error creating connections. */
class EditNeuronGroupException : public exception{
  virtual const char* what() const throw(){
    return "Error during editing of neuron group.";
  }
} editNeuronGroupException;


/*! Constructor. */
LayerManager::LayerManager(DBInterface *netDBInter, DBInterface* devDBInter){
	//Store reference to the database
	networkDBInterface = netDBInter;
	deviceDBInterface = devDBInter;

	//Initialize the random number generator with the seed
	srand(randomSeed);
	
	//Create a progress dialog
	progressDialog = new Q3ProgressDialog(0, "Creating layers", true);
} 


/*! Destructor. */
LayerManager::~LayerManager(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING LAYER MANAGER!"<<endl;
	#endif//MEMORY_DEBUG
	
	delete progressDialog;
}


//----------------------------------------------------------------------------------
//--------------------------------- PUBLIC METHODS ---------------------------------
//----------------------------------------------------------------------------------

/*! This method creates layers in the database
	Initially only handle rectangular layers. Other shapes could be added later if necessary.
	Progress dialog is used to keep track of progress with large layers, which can take some time. */
int LayerManager::createLayer(const NeuronGroup &neuronGrp){
	//Create appropriate connections	
	//Work on assumption that LayerPropertiesDialog has done checking on compatibility etc.
	switch(neuronGrp.neuronGrpType){
		case (NeuronGroupType::RectangularLayer2D):
			return create2DRectangularLayer(neuronGrp);
		case (NeuronGroupType::RectangularLayer3D):
			return create3DRectangularLayer(neuronGrp);
		case (NeuronGroupType::SIMNOSComponentLayer):
			return createSIMNOSComponentLayer(neuronGrp);
		default:
			cerr<<"CONNECTION TYPE NOT RECOGNIZED"<<endl;
			return -1;
	}
}


/*! Deletes layer from the database. */
void LayerManager::deleteLayers(vector<unsigned int> layerIDs){
	Query query = networkDBInterface->getQuery();
	
	vector<unsigned int>::iterator iter;
	for(iter = layerIDs.begin(); iter < layerIDs.end(); ++iter){
		//First delete information about the layer from the layer table
		query.reset();
		query<<"DELETE FROM NeuronGroups WHERE NeuronGrpID=\'"<<*iter<<"\'";
		query.execute();
		
		//Next delete neurons from neuron table
		query.reset();
		query<<"DELETE FROM Neurons WHERE NeuronGrpID=\'"<<*iter<<"\'";
		query.execute();

		//Delete neuron parameters for this neuron group
		/* Simpler just to delete neuron group from all parameter tables than to query the
			 id and then delete it from the appropriate table */
		query.reset();
		query<<"SELECT ParameterTableName FROM NeuronTypes";
                StoreQueryResult tableNameRes = query.store();
                for(StoreQueryResult::iterator paramTableIter = tableNameRes.begin(); paramTableIter != tableNameRes.end(); ++paramTableIter){
			Row tableNameRow (*paramTableIter);
			query.reset();
			query<<"DELETE FROM "<<(std::string)tableNameRow["ParameterTableName"]<<" WHERE NeuronGrpID= "<<*iter;
			query.execute();
		}

		//Delete noise parameters for this neuron group
		query.reset();
		query<<"DELETE FROM NoiseParameters WHERE NeuronGrpID=\'"<<*iter<<"\'";
		query.execute();
	}
}


/*! Passes a reference to connection widget that is used when creating 
	SIMNOS component layers. */
void LayerManager::setConnectionWidget(ConnectionWidget* connWidg){
	connectionWidget = connWidg;
}


/*! Sets the name of a layer. Called during layer editing mode
	Name does not conflict with anything else so no checks need to be run. */
void LayerManager::setNeuronGrpName(unsigned int neuronGrpID, QString name){
	Query query = networkDBInterface->getQuery();
	query.reset();
        query<<"UPDATE NeuronGroups SET Name = \""<<name.toStdString()<<"\" WHERE NeuronGrpID = "<<neuronGrpID;
	query.execute();
}


/*! Alters the spacing between neurons and the position of a neuron group
	This could potentially expand the layer to conflict with other layers, so need to check first
	False is returned if there is a conflict. */
bool LayerManager::setNeuronGrpSpacingPosition(const unsigned int neuronGrpID, const NeuronGroup &oldNeurGrp, const NeuronGroup &newNeurGrp){
	//Check for conflicts with existing layers. Do not want two neurons at same location
	//At present layers are organised along the z axis. 
	//If there is already a layer at this z position, need to see if proposed layer will overlap
	int actualWidth = newNeurGrp.spacing * (newNeurGrp.width - 1);
	int actualLength = newNeurGrp.spacing * (newNeurGrp.length - 1);
		
	//Get all layers with this z coordinate
	Query query = networkDBInterface->getQuery();
	query.reset();
	query<<"SELECT NeuronID FROM Neurons WHERE X >="<<newNeurGrp.xPos<<" AND X <= "<<(newNeurGrp.xPos+actualWidth)<<" AND Y >= "<<newNeurGrp.yPos<<" AND Y <= "<<(newNeurGrp.yPos + actualLength)<<" AND Z = "<<newNeurGrp.zPos<<" AND NeuronGrpID != "<<neuronGrpID;
        StoreQueryResult layerResult = query.store();
	if(layerResult.size() > 0)
		return false;
	
	//If have reached this point in the program, the new neuron spacing and position should be ok

	/*Need to relocate all of the neurons
		This uses the same method as was used to create the neurons
		The old location and neuron spacing is used to find the original neurons, 
		and their locations are updated.
		Use neuronNumber as a check that only the relevant neuron is being updated since there is a 
		chance that two neurons could be at the same location during the update process.*/

	//First get the first neuron Id in the group
	query.reset();
	query<<"SELECT MIN(NeuronID) FROM Neurons WHERE NeuronGrpID = "<<neuronGrpID;
        StoreQueryResult minNeuronIDRes = query.store();
	Row minNeuronIDRow(*minNeuronIDRes.begin());//Should only be one result

	//Initialise neuronIDCount
	unsigned int startNeuronID = Utilities::getUInt((std::string)minNeuronIDRow["MIN(NeuronID)"]);
	unsigned int neuronIDCount = startNeuronID;
	
	//Start up the progress dialog with the appropriate number of steps
	progressDialog->setTotalSteps(newNeurGrp.width*newNeurGrp.length);
	progressDialog->setLabelText("Updating Layer");
	int progressCounter = 0;
	
	//Work through the layers in the same way that was done when it was created
	for(unsigned int i=0; i<newNeurGrp.length; ++i){
		for(unsigned int j=0; j<newNeurGrp.width; ++j){
			query.reset();
			query<<"UPDATE Neurons SET X = "<<(newNeurGrp.xPos + (j*newNeurGrp.spacing))<<", Y = "<<(newNeurGrp.yPos + (i*newNeurGrp.spacing))<<", Z = "<<newNeurGrp.zPos<<" WHERE X = "<<(oldNeurGrp.xPos + (j*oldNeurGrp.spacing))<<" AND Y = "<<(oldNeurGrp.yPos + (i*oldNeurGrp.spacing))<<" AND Z = "<<oldNeurGrp.zPos<<" AND NeuronID = "<<neuronIDCount;
			
                        SimpleResult updateResult = query.execute();
			/*Check that neuron has been updated. For spacing changes, the first neuron 
				will not be changed so ignore it for this check */
                        if(updateResult.rows() == 1 || neuronIDCount == startNeuronID){
				++neuronIDCount;
				++progressCounter;
				if(progressCounter % 100 == 0)
					progressDialog->setProgress(progressCounter);
			}
			else{
				cerr<<"LayerManager: NEURON HAS BEEN PASSED OVER DURING SPACING/POSITION CHANGE. NEURONGROUP: "<<neuronGrpID<<"; i="<<i<<"; j="<<j<<"; neuronIDCount="<<neuronIDCount<<endl;
				cerr<<"Old Neuron group holder: x="<<oldNeurGrp.xPos<<"; y="<<oldNeurGrp.yPos<<"; z="<<oldNeurGrp.zPos<<"; spacing="<<oldNeurGrp.spacing<<"; width="<<oldNeurGrp.width<<"; length="<<oldNeurGrp.length<<endl;
				cerr<<"New Neuron group holder: x="<<newNeurGrp.xPos<<"; y="<<newNeurGrp.yPos<<"; z="<<newNeurGrp.zPos<<"; spacing="<<newNeurGrp.spacing<<"; width="<<newNeurGrp.width<<"; length="<<newNeurGrp.length<<endl;
				throw editNeuronGroupException;
			}
		}
	}
	progressDialog->reset();
	
	//Update neuron spacing in the neuron group table
	query.reset();
	query<<"UPDATE NeuronGroups SET Spacing = "<<newNeurGrp.spacing<<" WHERE NeuronGrpID = "<<neuronGrpID;
	query.execute();
	
	//Update neuron group position in the neuron group table
	query<<"UPDATE NeuronGroups SET X = "<<newNeurGrp.xPos<<", Y = "<<newNeurGrp.yPos<<", Z = "<<newNeurGrp.zPos<<" WHERE NeuronGrpID = "<<neuronGrpID;
	query.execute();
	return true;
}


/*! Sets the type of neuron in a layer.
	Neuron type does not conflict with anything else, so no checks need to be run. */
void LayerManager::setNeuronGrpType(unsigned int neuronGrpID, unsigned short neuronType){
	Query query = networkDBInterface->getQuery();
	try{
		query.reset();
	
		//First set the type in the neuron group
		query<<"UPDATE NeuronGroups SET NeuronType = "<<neuronType<<" WHERE NeuronGrpID = "<<neuronGrpID;
		query.execute();
	
		//Now need to sort out the parameters
		//Delete entries for this neuron group from parameter tables and add an entry in the appropriate place
		query.reset();
		query<<"SELECT TypeID, ParameterTableName FROM NeuronTypes";
                StoreQueryResult neurTypesRes = query.store();
                for(StoreQueryResult::iterator iter = neurTypesRes.begin(); iter != neurTypesRes.end(); ++iter){
			Row neurTypeRow (*iter);
			unsigned short neuronTypeID = Utilities::getUShort((std::string) neurTypeRow["TypeID"]);
			
			//Delete entry if it exists
			query.reset();
			query<<"DELETE FROM "<<(std::string)neurTypeRow["ParameterTableName"]<<" WHERE NeuronGrpID = "<<neuronGrpID;
			query.execute();
			
			//Add this neuron group to the parameter table
			if(neuronTypeID == neuronType){
				query.reset();
				query<<"INSERT INTO "<<(std::string)neurTypeRow["ParameterTableName"]<<"(NeuronGrpID) VALUES("<<neuronGrpID<<")";
				query.execute();
			}
		}
	}
	catch(Exception ex){
		cerr<<"Exception thrown whilst setting the neuron group type: "<<ex.what()<<endl;
	}
}


//-------------------------------------------------------------------------------
//------------------------------ PRIVATE METHODS --------------------------------
//-------------------------------------------------------------------------------

/*! Creates a 2D rectangular layer. */
int LayerManager::create2DRectangularLayer(const NeuronGroup &neuronGrp){
	//First set up the progress dialog with the appropriate number of steps
	progressDialog->reset();
	progressDialog->setTotalSteps(neuronGrp.width*neuronGrp.length);
	
	//Method starts by checking for conflicts with existing layers. Do not want two neurons at same location
	bool layerConflict = false;

	//At present layers are organised along the z axis. 
	//If there is already a layer at this z position, need to see if proposed layer will overlap
	int actualWidth = neuronGrp.spacing * (neuronGrp.width - 1);
	int actualLength = neuronGrp.spacing * (neuronGrp.length - 1);
		
	//Get all layers with this z coordinate
	Query query = networkDBInterface->getQuery();
	query.reset();
	query<<"SELECT NeuronID FROM Neurons WHERE X >="<<neuronGrp.xPos<<" AND X <= "<<(neuronGrp.xPos+actualWidth)<<" AND Y >= "<<neuronGrp.yPos<<" AND Y <= "<<(neuronGrp.yPos + actualLength)<<" AND Z = "<<neuronGrp.zPos;
        StoreQueryResult zLayerResult = query.store();
	if(zLayerResult.size() >0){
		layerConflict = true;
		cout<<"CONFLICT WITH EXISTING LAYERS"<<endl;
		progressDialog->reset();
		return -1;
	}

	//Only reach this point if there are no conflicts with existing layers
	//Add layer entry to layer database
	query.reset();
        query<<"INSERT INTO NeuronGroups (Name, NeuronGrpType, NeuronType, X, Y, Z, Width, Length, Spacing, TaskID) VALUES (\""<<neuronGrp.name.toStdString()<<"\", "<<neuronGrp.neuronGrpType<<", "<<neuronGrp.neuronType<<", "<<neuronGrp.xPos<<", "<<neuronGrp.yPos<<", "<<neuronGrp.zPos<<", "<<neuronGrp.width<<", "<<neuronGrp.length<<", "<<neuronGrp.spacing<<", -1)";
	query.execute();
	
	//Now need to get the automatically generated NeuronGrpID so that it can be added to the neurons in the new layers
	query.reset();
	query<<"SELECT MAX(NeuronGrpID) from NeuronGroups";
        StoreQueryResult grpIDResult = query.store();
	Row row(*(grpIDResult.begin()));
	unsigned int neuronGrpID = row.at(0);
			
	/* Add neurons to neuron database. 
		This adds them in a scanning pattern moving horizontally along the x axis 
		before moving to the next line */
	int neuronCount = 0;
	for(unsigned int i=0; i<neuronGrp.length; ++i){
		for(unsigned int j=0; j<neuronGrp.width; ++j){
			query.reset();
			query<<"INSERT INTO Neurons (X, Y, Z, NeuronGrpID) VALUES ("<<(neuronGrp.xPos + (j*neuronGrp.spacing))<<", "<<(neuronGrp.yPos + (i*neuronGrp.spacing))<<", "<<neuronGrp.zPos<<", "<<neuronGrpID<<")";
			query.execute();
			++neuronCount;
			if(neuronCount % 100 == 0)
				progressDialog->setProgress(neuronCount);
			if(progressDialog->wasCancelled())
				break;
		}
	}

	/* Add entry for this layer to the neuron parameters database */
	//Find the appropriate table
	query.reset();
	query<<"SELECT ParameterTableName FROM NeuronTypes WHERE TypeID = "<<neuronGrp.neuronType;
        StoreQueryResult tableNameRes = query.store();
	Row tableNameRow (*tableNameRes.begin());//Should only be 1 row
	
	//Now add an entry for this neuron group to the appropriate table
	query.reset();
	query<<"INSERT INTO "<<(std::string)tableNameRow["ParameterTableName"]<<" (NeuronGrpID) VALUES ("<<neuronGrpID<<")";
	query.execute();

	//Add entry for this layer to the noise parameters table
	query.reset();
	query<<"INSERT INTO NoiseParameters (NeuronGrpID) VALUES ("<<neuronGrpID<<")";
	query.execute();

	//Hide progress dialog
	progressDialog->cancel();

	return neuronGrpID;
}


/*! Create a 3D rectangular layer. */
int LayerManager::create3DRectangularLayer(const NeuronGroup &neuronGrp){
	//Parameters for the connection - will have to be set dynamically at some point
	unsigned int depth = 5;//3D box with this depth
	double density = 0.5;//Number of neurons per point within the box

	//Set up the progress dialog with the appropriate number of steps. This is a guess because of the random element in creating the layer
	unsigned int totalSteps = (unsigned int) ((double)neuronGrp.width * (double)neuronGrp.length * (double)depth * density);
	progressDialog->setTotalSteps(totalSteps);

	//Add layer entry to layer database
	Query query = networkDBInterface->getQuery();
	query.reset();
        query<<"INSERT INTO NeuronGroups (Name, NeuronGrpType, NeuronType, X, Y, Z, Width, Length, Spacing, TaskID) VALUES (\""<<neuronGrp.name.toStdString()<<"\", "<<neuronGrp.neuronGrpType<<", "<<neuronGrp.neuronType<<", "<<neuronGrp.xPos<<", "<<neuronGrp.yPos<<", "<<neuronGrp.zPos<<", "<<neuronGrp.width<<", "<<neuronGrp.length<<", "<<neuronGrp.spacing<<", -1)";
	query.execute();

	//Get the automatically generated NeuronGrpID so that it can be added to the neurons in the new layers
	query.reset();
	query<<"SELECT MAX(NeuronGrpID) from NeuronGroups";
        StoreQueryResult grpIDResult = query.store();
	Row row(*(grpIDResult.begin()));
	unsigned int neuronGrpID = Utilities::getUInt((std::string)row["MAX(NeuronGrpID)"]);
	unsigned int neuronCount = 0;

	//Work through each point in the box and add the neuron if necessary.
	for(int zPos = neuronGrp.zPos; zPos < (int)(neuronGrp.zPos + depth); ++zPos){
		for(int yPos = neuronGrp.yPos; yPos < (int)(neuronGrp.yPos + neuronGrp.length); ++yPos){
			for(int xPos = neuronGrp.xPos; xPos < (int)(neuronGrp.xPos + neuronGrp.width); ++xPos){
				//Decide if a neuron should be placed or not
				if(evaluatePlaceNeuronProbability(density)){
					//Check neuron does not exist at this location
					query.reset();
					query<<"SELECT NeuronID From Neurons WHERE X = "<<xPos<<" AND Y = "<<yPos<<" AND Z = "<<zPos;
                                        StoreQueryResult alreadyExistsResult = query.store();
					if(alreadyExistsResult.size() == 0){
						//Add entry for neuron to database
						query.reset();
						query<<"INSERT INTO Neurons (X, Y, Z, NeuronGrpID) VALUES ("<<xPos<<", "<<yPos<<", "<<zPos<<", "<<neuronGrpID<<")";
						query.execute();
						++neuronCount;
						if(neuronCount % 100 == 0)
							progressDialog->setProgress(neuronCount);
						if(progressDialog->wasCancelled())
							break;
					}
				}
			}
		}
	}

	//Hide progress dialog
	progressDialog->setProgress(totalSteps);

	//Check that at least one neurons has been added
	if(neuronCount <= 0){
		cout<<"LayerManager: No neurons have been created, probably due to conflicts with existing neurons"<<endl;
		query.reset();
		query<<"DELETE FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID;
		query.execute();
		return -1;
	}


	/* Add entry for this layer to the neuron parameters database */
	//Find the appropriate table
	query.reset();
	query<<"SELECT ParameterTableName FROM NeuronTypes WHERE TypeID = "<<neuronGrp.neuronType;
        StoreQueryResult tableNameRes = query.store();
	Row tableNameRow (*tableNameRes.begin());//Should only be 1 row
	
	//Now add an entry for this neuron group to the appropriate table
	query.reset();
	query<<"INSERT INTO "<<(std::string)tableNameRow["ParameterTableName"]<<" (NeuronGrpID) VALUES ("<<neuronGrpID<<")";
	query.execute();

	//Add entry for this layer to the noise parameters table
	query.reset();
	query<<"INSERT INTO NoiseParameters (NeuronGrpID) VALUES ("<<neuronGrpID<<")";
	query.execute();

	//Hide progress dialog
	progressDialog->cancel();

	return neuronGrpID;
}


/*! Creates a layer for a component of SIMNOS with appropriate connections
	to the device input layer. */
int LayerManager::createSIMNOSComponentLayer(const NeuronGroup &neuronGrp){
	//Create a 2D rectangular layer using method in this class
	int neuronGrpID = this->create2DRectangularLayer(neuronGrp);

	//Check layer has been created
	if(neuronGrpID < 0){
		cerr<<"LayerManager: FAILED TO CREATE LAYER WHILST CREATING SIMNOS COMPONENT LAYER"<<endl;
		return neuronGrpID;
	}

	//Find out if it is an input or output device
	Query deviceQuery = deviceDBInterface->getQuery();
	deviceQuery.reset();
	deviceQuery<<"SELECT ReceptorIDs FROM SIMNOSComponents WHERE ComponentID = "<<neuronGrp.componentID;
        StoreQueryResult simnosRes = deviceQuery.store();
	Row simnosRow (*simnosRes.begin());//Component ID should be unique
		
	//Extract the first receptor id in the list
	//NOTE THIS ASSUMES THAT ALL OF THE RECEPTORS ARE FROM THE SAME DEVICE
        QString receptorIDString = ((std::string)simnosRow["ReceptorIDs"]).data();
	if(receptorIDString.isEmpty()){
		cerr<<"LayerManager: NO RECEPTOR IDS FOR THIS COMPONENT"<<endl;
		return neuronGrpID;
	}

	//Get the receptor ID - converting to int is a good check
	unsigned int firstReceptorID = Utilities::getUInt(receptorIDString.section(',', 0, 0).ascii());
	
	deviceQuery.reset();
	deviceQuery<<"SELECT DeviceDescription FROM SIMNOSSpikeReceptors WHERE ReceptorID = "<<firstReceptorID;
        StoreQueryResult receptorDevDesRes = deviceQuery.store();
	Row receptorDevDesRow(*receptorDevDesRes.begin());//ReceptorID should be unique

	//Now get device type from database
	deviceQuery.reset();
	deviceQuery<<"SELECT Type FROM Devices WHERE Description = \""<<((std::string) receptorDevDesRow["DeviceDescription"])<<"\"";
        StoreQueryResult devTypeRes = deviceQuery.store();
	Row devTypeRow(*devTypeRes.begin());//Description should be unique
	unsigned int deviceType = Utilities::getUInt((std::string)devTypeRow["Type"]);

	/* Instruct connectionWidget to launch the dialog so user can 
		select the connection options */
	if(DeviceTypes::isInputDevice(deviceType))//Create connections from device
		connectionWidget->createConnections(neuronGrp.deviceNeuronGrpID, neuronGrpID, neuronGrp.componentID, true);
	else
		connectionWidget->createConnections(neuronGrpID, neuronGrp.deviceNeuronGrpID, neuronGrp.componentID, false);

	//Return the new neuronGrpID
	return neuronGrpID;
}


/*! Used to create layers that are not completely dense - especially
	the 3D layer. Decides whether neuron should be added at a particular
	point or not. */
bool LayerManager::evaluatePlaceNeuronProbability(double density){
	int threshold = (int)(density * (double)RAND_MAX);
	if(rand() < threshold)
		return true;
	return false;
}


