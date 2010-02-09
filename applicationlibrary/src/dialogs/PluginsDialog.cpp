//SpikeStream includes
#include "Globals.h"
#include "PluginManager_V2.h"
#include "PluginsDialog.h"
using namespace spikestream;

//Qt includes
#include <QComboBox>
#include <QLayout>
#include <QStackedWidget>

/*! Constructor */
PluginsDialog::PluginsDialog(QWidget* parent, const QString pluginFolder, const QString title) : QDialog(parent){
	//Set caption
	this->setCaption(title);

	//Create vertical box to organize layout
	QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);

	try{
		//Get list of available analysis plugins
		QString pluginPath = Globals::getSpikeStreamRoot() + pluginFolder;
		PluginManager_V2* pluginManager = new PluginManager_V2(pluginPath);
		QStringList pluginList = pluginManager->getPluginNames();

		//Add list to combo box
		QComboBox* pluginsCombo = new QComboBox(this);
		pluginsCombo->addItems(pluginList);

		//Add combo to layout
		QHBoxLayout *comboBox = new QHBoxLayout();
		comboBox->addWidget(new QLabel("Available plugins: "));
		comboBox->addWidget(pluginsCombo);
		comboBox->addStretch(5);
		mainVerticalBox->addLayout(comboBox);

		//Add the widgets to a stacked widget
		QStackedWidget* stackedWidget = new QStackedWidget();
		for(QList<QString>::iterator iter = pluginList.begin(); iter != pluginList.end(); ++iter){
			stackedWidget->addWidget(pluginManager->getPlugin(*iter));
		}

		//Add stacked widget to layout
		mainVerticalBox->addWidget(stackedWidget);

		//Connect combo changed signal to slot loading appropriate analysis widget
		connect(pluginsCombo, SIGNAL(currentIndexChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)) );

		//Add Ok and Cancel buttons
		QHBoxLayout* buttonBox = new QHBoxLayout();
		QPushButton* okButton = new QPushButton("Ok");
		connect (okButton, SIGNAL(clicked()), this, SLOT(accept()));
		buttonBox->addWidget(okButton);
		QPushButton* cancelButton = new QPushButton("Cancel");
		buttonBox->addWidget(cancelButton);
		connect (cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
		mainVerticalBox->addLayout(buttonBox);

	}
	catch(SpikeStreamException* ex){
		qCritical()<<ex->getMessage();
	}
}


/*! Destructor */
PluginsDialog::~PluginsDialog(){
}




