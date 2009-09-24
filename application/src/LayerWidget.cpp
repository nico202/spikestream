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
#include "LayerWidget.h"
#include "NetworkViewer_V2.h"
#include "LayerPropertiesDialog.h"
#include "Debug.h"
#include "Utilities.h"
#include "SpikeStreamMainWindow.h"
#include "NeuronGroup.h"

//Qt includes
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <q3accel.h>
#include <qlayout.h>
#include <qfile.h>
#include <qcursor.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <QPixmap>
#include <Q3PopupMenu>
#include <Q3VBoxLayout>

//Other includes
#include <iostream>
#include <vector>
#include "mysql++.h"
using namespace std;
using namespace mysqlpp;


/*! Constructor. */
LayerWidget::LayerWidget(QWidget *parent, DBInterface *netDBInter, DBInterface* devDBInter) : QWidget(parent, "Layer Widget"){
	//Keep reference to database interfaces 
	networkDBInterface = netDBInter;
	deviceDBInterface = devDBInter;
	
	//Create a layerManager to manage the neurons
	layerManager = new LayerManager(networkDBInterface, deviceDBInterface);
	
	//Set up pixmaps to control zoom
	showPixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/view.xpm");
	hidePixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/hide.xpm");
	zoomToPixmap = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/zoom_to_small.xpm");
	zoomToPixmapHighlight = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/zoom_to_highlight.xpm");
	zoomAbovePixmapHighlight = new QPixmap(SpikeStreamMainWindow::workingDirectory + "/images/zoom_above_highlight.xpm");
	
	//Set zoom layer row to -1 and click count to zero
	zoomLayerRow = -1;
	zoomClickCount = 0;
	
	//Create vertical layout to organise widget
        Q3VBoxLayout *verticalBox = new Q3VBoxLayout(this, 5, 5, "vertical");

	//Set up buttons
	Q3HBoxLayout *buttonBox = new Q3HBoxLayout();
	newLayerButt = new QPushButton("Add Neurons", this, "NewNeuronGrp");
	newLayerButt->setAccel(QKeySequence(Qt::CTRL + Qt::Key_N));
	connect (newLayerButt, SIGNAL(clicked()), this, SLOT(newLayer()));
	buttonBox->addWidget(newLayerButt);
	
	deleteButt = new QPushButton("Delete", this, "Delete");
	deleteButt->setAccel(QKeySequence(Qt::Key_Delete));
	connect (deleteButt, SIGNAL(clicked()), this, SLOT(deleteLayer()));
	buttonBox->addWidget(deleteButt);
	buttonBox->addStretch(5);
	
	verticalBox->addLayout(buttonBox);
	
	//Set up table
	layerTable = new Q3Table(0, 10, this);
	layerTable->setColumnReadOnly(0, true);
	layerTable->setColumnReadOnly(1, true);
	layerTable->setShowGrid(false);
	layerTable->setSorting(false);
	layerTable->setSelectionMode(Q3Table::NoSelection);
	layerTable->verticalHeader()->hide();
	layerTable->setLeftMargin(0);
	Q3Header * layerTableHeader = layerTable->horizontalHeader();
	layerTableHeader->setLabel(0, "");
	layerTable->setColumnWidth(0, 16);
	layerTableHeader->setLabel(1, "");
	layerTable->setColumnWidth(1, 16);
	layerTableHeader->setLabel( 2, "Name" );
	layerTable->setColumnWidth( 2, 200);
	layerTableHeader->setLabel( 3, "ID");
	layerTable->setColumnWidth( 3, 50);
	layerTableHeader->setLabel( 4, "Neuron Type");
	layerTable->setColumnWidth( 4, 200);
	layerTableHeader->setLabel( 5, "Width");
	layerTable->setColumnWidth( 5, 50);
	layerTableHeader->setLabel( 6, "Length");
	layerTable->setColumnWidth( 6, 50);
	layerTableHeader->setLabel( 7, "Size" );
	layerTable->setColumnWidth( 7, 50);
	layerTableHeader->setLabel( 8, "Position" );
	layerTable->setColumnWidth( 8, 100);
	layerTableHeader->setLabel( 9, "Task ID" );
	layerTable->setColumnWidth( 9, 100);
	
	//Connect layer table header clicked() slot to select all
	connect (layerTableHeader, SIGNAL(clicked(int)), this, SLOT(tableHeaderClicked(int)));
	
	//Need to set up table so that change in selection is reflected in 3D display
	connect (layerTable, SIGNAL(clicked(int, int, int, const QPoint &)), this, SLOT (tableClicked(int, int, int, const QPoint &)));
	
	//Now load the list of layers from the database
	addAllLayersToTable();

	verticalBox->addWidget(layerTable);
	
	//Set up popup menu to enable the user to edit a layer using a right mouse click
	layerPropertiesPopup = new Q3PopupMenu(this);
	layerPropertiesPopup->insertItem("Edit layer properties", this, SLOT(editLayerProperties()));
}


