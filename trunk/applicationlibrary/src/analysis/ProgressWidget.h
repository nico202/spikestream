#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

//Qt includes
#include <QLabel>
#include <QLayout>
#include <QHash>
#include <QProgressBar>
#include <QWidget>
#include <QMutex>

namespace spikestream {

    class ProgressWidget : public QWidget {
	Q_OBJECT

	public:
	    ProgressWidget(QWidget* parent);
	    ~ProgressWidget();
	    void reset();


	private slots:
	    void timeStepComplete(unsigned int timeStep);
	    void updateProgress(const QString& msg, unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);


	private:
	    //===================  VARIABLES  ======================
	    /*! Holds a reference to each progress bar to update the progress */
	    QHash<unsigned int, QProgressBar*> progressBarMap;

	    /*! Holds a reference to each progress details label to update the progress */
	    QHash<unsigned int, QLabel*> progressDetailsMap;

	    /*! The layout organizing the widget */
	    QGridLayout* gridLayout;

	    /*! Prevents multiple threads accessing the slots simultaneously */
	    QMutex mutex;


	    //====================  METHODS  =======================
		void addProgressBar(unsigned int timeStep, unsigned int stepsCompleted, unsigned int totalSteps);
    };

}

#endif//PROGRESSWIDGET_H
