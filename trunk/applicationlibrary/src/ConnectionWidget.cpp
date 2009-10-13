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
#include "ConnectionWidget.h"
#include "ConnectionPropertiesDialog.h"
#include "NetworkViewer_V2.h"
#include "Debug.h"
#include "Utilities.h"
#include "ConnectionTableItem.h"
#include "ConnectionCheckTableItem.h"
#include "SpikeStreamMainWindow.h"
#include "ConnectionParameterViewer.h"

//Qt includes
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <q3accel.h>
#include <qlayout.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPixmap>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
#include <vector>
#include <mysql++.h>
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
ConnectionWidget::ConnectionWidget(QWidget *parent, DBInterface *netDBInter, DBInterface* devDBInter) : QWidget(parent, "Connection Widget"){
	//Keep reference to database interface
	networkDBInterface = netDBInter;
	deviceDBInterface = devDBInter;

	//Create dialog to show when deleting connections
	busyDialog = new BusyDialog(SpikeStreamMainWindow::spikeStrMainWin, "Editing Connections");

	//Create a ConnectionManager to manage the neurons
	connectionManager = new ConnectionManager(networkDBInterface, deviceDBInterface);
	
	//Set up pixmaps to control showing and hiding connections and parameters
	showPixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/view.xpm");
	hidePixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/hide.xpm");
	paramPixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/view_parameters.xpm");
	
	//Set up widget
	//Create vertical layout to arrange panel
        Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 5, 5, "vertical");

	//Set up buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	newConnButt = new QPushButton("Add Connections", this, "NewConnection");
	newConnButt->setAccel(QKeySequence(Qt::CTRL + Qt::Key_N));
	connect (newConnButt, SIGNAL(clicked()), this, SLOT(newConnectionsButtonPressed()));
	buttonBox->addWidget(newConnButt);
	
	deleteConnButt = new QPushButton("Delete", this, "Delete Conns");
	deleteConnButt->setAccel(QKeySequence(Qt::Key_Delete));
	connect (deleteConnButt, SIGNAL(clicked()), this, SLOT(deleteConnections()));
	buttonBox->addWidget(deleteConnButt);
	buttonBox->addStretch(5);

	verticalBox->addLayout(buttonBox);	
	
	//Set up table
	connTable = new Q3Table(0, 8, this);
	connTable->setShowGrid(false);
	connTable->setSorting(false);
	connTable->setSelectionMode(Q3Table::NoSelection);
	connTable->verticalHeader()->hide();
	connTable->setLeftMargin(0);
	Q3Header * connTableHeader = connTable->horizontalHeader();
	connTableHeader->setLabel( 0, "" );
	connTable->setColumnWidth( 0, 16);	
	connTableHeader->setLabel( 1, "From Layer" );
	connTable->setColumnWidth( 1, 150);
	connTableHeader->setLabel( 2, "To Layer");
	connTable->setColumnWidth( 2, 150);
	connTableHeader->setLabel( 3, "Connection Type");
	connTable->setColumnWidth( 3, 200);
	connTableHeader->setLabel( 4, "Synapse Type");
	connTable->setColumnWidth( 4, 200);
	connTableHeader->setLabel( 5, "Size");
	connTable->setColumnWidth( 5, 50);
	connGrpIDColumn = 6;
	connTableHeader->setLabel(connGrpIDColumn, "ID");
	connTable->setColumnWidth(connGrpIDColumn, 50);
	paramCol = 7;
	connTableHeader->setLabel(paramCol, "Parameters");
	connTable->setColumnWidth(connGrpIDColumn, 50);
	
	verticalBox->addWidget(connTable);
	
	//Connect header with table header clicked to enable selecting/ deselecting all layers
	connect (connTableHeader, SIGNAL(clicked(int)), this, SLOT(tableHeaderClicked(int)));
	
	//Need to set up table so that clicks on view pixmaps are reflected in 3D display
	connect (connTable, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT(tableClicked(int, int, int, const QPoint &)));
	
	//Now load the list of connections from the database
	loadAllConnections();
} 


/*! Destructor. */
ConnectionWidget::~ConnectionWidget(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING CONNECTION WIDGET"<<endl;
	#endif//MEMORY_DEBUG
	
	delete showPixmap;
	delete hidePixmap;
	delete connectionManager;
}

//---------------------------------------------------------------------------------------------
//------------------------------- PUBLIC METHODS ----------------------------------------------
//---------------------------------------------------------------------------------------------

/*! Creates connections. This method can be called from outside or internally when the
	new connections button is pressed. */