/*! Destructor. */
LayerWidget::~LayerWidget(){
	#ifdef MEMORY_DEBUG
		cout<<"DELETING LAYER WIDGET!"<<endl;
	#endif//MEMORY_DEBUG
	
	delete layerManager;
	delete showPixmap;
	delete hidePixmap;
	delete zoomToPixmap;
	delete zoomToPixmapHighlight;
	delete zoomAbovePixmapHighlight;
}


//--------------------------------------------------------------------------
//--------------------- PUBLIC METHODS -------------------------------------
//--------------------------------------------------------------------------

/*! Reloads neuron groups when database has changed. */
void LayerWidget::reloadNeuronGroups(){
	addAllLayersToTable();
}


/*! Need communication between layer widget and connection widget to enable layer widget
	to delete connections to layers that are being deleted. */
void LayerWidget::setConnectionWidget(ConnectionWidget* connWidg){
	connectionWidget = connWidg;
	layerManager->setConnectionWidget(connWidg);
}


/*! Called when simulation is initialised to disable editing. */
void LayerWidget::simulationDestroyed(){
	newLayerButt->setEnabled(true);
	deleteButt->setEnabled(true);
}


/*! Called when simulation is destroyed to enable editing. */
void LayerWidget::simulationInitialised(){
	newLayerButt->setEnabled(false);
	deleteButt->setEnabled(false);
}


//-----------------------------------------------------------------------
//--------------------------- SLOTS -------------------------------------
//-----------------------------------------------------------------------

/*! Deletes a layer from the table and instructs connection manager to delete the layer from the
	database along with all associated connections. */
