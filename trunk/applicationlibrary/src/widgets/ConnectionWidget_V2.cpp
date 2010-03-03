//SpikeStream includes
#include "ConnectionWidget_V2.h"
#include "ConnectionGroupModel.h"
#include "ConnectionGroupTableView.h"
#include "PluginsDialog.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QPushButton>


/*! Constructor */
ConnectionWidget_V2::ConnectionWidget_V2(QWidget* parent)  : QWidget(parent) {
	//Vertical box to organize layout
	QVBoxLayout* verticalBox = new QVBoxLayout(this);

	//Add button to launch add neurons dialog
	QPushButton* addConnectionsButton = new QPushButton("Add Connections");
	addConnectionsButton->setMaximumSize(150, 30);
	connect (addConnectionsButton, SIGNAL(clicked()), this, SLOT(addConnections()));
	verticalBox->addWidget(addConnectionsButton);

	//Construct table view and model
	QAbstractTableModel* connGroupModel = new ConnectionGroupModel();
	QTableView* connGroupView = new ConnectionGroupTableView(this, connGroupModel);
	verticalBox->addWidget(connGroupView);
}


/*! Destructor */
ConnectionWidget_V2::~ConnectionWidget_V2(){
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Shows plugins dialog configured to load up available plugins for adding neurons */
void ConnectionWidget_V2::addConnections(){
	PluginsDialog* pluginsDialog = new PluginsDialog(this, "/plugins/neurons", "Add Neurons");
	pluginsDialog->exec();
	delete pluginsDialog;
}