void ConnectionWidget::createConnections(unsigned int fromNeurGrpID, unsigned int toNeurGrpID, unsigned int componentID, bool deviceIsFrom){
	try{
		/* Set up connections properties dialog differently depending on whethe
			this method has been called by the user or the layer manager */
		ConnectionPropertiesDialog *connPropDlg;
		bool simnosComponentConns = false;
		if(fromNeurGrpID == 0  && toNeurGrpID == 0 && componentID == 0)
			connPropDlg = new ConnectionPropertiesDialog(this, "Conn Properties", networkDBInterface);
		else{
			connPropDlg = new ConnectionPropertiesDialog(this, "Conn Properties", networkDBInterface, fromNeurGrpID, toNeurGrpID, ConnectionType::SIMNOSComponent);
			simnosComponentConns = true;
		}

        connPropDlg->setCaption( "Connection Properties" );
        if ( connPropDlg->exec() == QDialog::Accepted ) {
			//Create new set of connections
			//Extract parameters for connections from dialog
			ConnectionHolder connHolder = connPropDlg->getConnectionHolder();

			//Add component ID if applicable
			if(simnosComponentConns)
				connHolder.componentID = componentID;

			//Set whether the device group is from or to
			connHolder.deviceIsFrom = deviceIsFrom;

			//Create the connections
			int newConnGrpID = connectionManager->createConnections(connHolder);
			if(newConnGrpID < 0){//Method has not created any connections probably due to conflict with existing connections
				QMessageBox::warning(this, "Connections Conflict!", "Connections have not been created, perhaps due to conflict with existing connections");
			}
			else{//Connections have been successfully created
				/* With the simnos connections, the network viewer has not loaded up the new
					neuron group yet, so this needs to be done before loading the connection 
					group. */
				if(simnosComponentConns){
					if(connHolder.deviceIsFrom)//New layer is to neuron grp id
						SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadNeuronGroup(connHolder.toLayerID, true);
					else
						SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadNeuronGroup(connHolder.fromLayerID, true);
				}

				//Load new connection group into network viewer and table
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadConnectionGroup(newConnGrpID, true);
				loadConnection(newConnGrpID);

				//Reload the connections in the rest of the application
				SpikeStreamMainWindow::spikeStrMainWin->reloadConnections();
			}
        }
        delete connPropDlg;
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query creating connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ConnectionWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown creating connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
    }
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"ConnectionWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown creating connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
	}
}


/*! Deletes all connections to and from layer
	This method is usually called by the layerWidget when deleting a layer so that all the 
	connections to the layer can be deleted. */
void ConnectionWidget::deleteLayerConnections(unsigned int layerID){
	try{
		Query query = networkDBInterface->getQuery();
		
		//First remove data structures from Network Viewer and delete connections
		query.reset();
		query<<"SELECT ConnGrpID FROM ConnectionGroups WHERE FromNeuronGrpID = "<<layerID<<" OR ToNeuronGrpID = "<<layerID;
                StoreQueryResult connGrpRes = query.store();
                for(mysqlpp::StoreQueryResult::iterator connGrpIter = connGrpRes.begin(); connGrpIter != connGrpRes.end(); connGrpIter++){
			Row connGrpRow(*connGrpIter);
			unsigned int connGrpID = Utilities::getUInt((std::string)connGrpRow["ConnGrpID"]);
			SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->deleteConnectionGroup(connGrpID);
			removeConnectionFromTable(connGrpID);
			
			//Remove connections from database
			query.reset();
			query<<"DELETE FROM Connections WHERE ConnGrpID = "<<connGrpID;
			query.execute();
	
			/*Delete synapse parameters for this connection group
				Simpler just to delete connection group from all parameter tables than to query the
				id and then delete it from the appropriate table */
			query.reset();
			query<<"SELECT ParameterTableName FROM SynapseTypes";
                        StoreQueryResult tableNameRes = query.store();
                        for(StoreQueryResult::iterator paramTableIter = tableNameRes.begin(); paramTableIter != tableNameRes.end(); ++paramTableIter){
				Row tableNameRow (*paramTableIter);
				query<<"DELETE FROM "<<(std::string)tableNameRow["ParameterTableName"]<<" WHERE ConnGrpID= "<<connGrpID;
				query.execute();
			}
		}
		
		//Now delete all connection groups that involve this layer from the database
		query.reset();
		query<<"DELETE FROM ConnectionGroups WHERE FromNeuronGrpID = "<<layerID<<" OR ToNeuronGrpID = "<<layerID;
		query.execute();
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when deleting layer connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
		return;
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ConnectionWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when deleting layer connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
		return;
    }
}


