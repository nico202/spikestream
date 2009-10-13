
//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "Globals.h"
#include "PluginManager.h"

//Qt includes
#include <QLabel>
#include <QDebug>
#include <QStackedWidget>


/*! Constructor */
AnalysisLoaderWidget::AnalysisLoaderWidget(QWidget* parent) : QWidget(parent) {
    //Create vertical box to organize layout
    mainVerticalBox = new QVBoxLayout(this);

    try{
	//Get list of available analysis plugins
	QString pluginPath = Globals::getSpikeStreamRoot() + "/plugins/analysis";
	PluginManager* pluginManager = new PluginManager(pluginPath);
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
	QStackedWidget* stackedWidget = new QStackedWidget();
	for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
	    pluginWidgetMap[*iter] = stackedWidget->addWidget(pluginManager->getPlugin(*iter));
	}

	//Add stacked widget to layout
	mainVerticalBox->addWidget(stackedWidget);

	//Connect combo changed signal to slot loading appropriate analysis widget
	connect(pluginsCombo, SIGNAL(currentIndexChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)) );

	//Add the currently selected widget to the layout
/*        if(!pluginList.isEmpty()){
	    currentAnalysisWidget = pluginWidgetMap[pluginsCombo->currentText()];
	    currentAnalysisWidget->setVisible(true);
	    mainVerticalBox->addWidget(currentAnalysisWidget);
	}*/
    }
    catch(SpikeStreamException* ex){
	qCritical()<<ex->getMessage();
    }
}


/*! Destructor */
AnalysisLoaderWidget::~AnalysisLoaderWidget(){
    qDebug()<<"Destroying AnalysisLoaderWidget";
}


/*---------------------------------------------------------------------------------*/
/*----------                   PRIVATE SLOTS                          -------------*/
/*---------------------------------------------------------------------------------*/

/*! Sets the current analysis widget */
void AnalysisLoaderWidget::showAnalysisWidget(){
    //Hide current widget and remove from layout
//    currentAnalysisWidget->setVisible(false);
//    mainVerticalBox->removeWidget(currentAnalysisWidget);
//
//    //Load new widget
//    currentAnalysisWidget = pluginWidgetMap[pluginsCombo->currentText()];
//    currentAnalysisWidget->setVisible(true);
//    mainVerticalBox->addWidget(currentAnalysisWidget);
}



