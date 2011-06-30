#ifndef MASQUELIERCODINGEPTMANAGER_H
#define MASQUELIERCODINGEPTMANAGER_H

//SpikeStream includes
#include "NemoWrapper.h"
#include "SpikeStreamThread.h"
#include "SpikeStreamTypes.h"

//Qt includes
#include <QString>


namespace spikestream {

        /*! Runs selected experiments on network exploring how temporal code can be used
                as part of pattern learning in a network. */
        class MasquelierCodingeEptManager : public SpikeStreamThread {
                Q_OBJECT

                public:
                        MasquelierCodingeEptManager();
                        ~MasquelierCodingeEptManager();
                        void run();
                        void startExperiment(NemoWrapper* nemoWrapper, QHash<QString, double>& parameterMap);

                signals:
                        void statusUpdate(QString statusMsg);

                private:
                        //=====================  VARIABLES  =====================
                        /*! Wrapper of simulation */
                        NemoWrapper* nemoWrapper;

                        /*! Seed for random number generator */
                        int randomSeed;

                        /*! ID of experiment that is carried out. */
                        int experimentNumber;

                        static const int NO_EXPERIMENT = -1;
                        static const int EXPERIMENT1 = 0;

                        /*! Neuron group to inject patterns into */
                        NeuronGroup* inputNeuronGroup;

                        /*! Output Neuron */
                        neurid_t outputNeuron1ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron2ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron3ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron4ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron5ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron6ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron7ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron8ID;

                        /*! Output Neuron */
                        neurid_t outputNeuron9ID;

                        /*! Time to pause between sections of the experiment */
                        int pauseInterval_ms;

                        /*! Number of input neurons */
                        int numInputNeurons;

                        /*! Number of output neurons */
                        int numOutputNeurons;

                        /*! Number of afferents */
                        int numAfferents;

                        /*! Number of patterns per afferent */
                        int numPatterns;

                        /*! Starting position of input layer on x axis */
                        int inputXStart;

                        /*! Starting position of input layer on y axis */
                        int inputYStart;

                        /*! Starting position of input layer on z axis */
                        int inputZStart;

                        /*! Number of afferents which contain a pattern */
                        float numCopyPasteAfferent;

                        /*! Time increment eg 1ms */
                        float dt;

                        /*! Maximum Firing rate (Hz) for poissonian process */
                        int maxFiringRate;

                        /*! Set firing rate (Hz) for spontaneous activity*/
                        int spontaneousActivity;

                        /*! Length of patterns eg 50ms */
                        float copyPasteDuration;

                        /*! Percentage of afferents that include the pattern */
                        float pct_aff_pat;

                        /*! Inject jitter into patterns */
                        float jitter;

                        /*! Max time spike Train can go without spike (Best to set <= copyPasteDuration) */
                        float maxTimeWithoutSpike;

                        /*! Proportion of each afferent containing the copy and pasted patterns (eg 1/3) */
                        float patternFreq;

                        /*! Length of spike train */
                        int timeT;

                        /*! Defines the points in the poissionian spike train which contain patterns */
                        QList<QList<int> > posCopyPaste;

                        /*! Just for experimental purposes in MATLAB hold the values of the spikes */
                        QList<QList<int> > posCopyPasteSpike;

                        /*! Track spikes related to the patterns */
                        QList<QList<unsigned> > posCopyPasteSpike2;

                        //======================  METHODS  ======================
                        QList<QList<int> > applyInputSequence(QList<Pattern>& patternList);
                        QList<QList<unsigned> > generateSpikeTrain();
                        QList<Pattern> getTemporalPatternSequence(QList<unsigned>& numberSequence);
                        void runExperiment1();
                        void stepNemo(unsigned numTimeSteps);
                        void storeParameters(QHash<QString, double>& parameterMap);
                        void initialiseVariables();
                        QList<unsigned> variablePoissonSpikeTrain(int minRate, int maxRate, int maxChangeSpeed, float time, float _dt, float maxTimeNoSpike);
                        float getRandomFloat();
                        QList<unsigned> copyDeletePaste(QList<unsigned>& spikeList, float jitter, int pat);
                        QList<unsigned> applyjitter(QList<unsigned>& spikeList, float jitter);
                        QList<Pattern> getTemporalPatternSequence(QList<QList<unsigned> >& spikeAffList);
                        QList<QList<unsigned> > readWriteSpikeTrain();
                        void outputResults(QList<QList<int> > firingTimes);
        };

}

#endif//MASQUELIERCODINGEPTMANAGER_H