/*! Returns the vector containing a list of visible connection group IDs. */
vector<unsigned int> ConnectionWidget::getConnectionViewVector(){
	return viewVector;
}


/*! Reloads connections when changes have been made to them or to the neuron groups. */
void ConnectionWidget::reloadConnections(){
	loadAllConnections();
}


/*! Enables editing after simulation has been destroyed. */
void ConnectionWidget::simulationDestroyed(){
	newConnButt->setEnabled(true);
	deleteConnButt->setEnabled(true);
}


/*! Disables editing when simulation is initialised. */
void ConnectionWidget::simulationInitialised(){
	newConnButt->setEnabled(false);
	deleteConnButt->setEnabled(false);
}


//-----------------------------------------------------------------------------------
//--------------------------------- SLOTS -------------------------------------------
//-----------------------------------------------------------------------------------

/*! Deletes all selected connections
	Shows message box to confirm delete. */
void ConnectionWidget::deleteConnections(){
	try{
		QString confirmDeleteStr = "Do you want to delete the following connections:\n";
		vector<ConnectionHolder> deleteConnectionVector;
		for(int i=0; i<connTable->numRows(); i++){
			Q3CheckTableItem * item = (Q3CheckTableItem*)connTable->item(i, 1);
			if(item->isChecked()){
				//Declare a temporary connection holder to contain variables
				ConnectionHolder tempConnHolder;
				
				//Fill connection holder with parameters
				tempConnHolder.connectionGrpID = Utilities::getUInt(connTable->item(i, connGrpIDColumn)->text().ascii());
				tempConnHolder.fromLayerID = Utilities::getNeuronGrpID(connTable->item(i, 1)->text());
				tempConnHolder.toLayerID = Utilities::getNeuronGrpID(connTable->item(i, 2)->text());
				QString connectionTypeStr = connTable->item(i, 3)->text();
                                tempConnHolder.connectionType = ConnectionType::getType(connectionTypeStr);
				deleteConnectionVector.push_back(tempConnHolder);
				
				//Add variables to string for confirmation dialog
				confirmDeleteStr += "   From: ";
				confirmDeleteStr += QString::number(tempConnHolder.fromLayerID) += " To: ";
				confirmDeleteStr += QString::number(tempConnHolder.toLayerID) += "; Type: ";
				confirmDeleteStr += connTable->item(i, 3)->text() += "\n";
			}
		}
		//Check that user really wants to delete these connections
		if(deleteConnectionVector.size() > 0){
			if(!( QMessageBox::warning( this, "Confirm Delete Connections", confirmDeleteStr, "Yes", "No", 0, 0, 1 ))) {
	
				//Show dialog to inform user that deletion is in progress
				//FIXME THE TEXT ON THIS DIALOG IS ONLY VISIBLE SOMETIMES - SOMETHING TO DO WITH UPDATES
				busyDialog->setModal(true);
				busyDialog->showDialog("Deleting connections, please wait.");
				SpikeStreamMainWindow::spikeStreamApplication->processEvents();
	
				//Instruct connection manager to delete connections
				connectionManager->deleteConnections(deleteConnectionVector);
	
				//Remove data structures from network viewer
				for(unsigned int i=0; i<deleteConnectionVector.size(); i++){
					unsigned int connGrpID = deleteConnectionVector[i].connectionGrpID;
					
					//Remove connection from network viewer
					SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->deleteConnectionGroup(connGrpID);
	
					//Remove connection from table
					removeConnectionFromTable(connGrpID);
				}
	
				//Reload the connections in the rest of the application
				SpikeStreamMainWindow::spikeStrMainWin->reloadConnections();
				busyDialog->hide();
			}
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when deleting connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"ConnectionWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when deleting connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"ConnectionWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown deleting connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connection Error", errorString);
	}
}


/*! Creates new connections using the connection properties dialog. */
void ConnectionWidget::newConnectionsButtonPressed(){
	createConnections(0, 0, 0, false);
}


/*! Adjusts the 3D view by making connection groups visible or invisible. */
void ConnectionWidget::tableClicked(int row, int col, int, const QPoint &){
	if(col == 0){//Want a list of ConnectionGrpIDs to send to the NetworkViewer
		//Get Connection Group ID
		unsigned int connGrpID = 0;
		try{
			connGrpID= Utilities::getUInt(connTable->item(row, connGrpIDColumn)->text().ascii());
		}
		catch(std::exception& er){// Catch-all for std exceptions
			cerr<<"ConnectionWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown converting string to unsigned int: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Connection Group ID Error", errorString);
			return;
		}

		//Find out if it is already part of the viewVector
		bool connGrpIDFound = false;
		for(vector<unsigned int>::iterator iter = viewVector.begin(); iter != viewVector.end(); iter++){
			if(*iter == connGrpID){//Connection was already visible, so need to make it invisible
				connGrpIDFound = true;
				viewVector.erase(iter);
				connTable->setPixmap(row, 0, *hidePixmap);
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setConnectionView(viewVector);
				break;
			}
		}
		if(!connGrpIDFound){//Not already showing, so show connection
			//Only change pixmap if it is not a virtual connection, which cannot be shown.
			if(connTable->text(row, 3) != "Virtual")
				connTable->setPixmap(row, 0, *showPixmap);
			viewVector.push_back(connGrpID);
			SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setConnectionView(viewVector);
		}
	}
	else if (col == paramCol){
		//Get Connection Group ID
		unsigned int connGrpID = 0;
		try{
			connGrpID= Utilities::getUInt(connTable->item(row, connGrpIDColumn)->text().ascii());
		}
		catch(std::exception& er){// Catch-all for std exceptions
			cerr<<"ConnectionWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown converting string to unsigned int: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Connection Group ID Error", errorString);
			return;
		}
		
		//Launch a dialog to view the connection parameters
		ConnectionParameterViewer* connParamViewer = new ConnectionParameterViewer(this, networkDBInterface, connGrpID);
		if(!connParamViewer->loadError())
			connParamViewer->show();
	}
}


