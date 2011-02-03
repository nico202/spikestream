//SpikeStream includes
#include "AbstractExperimentWidget.h"
using namespace spikestream;


/*! Constructor */
AbstractExperimentWidget::AbstractExperimentWidget(QWidget* parent) : QWidget(parent){
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

	QAction* playAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/play.png"), "Start experiment", this);
	connect(playAction, SIGNAL(triggered()), this, SLOT(startExperiment()));
	tmpToolBar->addAction (playAction);

	QAction* stopAction = new QAction(QIcon(Globals::getSpikeStreamRoot() + "/images/stop.png"), "Stop experiment", this);
	connect(stopAction, SIGNAL(triggered()), this, SLOT(stopExperiment()));
	stopAction->setEnabled(false);
	tmpToolBar->addAction (stopAction);

	return tmpToolBar;
}