void LayerWidget::deleteLayer(){
	QString confirmDeleteStr = "Do you want to delete the following neuron groups and their associated connection groups?\n";
	vector<unsigned int> deleteLayerIDs;
	//First get the layer id(s) for the layer(s) to be deleted
	for(int i=0; i<layerTable->numRows(); i++){
		Q3CheckTableItem * item = (Q3CheckTableItem*)layerTable->item(i, 2);
		if(item->isChecked()){
			unsigned int tempInt = layerTable->item(i, 3)->text().toUInt();
			deleteLayerIDs.push_back(tempInt);
			confirmDeleteStr += "   ";
			confirmDeleteStr += layerTable->item(i, 2)->text()+= " (ID: ";
			confirmDeleteStr += layerTable->item(i, 3)->text() += ")\n";
		}
	}
	//Check that user really wants to delete these layers
	if(deleteLayerIDs.size() > 0){
		if(!( QMessageBox::warning( this, "Confirm Delete Neuron Groups", confirmDeleteStr, "Yes", "No", 0, 0, 1 ))) {
			
			//Show dialog to inform user that deletion is in progress
			BusyDialog* busyDialog = new BusyDialog(SpikeStreamMainWindow::spikeStrMainWin, "Editing Neuron Groups");
			busyDialog->setModal(true);
			busyDialog->showDialog("Deleting neuron groups, please wait.");
			SpikeStreamMainWindow::spikeStreamApplication->processEvents();

			//Delete the layers
			try{
    			layerManager->deleteLayers(deleteLayerIDs);

				//Delete layer data from NetworkViewer
				vector<unsigned int>::iterator deleteIDsIter;
				for(deleteIDsIter = deleteLayerIDs.begin(); deleteIDsIter != deleteLayerIDs.end(); deleteIDsIter++){
					SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->deleteNeuronGroup(*deleteIDsIter);
					connectionWidget->deleteLayerConnections(*deleteIDsIter);
					removeLayerFromTable(*deleteIDsIter);
				}
				//Reset default clipping volume so that remaining neuron groups fit within it
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadDefaultClippingVolume();
				//SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->resetView();
				
				//Reload neuron groups and connection groups
				SpikeStreamMainWindow::spikeStrMainWin->reloadNeuronGroups();
				SpikeStreamMainWindow::spikeStrMainWin->reloadConnections();
			}
			catch (const BadQuery& er) {// Handle any query errors
				cerr<<"LayerWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Bad query when deleting layers: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Delete Layer Error", errorString);
			}
			catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
				cerr<<"LayerWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown deleting layers: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Delete Layer Error", errorString);
			}
			catch(std::exception& er){// Catch-all for std exceptions
				cerr<<"LayerWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown deleting layers: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Delete Layer Error", errorString);
			}

			//Delete busy dialog
			delete busyDialog;
    	}
	}
}