/*! Makes all connection groups visible / invisible or selects/deselects all layers. */
void ConnectionWidget::tableHeaderClicked(int colNumber){
	if(colNumber == 0){//View header has been clicked
		//All rows are already selected, so deselect
		if(viewVector.size() == (unsigned int)connTable->numRows()){
			viewVector.clear();
			SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setConnectionView(viewVector);
			
			//Change pixmaps to hide
			for(int i=0; i<connTable->numRows(); i++){
				connTable->setPixmap(i, 0, *hidePixmap);
			}
		}
		//Not all rows selected, so select all
		else{
			viewVector.clear();
			for(int i=0; i<connTable->numRows(); i++){
				unsigned int tempConnGrpID = connTable->item(i, connGrpIDColumn)->text().toUInt();
				viewVector.push_back(tempConnGrpID);
				connTable->setPixmap(i, 0, *showPixmap);
			}
			SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setConnectionView(viewVector);
		}
	}
	else if(colNumber ==1){//Header above check boxes has been clicked
		//First count the number of selected rows
		int selectedRowCount = 0;
		for(int i=0; i<connTable->numRows(); i++){
			Q3CheckTableItem * item = (Q3CheckTableItem*)connTable->item(i, 1);
			if(item->isChecked())
				selectedRowCount++;
		}
		//If all rows are selected want to deselect rows
		if(selectedRowCount == connTable->numRows()){
			for(int i=0; i<connTable->numRows(); i++){//Deselect all rows
				Q3CheckTableItem * item = (Q3CheckTableItem*)connTable->item(i, 1);
				item->setChecked(false);
			}
		}
		else{//Select all rows
			for(int i=0; i<connTable->numRows(); i++){
				Q3CheckTableItem * item = (Q3CheckTableItem*)connTable->item(i, 1);
				item->setChecked(true);
			}
		}
	}
}


//---------------------------------------------------------------------------------------------
//------------------------------ PRIVATE METHODS ----------------------------------------------
//---------------------------------------------------------------------------------------------

/*! Gets the description of a particular synapse type from the synapse types database. */
string ConnectionWidget::getSynapseTypeDescription(unsigned short synapseTypeID){
	string descStr("Unknown");
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT Description FROM SynapseTypes WHERE TypeID = "<<synapseTypeID;
                StoreQueryResult result = query.store();
		if(result.size() != 1){
			return string("Unknown");
		}
		Row row(*result.begin());//TypeID is unique
		descStr = (std::string)row["Description"];
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when getting synapse type description: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Type Error", errorString);
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ConnectionWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when getting synapse type description: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Synapse Type Error", errorString);
    }
	return descStr;
}


/*! Loads all of the connection groups in the database into the table. */
void ConnectionWidget::loadAllConnections(){
	//Initialise view vector and remove all rows from table
	connTable->setNumRows(0);
	viewVector.clear();
	
	//Load all connection groups, filtering out temporary virtual connections
	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query << "SELECT ConnGrpID FROM ConnectionGroups WHERE ConnType != "<<ConnectionType::TempVirtual;
                StoreQueryResult connResult = query.store();
                StoreQueryResult::iterator resIter;
		for(resIter = connResult.begin(); resIter != connResult.end(); resIter++){
			Row row(*resIter);
			unsigned int connectionID = Utilities::getUInt((std::string)row["ConnGrpID"]);
			loadConnection(connectionID);
		}
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"ConnectionWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when loading connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connections Error", errorString);
		exit(1);//Critical error
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"ConnectionWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when loading connections: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Connections Error", errorString);
		exit(1);//Critical error
    }
}


