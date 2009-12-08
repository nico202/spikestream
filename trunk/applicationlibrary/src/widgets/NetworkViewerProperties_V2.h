#ifndef NETWORKVIEWERPROPERTIES_V2
#define NETWORKVIEWERPROPERTIES_V2

//Qt includes
#include <QComboBox>
#include <QLabel>
#include <QWidget>
#include <QRadioButton>

namespace spikestream {

    class NetworkViewerProperties_V2 : public QWidget {
	Q_OBJECT

	public:
	    NetworkViewerProperties_V2(QWidget* parent);
	    ~NetworkViewerProperties_V2();

	private slots:
	    void fromToSelectionChanged(int index);
	    void networkDisplayChanged();
	    void posNegSelectionChanged(int index);


	private:
	    //=======================  VARIABLES  =======================
	    /*! Indicates that we are showing all selected connections */
	    QRadioButton* allConsButt;

	    /*! Indicates that we are showing connections to/from a single neuron */
	    QRadioButton* conSingleNeurButt;

	    /*! Indicates that we are showing connections between two neurons */
	    QRadioButton* conBetweenNeurButt;

	    /*! Label showing the neuron id when a single neuron is selected */
	    QLabel* singleNeuronIDLabel;

	    /*! Allows user to filter by direction of connection */
	    QComboBox* fromToCombo;

	    /*! Label showing id of from neuron in between mode */
	    QLabel* fromNeuronIDLabel;

	    /*! Label showing to neuron id in between mode */
	    QLabel* toNeuronIDLabel;

	    /*! Allows user to filter connections by connection weight */
	    QComboBox* posNegCombo;

	    /*! Keep reference to label saying "from" to enable and disable it */
	    QLabel* fromLabel;

	    /*! Keep reference to label saying "to" to enable and disable it */
	    QLabel* toLabel;

	    //=======================  METHODS  =========================
	    void showAllConnections();
	    void showBetweenConnections();
	    void showSingleConnections();
    };

}

#endif// NETWORKVIEWERPROPERTIES_V2

