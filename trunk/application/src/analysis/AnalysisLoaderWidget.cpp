
//SpikeStream includes
#include "AnalysisLoaderWidget.h"
#include "Globals.h"
#include "PluginManager.h"

//Qt includes
#include <QLabel>
#include <QLayout>
#include <QDebug>
#include <QComboBox>

/*! Constructor */
AnalysisLoaderWidget::AnalysisLoaderWidget(QWidget* parent) : QWidget(parent) {
    try{
        //Get list of available analysis plugins
        QString pluginPath = Globals::getRootDirectory() + "/plugins/analysis";
        PluginManager* pluginManager = new PluginManager(this, pluginPath);
        QStringList pluginList = pluginManager->getPluginNames();

        //Add list to combo box
        QComboBox* pluginsCombo = new QComboBox(this);
        pluginsCombo->addItems(pluginList);

        //Add combo to layout
        QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);
        QHBoxLayout *comboBox = new QHBoxLayout();
        comboBox->addWidget(new QLabel("Analysis plugins: "));
        comboBox->addWidget(pluginsCombo);
        comboBox->addStretch(5);
        mainVerticalBox->addLayout(comboBox);

        //Add the currently selected widget to the layout
        if(!pluginList.isEmpty()){
            QWidget* currentWidget = pluginManager->getPlugin(pluginsCombo->currentText());
            mainVerticalBox->addWidget(currentWidget);
        }
        mainVerticalBox->addStretch(5);
    }
    catch(SpikeStreamException* ex){
        qCritical()<<ex->getMessage();
    }
}


/*! Destructor */
AnalysisLoaderWidget::~AnalysisLoaderWidget(){
}