/*! Used to change the properties of a layer, except the width and length. */
void LayerWidget::editLayerProperties(){
	NeuronGroup origNeurGrpProps;
	
	//Double check that the activeRow is a sensible number
	if(layerTable->numRows() <= 0 || activeRow > (unsigned int)layerTable->numRows()){
		QMessageBox::critical( 0, "Edit Layer Error", "Active row is out of range.");
		return;
	}

	unsigned int neuronGrpID = 0;
	try{
		//Extract neuronGrpID from table
		neuronGrpID = Utilities::getUInt(layerTable->text(activeRow, 3).ascii());

		//Get all information from database
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT Name, NeuronType, X, Y, Z, Width, Length, Spacing FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID; //Write query string
                StoreQueryResult layerResult = query.store();
		Row row(*layerResult.begin());//Neuron group ids are unique
	
		//Fill neuron group holder
                origNeurGrpProps.name = ((std::string)row["Name"]).data();
		origNeurGrpProps.neuronType = Utilities::getUInt((std::string)row["NeuronType"]);
		origNeurGrpProps.width = Utilities::getUInt((std::string)row["Width"]);;
		origNeurGrpProps.length = Utilities::getUInt((std::string)row["Length"]);
		origNeurGrpProps.spacing =Utilities::getUInt((std::string)row["Spacing"]);
		origNeurGrpProps.xPos = Utilities::getUInt((std::string)row["X"]);
		origNeurGrpProps.yPos = Utilities::getUInt((std::string)row["Y"]);
		origNeurGrpProps.zPos = Utilities::getUInt((std::string)row["Z"]);
	}
    catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LayerWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when editing layer properties: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Edit Layer Error", errorString);
		return;
    }
    catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
        cerr<<"LayerWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown editing layer properties: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Edit Layer Error", errorString);
		return;
    }
	catch(std::exception& er){// Catch-all for std exceptions
        cerr<<"LayerWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown editing layer properties: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Edit Layer Error", errorString);
		return;
	}

	//Show dialog with information
	LayerPropertiesDialog *layPropDlg = new LayerPropertiesDialog(this, "Neuron Group Properties", true, origNeurGrpProps, networkDBInterface, deviceDBInterface);
	layPropDlg->setCaption( "Neuron Group Properties" );
	if ( layPropDlg->exec() == QDialog::Accepted ) {//Adjust layer properties
	
		//Get data from dialog
		NeuronGroup newNeurGrpProps = layPropDlg->getNeuronGroup();
		
		//Create bool to record any changes
		bool neuronGrpChanged = false;
		bool neuronGrpNameChanged = false;
		
		//Work through all the neuron properties and change all those which have been altered
		try{
			if(newNeurGrpProps.name != origNeurGrpProps.name){
				layerManager->setNeuronGrpName(neuronGrpID, newNeurGrpProps.name);
				neuronGrpNameChanged = true;
			}
				
			if(newNeurGrpProps.neuronType != origNeurGrpProps.neuronType){
				layerManager->setNeuronGrpType(neuronGrpID, newNeurGrpProps.neuronType);
				neuronGrpChanged = true;
			}
			
			//The next two changes need to be checked for conflicts with existing layers.
			QString errorString = "";
			if((newNeurGrpProps.spacing != origNeurGrpProps.spacing) || (newNeurGrpProps.xPos != origNeurGrpProps.xPos || newNeurGrpProps.yPos != origNeurGrpProps.yPos || newNeurGrpProps.zPos != origNeurGrpProps.zPos)){
				bool neurSpacingPositionChanged = layerManager->setNeuronGrpSpacingPosition(neuronGrpID, origNeurGrpProps, newNeurGrpProps);
				if(!neurSpacingPositionChanged)
					errorString += "New neuron spacing and/or position causes overlap with other layers.\n Left at original value.\n";
				else
					neuronGrpChanged = true;
			}
			//Show message dialog about any errors
			if(errorString != "")
				QMessageBox::warning(this, "Neuron Group Properties Conflict!", errorString);


			//Reload altered neuron group in network viewer and any affected connection groups
			if(neuronGrpChanged){
				//Reload neuron group
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->deleteNeuronGroup(neuronGrpID);
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadNeuronGroup(neuronGrpID, true);
				
				//Load default clipping volume so that remaining neuron groups fit within it
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadDefaultClippingVolume();
				//SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->resetView();
				
				//Set the view vector so the same neuron groups are visible as before
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setLayerView(viewVector);
				
				//Reload connection groups that connect to this neuron group
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setConnectionView(connectionWidget->getConnectionViewVector());
			}
			if(neuronGrpChanged || neuronGrpNameChanged){
				//Reload layer data into table
				removeLayerFromTable(neuronGrpID);
				addLayerToTable(neuronGrpID);
				
				//Update list of neuron groups in the rest of the application
				SpikeStreamMainWindow::spikeStrMainWin->reloadNeuronGroups();
			}
		}
		catch (const BadQuery& er) {// Handle any query errors
			cerr<<"LayerWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Bad query when editing layer properties: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Edit Layer Error", errorString);
			return;
		}
		catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
			cerr<<"LayerWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown editing layer properties: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Edit Layer Error", errorString);
			return;
		}
		catch(std::exception& er){// Catch-all for any other exceptions
			cerr<<"LayerWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown editing layer properties: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Edit Layer Error", errorString);
			return;
		}
	}
	//Clean up dialog
	delete layPropDlg;
}


