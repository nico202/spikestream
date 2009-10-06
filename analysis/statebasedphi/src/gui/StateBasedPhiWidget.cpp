//SpikeStream includes
#include "StateBasedPhiWidget.h"
using namespace spikestream;

//Qt includes
#include <QLabel>
#include <QLayout>
#include <QDebug>


//Functions for dynamic library loading
extern "C" {
    /*! Creates a StateBasedPhiWidget class when library is dynamically loaded. */
    StateBasedPhiWidget* getClass(){
	return new StateBasedPhiWidget();
    }

    /*! Returns a sensible name for this widget */
    QString getName(){
	return QString("State-based Phi Analysis");
    }
}


/*! Constructor */
StateBasedPhiWidget::StateBasedPhiWidget(QWidget *parent) : QWidget(parent){
    QVBoxLayout *mainVerticalBox = new QVBoxLayout(this);
    mainVerticalBox->addWidget(new QLabel("State based phi analysis"));
}


/*! Destructor */
StateBasedPhiWidget::~StateBasedPhiWidget(){
    //qDebug()<<"DestroyingStateBasedPhiWidget";
}

