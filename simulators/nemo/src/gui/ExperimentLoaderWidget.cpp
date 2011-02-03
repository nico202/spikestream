//SpikeStream includes
#include "ExperimentLoaderWidget.h"
#include "Globals.h"
#include "PluginManager.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QDebug>


/*! Constructor */
ExperimentLoaderWidget::ExperimentLoaderWidget(QString pluginPath, QWidget* parent) : QWidget(parent) {
	//Create vertical box to organize layout
	mainVerticalBox = new QVBoxLayout(this);

	try{
		//Get list of available analysis plugins
		PluginManager* pluginManager = new PluginManager(pluginPath);
		pluginManager->loadPlugins();
		QStringList pluginList = pluginManager->getPluginNames();

		//Add list to combo box
		QComboBox* pluginsCombo = new QComboBox(this);
		pluginsCombo->addItems(pluginList);

		//Add combo to layout
		QHBoxLayout *comboBox = new QHBoxLayout();
		comboBox->addWidget(new QLabel("Experiment plugins: "));
		comboBox->addWidget(pluginsCombo);
		comboBox->addStretch(5);
		mainVerticalBox->addLayout(comboBox);

		//Add the widgets to a stacked widget
		stackedWidget = new QStackedWidget();
		for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
			QWidget* tmpWidget = pluginManager->getPlugin(*iter);
			abstractExperimentList.append((AbstractExperimentWidget*)tmpWidget);
			pluginWidgetMap[*iter] = stackedWidget->addWidget(tmpWidget);
		}

		//Add stacked widget to layout
		mainVerticalBox->addWidget(stackedWidget);

		//Connect combo changed signal to slot loading appropriate analysis widget
		connect(pluginsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(showExperimentWidget(int)) );
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Destructor */
ExperimentLoaderWidget::~ExperimentLoaderWidget(){
}


/*------------------------------------------------------------*/
/*------               PRIVATE SLOTS                    ------*/
/*------------------------------------------------------------*/

/*! Requests current widget to hide its results - particularly in the network viewer.
	Sets the current analysis widget */
void ExperimentLoaderWidget::showExperimentWidget(int layerID){
	if(layerID != stackedWidget->currentIndex()){
		stackedWidget->setCurrentIndex(layerID);
	}
}