/*! Creates a new layer. */
void LayerWidget::newLayer(){
	NeuronGroup neuronGrp; //Empty neuron group holder to reduce double coding in layer properties dialog to accommodate edit mode.
	LayerPropertiesDialog *layPropDlg = new LayerPropertiesDialog(this, "Neuron Group Properties", false, neuronGrp, networkDBInterface, deviceDBInterface);
	layPropDlg->setCaption( "Neuron Group Properties" );
	if ( layPropDlg->exec() == QDialog::Accepted ) {//Create new layer
	
		//Get neuronGrpHolder with layer details
		NeuronGroup neuronGrp = layPropDlg->getNeuronGroup();

		//Instruct layer manager to create layer
		int layerID = -1;
		try{
			layerID = layerManager->createLayer(neuronGrp);

			if(layerID < 0){//Method has returned a negative number or exception thrown, indicating that no layer has been created, probably due to conflict with existing layer
				QMessageBox::warning(this, "Layer Error!", "Layer has not been created due to an error or conflict with existing layers.");
			}
			else{//Layer has been created successfully
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadNeuronGroup(layerID, true);
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->loadDefaultClippingVolume();
				//SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->resetView();
				
				//Update list of neuron groups in the rest of the application
				SpikeStreamMainWindow::spikeStrMainWin->reloadNeuronGroups();
				
				//Add layer to layer table
				addLayerToTable(layerID);
			}
		}
		catch (const BadQuery& er) {// Handle any query errors
			cerr<<"LayerWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Bad query creating layer: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Create Layer Error", errorString);
		}
		catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
			cerr<<"LayerWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown creating layer: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Create Layer Error", errorString);
		}
		catch(std::exception& er){// Catch-all for std exceptions
			cerr<<"LayerWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
			QString errorString = "Exception thrown creating layer: \"";
			errorString += er.what();
			errorString += "\"";
			QMessageBox::critical( 0, "Create Layer Error", errorString);
		}
	}

	//Clean up dialog
	delete layPropDlg;
}


/*! Displays the popup menu if the right mouse button has been clicked.
	Otherwise controls the zoom and viewing of the layer. */
void LayerWidget::tableClicked(int row, int col, int button, const QPoint &){
	//Show editing popup menu if it is the right button
	if(button == Qt::RightButton){
		//First store active row so that information about the layer that is being edited can be accessed
		activeRow = row;
	
		//Show popup menu 
		layerPropertiesPopup->exec(QCursor::pos());
	}

	//Otherwise check to see if the view changing pixmaps have been clicked
	else if(button == Qt::LeftButton){
		if(col == 0){//View pixmap has been clicked
			//Get layerID
			unsigned int layerID = 0;
			try{
				layerID = Utilities::getUInt(layerTable->item(row, 3)->text().ascii());
			}
			catch(std::exception& er){// Catch-all for any other exceptions
				cerr<<"LayerWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
				QString errorString = "Exception thrown extracting neuron group ID: \"";
				errorString += er.what();
				errorString += "\"";
				QMessageBox::critical( 0, "Neuron Group ID Error", errorString);
				return;
			}
					
			//Find out if it is already part of the viewLayerIDVector
			bool layerIDFound = false;
			for(vector<unsigned int>::iterator iter = viewVector.begin(); iter != viewVector.end(); iter++){
				if(*iter == layerID){//Layer was already visible, so need to make it invisible
					layerIDFound = true;
					viewVector.erase(iter);
					layerTable->setPixmap(row, 0, *hidePixmap);
					SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setLayerView(viewVector);
					break;
				}
			}
			if(!layerIDFound){//Not already showing, so show layer
				layerTable->setPixmap(row, 0, *showPixmap);
				viewVector.push_back(layerID);
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setLayerView(viewVector);
			}
		}
		else if(col == 1){//Zoom pixmap has been clicked
			if(row == zoomLayerRow && zoomClickCount == 1){//Second click on the same row. Want to zoom above layer
				unsigned int layerID = layerTable->item(row, 3)->text().toUInt();
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->zoomAboveLayer(layerID);
				layerTable->setPixmap(row, 1, *zoomAbovePixmapHighlight);
				zoomClickCount++;
			}
			else if(row == zoomLayerRow && zoomClickCount == 2){//Third click on same row. Want to reset view and zoom to all layers
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->zoomToLayer(0);
				layerTable->setPixmap(row, 1, *zoomToPixmap);
				zoomClickCount = 0;
				zoomLayerRow = -1;
			}
			else{//Different row has been clicked
				unsigned int layerID = layerTable->item(row, 3)->text().toUInt();
				SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->zoomToLayer(layerID);
				
				//Change pixmap to hightlighted pixmap
				for(int i=0; i<layerTable->numRows(); i++){
					if(i==row)
						layerTable->setPixmap(i, 1, *zoomToPixmapHighlight);
					else
						layerTable->setPixmap(i, 1, *zoomToPixmap);
				}
				
				//Record which layer is selected
				zoomLayerRow = row;
				zoomClickCount = 1;
			}
		}
	}
}


