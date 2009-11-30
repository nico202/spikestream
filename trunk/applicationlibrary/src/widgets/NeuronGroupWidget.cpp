//SpikeStream includes
#include "NeuronGroupWidget.h"
#include "PluginsDialog.h"
using namespace spikestream;

//Qt includes
#include <QLayout>
#include <QPushButton>


/*! Constructor */
NeuronGroupWidget::NeuronGroupWidget(QWidget* parent)  : QWidget(parent) {
    //Vertical box to organize layout
    QVBoxLayout* verticalBox = new QVBoxLayout(this, 2, 2);

    //Add button to launch add neurons dialog
    QPushButton* addNeuronsButton = new QPushButton("Add Neurons");
    addNeuronsButton->setMaximumSize(150, 30);
    connect (addNeuronsButton, SIGNAL(clicked()), this, SLOT(addNeurons()));
    verticalBox->addWidget(addNeuronsButton);
}


/*! Destructor */
NeuronGroupWidget::~NeuronGroupWidget(){
}


/*--------------------------------------------------------*/
/*-------             PRIVATE SLOTS                -------*/
/*--------------------------------------------------------*/

/*! Shows plugins dialog configured to load up available plugins for adding neurons */
void NeuronGroupWidget::addNeurons(){
    PluginsDialog* pluginsDialog = new PluginsDialog(this, "/plugins/neurons", "Add Neurons");
    pluginsDialog->exec();
    delete pluginsDialog;
}









