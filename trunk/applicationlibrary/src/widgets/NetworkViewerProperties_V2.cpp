//SpikeStream includes
#include "NetworkViewerProperties_V2.h"
using namespace spikestream;

//Qt includes
#include <QRadioButton>
#include <QButtonGroup>
#include <QLayout>


/*! Constructor */
NetworkViewerProperties_V2::NetworkViewerProperties_V2(QWidget* parent) : QWidget(parent){
    //Main vertical layout
    QVBoxLayout* mainVerticalBox = new QVBoxLayout(this);

    //Button group to set connection mode
    QButtonGroup* conButGroup = new QButtonGroup();
    QRadioButton* allConsButt = new QRadioButton("All selected connections");
    allConsButt->setChecked(true);
    allConsButt->setEnabled(true);
    conButGroup->addButton(allConsButt);
    mainVerticalBox->addWidget(allConsButt);

    QRadioButton* conSingleNeurButt = new QRadioButton("Single neuron connection");
    conSingleNeurButt->setChecked(false);
    conSingleNeurButt->setEnabled(true);
    conButGroup->addButton(conSingleNeurButt);
    mainVerticalBox->addWidget(conSingleNeurButt);

    QRadioButton* conBetweenNeurButt = new QRadioButton("Between neuron connection");
    conBetweenNeurButt->setChecked(false);
    conBetweenNeurButt->setEnabled(true);
    conButGroup->addButton(conBetweenNeurButt);
    mainVerticalBox->addWidget(conBetweenNeurButt);

}


/*! Destructor */
NetworkViewerProperties_V2::~NetworkViewerProperties_V2(){
}