/*! Used to select all layers or deselect all layers for deletion or zooming. */
void LayerWidget::tableHeaderClicked(int colNumber){
	if(colNumber == 0){//View header has been clicked
		//All rows are already selected, so deselect
		if(viewVector.size() == (unsigned int)layerTable->numRows()){
			viewVector.clear();
			SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setLayerView(viewVector);
			
			//Change pixmaps to hide
			for(int i=0; i<layerTable->numRows(); i++){
				layerTable->setPixmap(i, 0, *hidePixmap);
			}
		}
		//Not all rows selected, so select all
		else{
			viewVector.clear();
			for(int i=0; i<layerTable->numRows(); i++){
				unsigned int tempLayerID = layerTable->item(i, 3)->text().toUInt();
				viewVector.push_back(tempLayerID);
				layerTable->setPixmap(i, 0, *showPixmap);
			}
			SpikeStreamMainWindow::spikeStrMainWin->getNetworkViewer()->setLayerView(viewVector);
		}
	}
	else if(colNumber ==2){//Header above check boxes has been clicked
		//First count the number of selected rows
		int selectedRowCount = 0;
		for(int i=0; i<layerTable->numRows(); i++){
			Q3CheckTableItem * item = (Q3CheckTableItem*)layerTable->item(i, 2);
			if(item->isChecked())
				selectedRowCount++;
		}
		//If all rows are selected want to deselect rows
		if(selectedRowCount == layerTable->numRows()){
			for(int i=0; i<layerTable->numRows(); i++){//Deselect all rows
				Q3CheckTableItem * item = (Q3CheckTableItem*)layerTable->item(i, 2);
				item->setChecked(false);
			}
		}
		else{//Select all rows
			for(int i=0; i<layerTable->numRows(); i++){
				Q3CheckTableItem * item = (Q3CheckTableItem*)layerTable->item(i, 2);
				item->setChecked(true);
			}
		}
	}
}


//--------------------------------------------------------------------------
//-------------------------- PRIVATE METHODS -------------------------------
//--------------------------------------------------------------------------

