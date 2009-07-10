
//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "Globals.h"
#include "PluginManager.h"

//Qt includes
#include <QLabel>
#include <QDebug>


/*! Constructor */
AnalysisLoaderWidget::AnalysisLoaderWidget(QWidget* parent) : QWidget(parent) {
    //Create vertical box to organize layout
    mainVerticalBox = new QVBoxLayout(this);

    try{
        //Get list of available analysis plugins
        QString pluginPath = Globals::getRootDirectory() + "/plugins/analysis";
        PluginManager* pluginManager = new PluginManager(this, pluginPath);
        QStringList pluginList = pluginManager->getPluginNames();

        //Store the widgets so that state is preserved when the combo is changed
        for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
            pluginWidgetMap[*iter] = pluginManager->getPlugin(*iter);
            pluginWidgetMap[*iter]->setVisible(false);//Has been created with this widget as parent so all widgets may appear
        }
/*
        //Add list to combo box
        pluginsCombo = new QComboBox(this);
        pluginsCombo->addItems(pluginList);

        //Add combo to layout
        QHBoxLayout *comboBox = new QHBoxLayout();
        comboBox->addWidget(new QLabel("Analysis plugins: "));
        comboBox->addWidget(pluginsCombo);
        comboBox->addStretch(5);
        mainVerticalBox->addLayout(comboBox);
*/
        //Connect combo changed signal to slot loading appropriate analysis widget
       // connect( pluginsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setAnalysisWidget()) );

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
void AnalysisLoaderWidget::setAnalysisWidget(){
    //Hide current widget and remove from layout
    currentAnalysisWidget->setVisible(false);
    mainVerticalBox->removeWidget(currentAnalysisWidget);

    //Load new widget
    currentAnalysisWidget = pluginWidgetMap[pluginsCombo->currentText()];
    currentAnalysisWidget->setVisible(true);
    mainVerticalBox->addWidget(currentAnalysisWidget);
}