/*! Loads a single connection group from the database
	Default is that connections are hidden by default so do not add to viewVector 
	and use hide icon.	Will not load temporary virtual connections. */
void ConnectionWidget::loadConnection(unsigned int connectionGroupID){
	Query query = networkDBInterface->getQuery();
    query.reset();
    query << "SELECT FromNeuronGrpID, ToNeuronGrpID, ConnType, SynapseType FROM ConnectionGroups WHERE ConnGrpID = "<<connectionGroupID<<" AND ConnType != "<<ConnectionType::TempVirtual;
    StoreQueryResult connResult = query.store();
	Row connRow(*connResult.begin());//Should only be one result
	
	//Add new row to the table
	int currentRowNumber = connTable->numRows();
	connTable->insertRows(currentRowNumber, 1);
	
	//Need to extract the name of the from and to layers to make the table easier to read
	unsigned short cType = (unsigned short)connRow["ConnType"];
	int fromLayerID = (int)connRow["FromNeuronGrpID"];
	int toLayerID = (int)connRow["ToNeuronGrpID"];

	query.reset();
	query<<"SELECT Name FROM NeuronGroups WHERE NeuronGrpID = "<<fromLayerID;
        StoreQueryResult tempResult = query.store();
	Row tempFromRow(*tempResult.begin());
        string fromLayerName = (std::string)tempFromRow.at(0) + " [";
        fromLayerName += QString::number(fromLayerID).toStdString() + "]";
	query.reset();
	query<<"SELECT Name FROM NeuronGroups WHERE NeuronGrpID = "<<toLayerID;
	tempResult = query.store();
	mysqlpp::Row tempToRow(*tempResult.begin());
        string toLayerName = (std::string)tempToRow.at(0) + " [";
        toLayerName += QString::number(toLayerID).toStdString() + "]";
	
	//Fill Rows
	connTable->setPixmap(currentRowNumber, 0, *hidePixmap);
	
	connTable->setItem(currentRowNumber, 1, 
                new ConnectionCheckTableItem( connTable, fromLayerName.data()));//From layer
		
	connTable->setItem(currentRowNumber, 2, 
                new ConnectionTableItem(connTable, Q3TableItem::Never, toLayerName.data()));//To layer
		
	connTable->setItem(currentRowNumber, 3, 
		new ConnectionTableItem(connTable, Q3TableItem::Never, ConnectionType::getDescription(cType)));//Connection Type

	connTable->setItem(currentRowNumber, 4, 
                new ConnectionTableItem(connTable, Q3TableItem::Never, getSynapseTypeDescription(Utilities::getUShort((std::string)connRow["SynapseType"])).data()));//Synapse Type

	//Need to get size using neuron and connection tables
	//First do the query to get the size
	query.reset();
	query<<"SELECT COUNT(*) FROM Connections WHERE ConnGrpID = "<<connectionGroupID;
        StoreQueryResult connectionResult = query.store();//Execute the query
	Row connSizeRow(*connectionResult.begin());//Only one result since selecting count
	connTable->setItem(currentRowNumber, 5,
                new ConnectionTableItem(connTable, Q3TableItem::Never, ((std::string)connSizeRow.at(0)).data()));//Number of connections in this connection group
	
	//Set ID
	connTable->setItem(currentRowNumber, connGrpIDColumn, 
		new ConnectionTableItem(connTable, Q3TableItem::Never, QString::number(connectionGroupID)));

	//Set pixmap that user clicks on to view the parameters of the connection.
	connTable->setPixmap(currentRowNumber, paramCol, *paramPixmap);
}


/*! Removes the indicated connection group from the table. */
void ConnectionWidget::removeConnectionFromTable(unsigned int connectionGrpID){
	//Remove from table
	for(int i=0; i<connTable->numRows(); i++){
		unsigned int tempID = connTable->item(i, connGrpIDColumn)->text().toUInt();
		if(tempID == connectionGrpID){
			connTable->removeRow(i);
			break;
		}
	}
	//Remove from view vector
	for(vector<unsigned int>::iterator iter = viewVector.begin(); iter != viewVector.end(); iter++){
		if(*iter == connectionGrpID){
			viewVector.erase(iter);
			break;
		}
	}
}


