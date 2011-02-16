//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "Globals.h"
#include "ParametersDialog.h"
using namespace spikestream;


/*! Constructor */
AbstractExperimentWidget::AbstractExperimentWidget(QWidget* parent) : QWidget(parent){
	parametersBuilt = false;
}


/*! Destructor */
AbstractExperimentWidget::~AbstractExperimentWidget(){
}


/*----------------------------------------------------------*/
/*------               PROTECTED METHODS              ------*/
/*----------------------------------------------------------*/

/*! Builds the toolbar for the widget */
QToolBar* AbstractExperimentWidget::getToolBar(){
	QToolBar* tmpToolBar = new QToolBar(this);

	playAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"), "Start experiment", this);
	connect(playAction, SIGNAL(triggered()), this, SLOT(startExperiment()));
	tmpToolBar->addAction (playAction);

	stopAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png"), "Stop experiment", this);
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stopExperiment()));
	stopAction->setEnabled(false);
	tmpToolBar->addAction (stopAction);

	parametersButton = new QPushButton("Parameters");
	connect(parametersButton, SIGNAL(clicked()), this, SLOT(parametersButtonClicked()));
	parametersButton->setMaximumSize(100, 20);
	parametersButton->setMinimumSize(100, 20);
	tmpToolBar->addWidget(parametersButton);

	return tmpToolBar;
}


/*----------------------------------------------------------*/
/*------                 PRIVATE SLOTS                ------*/
/*----------------------------------------------------------*/

/*! Called when the parameters button is clicked. Sets the parameters. */
void AbstractExperimentWidget::parametersButtonClicked(){
	//Build parameter maps if this has not been done
	if(!parametersBuilt){
		buildParameters();
		parametersBuilt = true;
	}

	//Launch dialog to edit parameters
	try{
		ParametersDialog dialog(parameterInfoList, defaultParameterMap, parameterMap, this);
		if(dialog.exec() == QDialog::Accepted){
			parameterMap = dialog.getParameters();
		}
	}
	catch(SpikeStreamException& ex){
		qCritical()<<ex.getMessage();
	}
}


