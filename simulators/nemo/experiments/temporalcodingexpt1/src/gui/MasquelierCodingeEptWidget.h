#ifndef MASQUELIERCODINGEPTWIDGET_H
#define MASQUELIERCODINGEPTWIDGET_H

//SpikeStream includes
#include "AbstractExperimentWidget.h"
#include "MasquelierCodingeEptManager.h"
#include "NemoWrapper.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QHash>
#include <QTextEdit>

namespace spikestream {

        /*! Graphical interface for carrying out a pop1 experiment */
        class MasquelierCodingeEptWidget : public AbstractExperimentWidget {
                Q_OBJECT

                public:
                        MasquelierCodingeEptWidget(QWidget* parent = 0);
                        ~MasquelierCodingeEptWidget();
                        void setWrapper(void *wrapper);

                private slots:
                        void managerFinished();
                        void startExperiment();
                        void stopExperiment();
                        void updateStatus(QString statusMsg);

                protected:
                        void buildParameters();

                private:
                        //=====================  VARIABLES  ====================
                        /*! Wrapper of NeMo that is used in the experiments. */
                        NemoWrapper* nemoWrapper;

                        /*! Manager that runs the experiments */
                        MasquelierCodingeEptManager* masquelierCodingeEptManager;

                        /*! Widget holding messages from the manager */
                        QTextEdit* statusTextEdit;


                        //=====================  METHODS  ======================
                        void checkNetwork();

        };

}

#endif//MASQUELIERCODINGEPTWIDGET