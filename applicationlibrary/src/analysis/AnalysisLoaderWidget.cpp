//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "AbstractAnalysisWidget.h"
#include "Globals.h"
#include "PluginManager.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QDebug>


/*! Constructor */
AnalysisLoaderWidget::AnalysisLoaderWidget(QWidget* parent) : QWidget(parent) {
	//Create vertical box to organize layout
	mainVerticalBox = new QVBoxLayout(this);

	try{
		//Get list of available analysis plugins
		QString pluginPath = Globals::getSpikeStreamRoot() + "/plugins/analysis";
		PluginManager* pluginManager = new PluginManager(pluginPath);
		pluginManager->loadPlugins();
		QStringList pluginList = pluginManager->getPluginNames();

		//Add list to combo box
		QComboBox* pluginsCombo = new QComboBox(this);
		pluginsCombo->addItems(pluginList);

		//Add combo to layout
		QHBoxLayout *comboBox = new QHBoxLayout();
		comboBox->addWidget(new QLabel("Analysis plugins: "));
		comboBox->addWidget(pluginsCombo);
		comboBox->addStretch(5);
		mainVerticalBox->addLayout(comboBox);

		//Add the widgets to a stacked widget
		stackedWidget = new QStackedWidget();
		for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
			pluginWidgetMap[*iter] = stackedWidget->addWidget(pluginManager->getPlugin(*iter));
		}

		//Add stacked widget to layout
		mainVerticalBox->addWidget(stackedWidget);

		//Connect combo changed signal to slot loading appropriate analysis widget
		connect(pluginsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(showAnalysisWidget(int)) );
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


/*! Destructor */
AnalysisLoaderWidget::~AnalysisLoaderWidget(){
}


/*------------------------------------------------------------*/
/*------               PRIVATE SLOTS                    ------*/
/*------------------------------------------------------------*/

/*! Requests current widget to hide its results - particularly in the network viewer.
	Sets the current analysis widget */
void AnalysisLoaderWidget::showAnalysisWidget(int layerID){
	if(layerID != stackedWidget->currentIndex()){
		((AbstractAnalysisWidget*)stackedWidget->currentWidget())->hideAnalysisResults();
		stackedWidget->setCurrentIndex(layerID);
	}
}