/*! Works through all the layers and adds them to the table. */
void LayerWidget::addAllLayersToTable(){
	//First clear the table
	clearTable();
	try{
		Query query = networkDBInterface->getQuery();	//Get query object
		query.reset();// Reset the query object just in case it has already been used
		query<<"SELECT NeuronGrpID FROM NeuronGroups";
                StoreQueryResult layerResult = query.store();
                StoreQueryResult::iterator resIter;
		for (resIter = layerResult.begin(); resIter != layerResult.end(); ++resIter) {
			Row row(*resIter);
			unsigned int neuronGrpID = Utilities::getUInt((std::string)row.at(0));
			addLayerToTable(neuronGrpID);
		}
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LayerWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"LayerWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"LayerWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
}


/*! Adds an individual layer to the table. */
void LayerWidget::addLayerToTable(unsigned int neuronGrpID){
	//First add neuronGrpID to view vector. Default is that layers are visible
	viewVector.push_back(neuronGrpID);

	try{
		Query query = networkDBInterface->getQuery();
		query.reset();
		query << "SELECT Name, NeuronType, X, Y, Z, Width, Length, Spacing, TaskID FROM NeuronGroups WHERE NeuronGrpID = "<<neuronGrpID; //Write query string
                StoreQueryResult layerResult = query.store();
		Row row(*layerResult.begin());//Neuron group ids are unique
		
		//Add new row to the table
		int currentRow = layerTable->numRows();
		layerTable->insertRows(currentRow, 1);
		
		//Fill Row
		layerTable->setPixmap(currentRow, 0, *showPixmap);
		layerTable->setPixmap(currentRow, 1, *zoomToPixmap);
		
                Q3CheckTableItem *checkTableItem = new Q3CheckTableItem( layerTable, ((std::string)row["Name"]).data() );
		//checkTableItem->setChecked(true);
		layerTable->setItem( currentRow, 2, checkTableItem);//Name
			
		layerTable->setItem( currentRow, 3, 
			new Q3TableItem(layerTable, Q3TableItem::Never, QString::number(neuronGrpID)));//ID
			
		layerTable->setItem(currentRow, 4, 
			new Q3TableItem(layerTable, Q3TableItem::Never, getNeuronTypeDescription(Utilities::getUShort((std::string)row["NeuronType"]))));//Type
					
		layerTable->setItem(currentRow, 5,
                        new Q3TableItem(layerTable, Q3TableItem::Never, ((std::string)row["Width"]).data()));//Width
			
		layerTable->setItem(currentRow, 6, 
                        new Q3TableItem(layerTable, Q3TableItem::Never, ((std::string)row["Length"]).data()));//Length
		
		//Next get size from other Table for id
		query.reset();
		query << "SELECT * from Neurons WHERE NeuronGrpID = \""<<neuronGrpID<<"\"";
                StoreQueryResult neuronResult = query.store();
		layerTable->setItem( currentRow, 7, 
			new Q3TableItem(layerTable, Q3TableItem::Never, QString::number(neuronResult.size())));//Size
			
                string positionString = "(" + (std::string)row["X"] + ", " + (std::string)row["Y"] + ", " + (std::string)row["Z"] + ")";
		layerTable->setItem(currentRow, 8, 
                        new Q3TableItem(layerTable, Q3TableItem::Never, positionString.data()));//Position
		
		layerTable->setItem( currentRow, 9, 
                        new Q3TableItem(layerTable, Q3TableItem::Never, ((std::string)row["TaskID"]).data()));//Processing ID
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LayerWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"LayerWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
	catch(std::exception& er){// Catch-all for std exceptions
		cerr<<"LayerWidget: STD EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown adding layer to table: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Add Layer Error", errorString);
	}
}


/*! Clears the table and resets the appropriate variables. */
void LayerWidget::clearTable(){
	layerTable->setNumRows(0);
	viewVector.clear();
		
	//Reset zoom and click counts
	zoomClickCount = 0;
	zoomLayerRow = -1;
}


/*! Returns the description of a particular neuron type. */

QString LayerWidget::getNeuronTypeDescription(unsigned short neuronTypeID){
	try {
		Query query = networkDBInterface->getQuery();
		query.reset();
		query<<"SELECT Description FROM NeuronTypes WHERE TypeID = "<<neuronTypeID;
                StoreQueryResult result = query.store();
		if(result.size() != 1){
                        return QString("Unknown");
		}
		Row row(*result.begin());//TypeID is unique
                return QString(((std::string)row["Description"]).data());
	}
	catch (const BadQuery& er) {// Handle any query errors
		cerr<<"LayerWidget: MYSQL QUERY EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Bad query when getting neuron type description: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Type Error", errorString);
                return QString("Unknown");
	}
	catch (const Exception& er) {// Catch-all for any other MySQL++ exceptions
		cerr<<"LayerWidget: MYSQL EXCEPTION \""<<er.what()<<"\""<<endl;
		QString errorString = "Exception thrown when getting neuron type description: \"";
		errorString += er.what();
		errorString += "\"";
		QMessageBox::critical( 0, "Neuron Type Error", errorString);
                return QString("Unknown");
	}
}


/*! Removes the indicated neuronGrp from the table. */
void LayerWidget::removeLayerFromTable(unsigned int neuronGrpID){
	//Remove from table
	for(int i=0; i<layerTable->numRows(); i++){
		unsigned int tempID = layerTable->item(i, 3)->text().toUInt();
		if(tempID == neuronGrpID){
			layerTable->removeRow(i);
			break;
		}
	}
	//Remove from view vector
	for(vector<unsigned int>::iterator iter = viewVector.begin(); iter != viewVector.end(); iter++){
		if(*iter == neuronGrpID){
			viewVector.erase(iter);
			break;
		}
	}
}



