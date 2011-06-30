//SpikeStream includes
#include "Globals.h"
#include "MasquelierCodingeEptManager.h"
#include "Util.h"
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <QtAlgorithms>

using namespace spikestream;

//Qt includes
#include <QDebug>
#include <QList>
#include <QDataStream>

/*! Constructor */
MasquelierCodingeEptManager::MasquelierCodingeEptManager() : SpikeStreamThread(){
}

/*! Destructor */
MasquelierCodingeEptManager::~MasquelierCodingeEptManager(){
}

/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void MasquelierCodingeEptManager::run(){

        clearError();
        stopThread = false;
        unsigned origWaitInterval = nemoWrapper->getWaitInterval_ms();
        nemoWrapper->setWaitInterval(0);//Minimal wait between steps

        try{
            //Start appropriate experiment
            emit statusUpdate("Starting Experiment " + QString::number(experimentNumber + 1));
            runExperiment1();
        }
        catch(...){
            setError("IzhikevichAccuracy has thrown an unknown exception.");
        }

        emit statusUpdate("Experiment complete.");
        nemoWrapper->setWaitInterval(origWaitInterval);//Restore wrapper to original state

        experimentNumber = NO_EXPERIMENT;
        stopThread = true;
}


/*! Sets up the experiment and starts thread running. */
void MasquelierCodingeEptManager::startExperiment(NemoWrapper* nemoWrapper, QHash<QString, double>& parameterMap){
    this->nemoWrapper = nemoWrapper;
    storeParameters(parameterMap);

    //Get neuron layers that we need
    QList<NeuronGroup*> neurGrpList = Globals::getNetwork()->getNeuronGroups();
    inputNeuronGroup = NULL;
    int caseNo = 0;
    foreach(NeuronGroup* tmpNeurGrp, neurGrpList){
        if(tmpNeurGrp->getInfo().getName().toUpper() == "INPUT LAYER"){
            inputNeuronGroup = tmpNeurGrp;
            if(tmpNeurGrp->size() != numAfferents)
                throw SpikeStreamException("Input neuron group is expected to have number of neurons equal to afferents.");
        }
        if(tmpNeurGrp->getInfo().getName().toUpper() == "OUTPUT NEURON"){
            if(tmpNeurGrp->size() != 1)
                throw SpikeStreamException("Output neuron group 1 is expected to have 1 neuron.");
            if(caseNo==0)
                outputNeuron1ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==1)
                outputNeuron2ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==2)
                outputNeuron3ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==3)
                outputNeuron4ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==4)
                outputNeuron5ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==5)
                outputNeuron6ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==6)
                outputNeuron7ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==7)
                outputNeuron8ID = tmpNeurGrp->getNeuronIDs().at(0);
            else if(caseNo==8)
                outputNeuron9ID = tmpNeurGrp->getNeuronIDs().at(0);
            caseNo++;
        }
    }
    if(inputNeuronGroup == NULL && caseNo != numInputNeurons)
        throw SpikeStreamException("Input neuron group and/or output neuron/s not found.");

    numInputNeurons = inputNeuronGroup->size();
    inputXStart = inputNeuronGroup->getBoundingBox().getX1();
    inputYStart = inputNeuronGroup->getBoundingBox().getY1();
    inputZStart = inputNeuronGroup->getBoundingBox().getZ1();

    start();
}


/*----------------------------------------------------------*/
/*-----                 PRIVATE METHODS                -----*/
/*----------------------------------------------------------*/

/*! Generates a Spike Train which first populates with spontaneous activity, then poissionian activity including the copied and pasted patterns */
QList<QList<unsigned> > MasquelierCodingeEptManager::generateSpikeTrain(){

    QList<unsigned> spikeList; //List to hold spike times
    QList<unsigned> afferentList; //List to record corresponding afferent to spike in spikeList

    spikeList.clear();
    afferentList.clear();

    int numSpontaneous = 0;
    int numNormal = 0;
    float inf = -logf(0.0);

    //Add some spontaneous spiking activity to each afferent to make pattern recognition more difficult, where firing rate = spontaneousActivity level
    if(spontaneousActivity>0){
        emit statusUpdate("    Adding Spontaneous Activity");
        for(int i=1; i<=numAfferents; i++){
            QList<unsigned> spontaneousActivityList = variablePoissonSpikeTrain(spontaneousActivity, spontaneousActivity, 0, timeT, dt, inf);
            numSpontaneous += spontaneousActivityList.length();
            spikeList.append(spontaneousActivityList);
            for(int j=1; j<=spontaneousActivityList.length(); j++)
                afferentList.append(i);
        }  
    }

    //Add poissonian spiking activity to each afferent, where 0 < firing rate < maxFiringRate
    emit statusUpdate("    Emitting poisson spikes");
    for(int i=1; i<=numAfferents; i++){
        QList<unsigned> sl = variablePoissonSpikeTrain(0, maxFiringRate, maxFiringRate/copyPasteDuration, timeT, dt, inf); //Testing with no maxTimeWithoutSpike
        numNormal += sl.length();

        //For each afferent call copyDeletePaste function which copies and pastes patterns
        for (int pat=1; pat<=numPatterns; pat++){
            if((numPatterns==1 && i<=numCopyPasteAfferent) || (numPatterns>1 && getRandomFloat()< numCopyPasteAfferent/numAfferents )) //Ensure only copying to half the afferents
                sl = copyDeletePaste(sl,jitter,pat);
        }        

        //Add the afferents spiking activity including patterns to spikeList and record the corresponding afferents
        spikeList.append(sl);
        for(int m=0; m<sl.length(); m++)
            afferentList.append(i);
    }

    //Combine spikeList and afferentList into a list of lists.
    QList<QList<unsigned> > spikeAfferentList;
    QList<unsigned> tmpList;
    spikeAfferentList.clear();
    tmpList.clear();
    for(int i=0;i<afferentList.length();i++)
        spikeAfferentList.append(tmpList);

    for(int i=0;i<afferentList.length();i++){
        spikeAfferentList[i].append(spikeList.at(i));
        spikeAfferentList[i].append(afferentList.at(i));
    }

    for(int pat=0;pat<numPatterns;pat++)
        qSort(posCopyPasteSpike2[pat]);

    return spikeAfferentList;

}

/*! Generates a poissonian spike train based on the minimum and maximum firing rate */
QList<unsigned> MasquelierCodingeEptManager::variablePoissonSpikeTrain(int minRate, int maxRate, int maxChangeSpeed, float time, float _dt, float maxTimeNoSpike ){

    QList<unsigned> st;
    float virtualPreSimuSpike = -getRandomFloat()*maxTimeNoSpike;
    float rate = minRate + getRandomFloat()*(maxRate-minRate); //spiking rate
    float rateChange = 2*(getRandomFloat()-.5)*maxChangeSpeed; //rate of change of spiking rate
    float mtws = maxTimeNoSpike; //Max Time with no spike (Can be used to ensure mtws<length of pattern so every afferent has at least one spike in a pattern

    for(float t=_dt; t<time; ){
        float temp1;
        float temp2;
        if (st.empty()==false){
            temp1 = st.back();
            temp2 = temp1/1000;
        }
        if ((getRandomFloat() < _dt*rate) || (st.empty()==false && (t - temp2)>mtws) || (st.empty()==true && t-virtualPreSimuSpike>mtws)){
            float tmp = t-_dt*getRandomFloat();
            tmp = fmax(0, tmp);
            tmp = fmin(time, tmp);
            int tmp_ms = tmp*1000;
            st.append(tmp_ms);
            mtws = maxTimeNoSpike;
        }
        rate = rate + rateChange*dt;
        rateChange = rateChange + 0.2 * 2 * (getRandomFloat() -.5) * maxChangeSpeed;
        rateChange = fmax(fmin(rateChange, maxChangeSpeed), - maxChangeSpeed);
        rate = fmax(fmin(rate,maxRate),minRate);
        t +=_dt;
    }
    return st;
}

/*! Reads in an afferent and a pattern number and copies in the pattern according to posCopyPaste */
QList<unsigned> MasquelierCodingeEptManager::copyDeletePaste(QList<unsigned>& spikeList, float jitter, int pat){
    QList<unsigned> toCopy; //Stores patterns ie spikes to copy
    QList<unsigned> toRemove; //Stores all the spikes currently in the way of the patterns ie spikes to remove

    int s=0;

    for(int p=0;p<posCopyPaste[pat-1].length() ;p++){
        unsigned from = ((posCopyPaste[pat-1].at(p)-1)*1000*copyPasteDuration);
        unsigned to = from + (copyPasteDuration*1000);
        while (s<spikeList.size() && spikeList.at(s)<from)
            s++;
        while (s<spikeList.size() && spikeList.at(s)<=to){
            if(p==5)
                toCopy.append(spikeList.at(s) - ((posCopyPaste[pat-1].at(p)-1)*1000*copyPasteDuration)); // Record the spikes that need copying from each afferent
            toRemove.append(s);
            s=s+1;
        }
    }

    //Remove any spikes which are to be copied over by the pattern
    for(int i=0;i<toRemove.length();i++)
        spikeList.removeAt(toRemove.at(i)-i);

    //Append the copied pattern spikes onto the end which include jitter
    for(int p=0;p<posCopyPaste[pat-1].length();p++){
        QList<unsigned> spikejitter = applyjitter(toCopy,jitter);
        int tmp = ((posCopyPaste[pat-1].at(p)-1)*1000*copyPasteDuration);
        for(int j=0;j<spikejitter.length();j++)
            spikejitter[j] += tmp;
        spikeList.append(spikejitter);
        posCopyPasteSpike2[pat-1].append(spikejitter);
    }

    return spikeList;
}

/*! Applies jitter to each pattern */
QList<unsigned> MasquelierCodingeEptManager::applyjitter(QList<unsigned>& spikeList,float jitter){
    for(int i=0;i<spikeList.length();i++){
        int jitter_var = spikeList.at(i)+jitter*1000*getRandomFloat();
        spikeList.replace(i,jitter_var);
    }
    return spikeList;
}

/*! Returns a random float between 0 and 1 */
float MasquelierCodingeEptManager::getRandomFloat(){
    return (float) rand()/RAND_MAX;
}

/*! Returns a sequence of patterns encoding a set of numbers */
QList<Pattern> MasquelierCodingeEptManager::getTemporalPatternSequence(QList<QList<unsigned> >& spikeAffList){
    //Create empty patterns. Need 1 pattern for each time step

    unsigned numTimeSteps = timeT*1000; //Timestep per millisecond
    QList<Pattern> tmpPatternList;
    for(unsigned i=0; i<numTimeSteps; i++){
        //Add a couple of default points to ensure alignment
        Pattern tmpPattern;
        tmpPattern.addPoint(Point3D(inputXStart - 1.0f, inputYStart - 1.0f, inputZStart));
        tmpPattern.addPoint(Point3D(inputXStart + numInputNeurons, inputYStart + 1, inputZStart));
        tmpPatternList.append(tmpPattern);
    }

    //For each time step, check which afferents fire and add pattern
    for(int j=0;j<spikeAffList.length();j++){
        if(spikeAffList[j].at(0)<numTimeSteps)
            tmpPatternList[spikeAffList[j].at(0)].addPoint(Point3D(inputXStart + spikeAffList[j].at(1) -1, inputYStart, inputZStart));
    }

    return tmpPatternList;
}

/*! Applys a sequence of firing patterns to the network */
QList<QList<int> > MasquelierCodingeEptManager::applyInputSequence(QList<Pattern>& patternList){
    QList<QList<int> > fireTimes; // records the times the output neurons fire
    QList<int> tmpList;
    tmpList.clear();
    fireTimes.clear();

    for(int i=0;i< numOutputNeurons;i++)
        fireTimes.append(tmpList);

    nemoWrapper->setMonitor(false);
    nemoWrapper->setUpdateFiringNeurons(true);
    for(int i=0; i<patternList.size() && !stopThread; i++){

        nemoWrapper->setFiringInjectionPattern(patternList.at(i), inputNeuronGroup->getID(), false);
        stepNemo(1);
        QList<neurid_t> tmpList = nemoWrapper->getFiringNeuronIDs();
        foreach(neurid_t tmpNeurID, tmpList){
            if(tmpNeurID == outputNeuron1ID)
                fireTimes[0].append(i);
            if(tmpNeurID == outputNeuron2ID)
                fireTimes[1].append(i);
            if(tmpNeurID == outputNeuron3ID)
                fireTimes[2].append(i);
            if(tmpNeurID == outputNeuron4ID)
                fireTimes[3].append(i);
            if(tmpNeurID == outputNeuron5ID)
                fireTimes[4].append(i);
            if(tmpNeurID == outputNeuron6ID)
                fireTimes[5].append(i);
            if(tmpNeurID == outputNeuron7ID)
                fireTimes[6].append(i);
            if(tmpNeurID == outputNeuron8ID)
                fireTimes[7].append(i);
            if(tmpNeurID == outputNeuron9ID)
                fireTimes[8].append(i);
        }
    }
    return fireTimes;
}

/*! Runs the experiment. */
void MasquelierCodingeEptManager::runExperiment1(){

        emit statusUpdate("Random Seed: " + QString::number(randomSeed));

        QTime timer;

        timer.start();

        for(int i=1;i<1000;i++)
            qDebug()<<timer.elapsed();

        srand(randomSeed);
        initialiseVariables();
        emit statusUpdate("Generating spike train");
        QList<QList<unsigned> > spikeAffList = readWriteSpikeTrain();
        emit statusUpdate("Spike train Complete");
        qDebug()<<"Average Firing Rate = " << spikeAffList.length()/(numAfferents*timeT);
        QList<Pattern> patternList = getTemporalPatternSequence(spikeAffList);
        emit statusUpdate("Injecting Spike Train");
        QList<QList<int> > firingTimes = applyInputSequence(patternList);
        outputResults(firingTimes);
}

/*! Generate results from the experiment (For final 20 seconds). */
void MasquelierCodingeEptManager::outputResults(QList<QList<int> > firingTimes){
    emit statusUpdate("Results of the Experiment");

    //**DEBUGGING
    unsigned firstPat = (posCopyPaste[0].at(0)-1)*1000*copyPasteDuration;
    unsigned counter=0;
    QList<unsigned> pattyList;
    emit statusUpdate("PATTERN......");
    while(posCopyPasteSpike2[0].at(counter)<firstPat+50){
        pattyList.append(posCopyPasteSpike2[0].at(counter));
        emit statusUpdate(QString::number(posCopyPasteSpike2[0].at(counter)));
        counter++;
    }
    qDebug()<<"Pattern is: "<<pattyList;

    for(int neur=0;neur<numOutputNeurons;neur++){
        int spikes_outside_pat=0;
        int pattern_counter =0;
        int pattern_hit = 0;
        bool pattern_success = false;
        int spikes_inside_pat=0;
        int total_spikes=0;
        int modulus_level = copyPasteDuration*1000;
        double latency_pat=0;
        int mult =20000;
        emit statusUpdate("------------------------------Neuron number " + QString::number(neur+1) + "------------------------------");
        for(int pat=0;pat<numPatterns;pat++){
            emit statusUpdate("------------------------------Pattern number " + QString::number(pat+1)+ "------------------------------");
            for(int p=0;p<posCopyPaste[pat].length() && ((posCopyPaste[pat].at(p)-1)*1000*copyPasteDuration)<mult ;p++){
                int from = ((posCopyPaste[pat].at(p)-1)*1000*copyPasteDuration);
                pattern_counter++;
                int to = from + (copyPasteDuration*1000);

                //DEBUGGING!
                //if(mult==1580000)
                //    qDebug()<<"from= "<<from;


                while (total_spikes<firingTimes[neur].length() && firingTimes[neur].at(total_spikes)<from){

                    //DEBUGGING!
                    //if(mult==1580000)
                    //    qDebug()<<"firing outside = "<< firingTimes[neur].at(total_spikes);

                    spikes_outside_pat++;
                    total_spikes++;

                }
                while (total_spikes<firingTimes[neur].length() && firingTimes[neur].at(total_spikes)<=to){

                    //DEBUGGING!
                    //if(mult==1580000)
                    //    qDebug()<<"firing inside = "<< firingTimes[neur].at(total_spikes);


                    latency_pat += (firingTimes[neur].at(total_spikes)-from);
                    spikes_inside_pat++;
                    total_spikes++;
                    pattern_success =true;
                }
                if(pattern_success){
                    pattern_hit++;
                    pattern_success=false;
                }
                if(p+1<posCopyPaste[pat].length()){
                    if((posCopyPaste[pat].at(p+1)-1)*1000*copyPasteDuration>=mult){
                        while(total_spikes<firingTimes[neur].length() && firingTimes[neur].at(total_spikes)<mult){
                            spikes_outside_pat++;
                            total_spikes++;
                        }
                        //emit statusUpdate("***" + QString::number((mult-20000)/1000) + "-" + QString::number(mult/1000) + " seconds results***");
                        //emit statusUpdate("       Spikes outside pattern: " + QString::number(spikes_outside_pat));
                        //emit statusUpdate("       Spikes inside pattern: " + QString::number(spikes_inside_pat));
                        //emit statusUpdate("       Total spikes: " + QString::number(spikes_outside_pat+spikes_inside_pat));
                        //emit statusUpdate("       Average Latency: " + QString::number(latency/spikes_inside_pat));
                        mult += 20000;
                        spikes_outside_pat=0;
                        spikes_inside_pat=0;
                        latency_pat = 0;
                        pattern_hit = 0;
                        pattern_counter = 0;
                    }
                }
                if(p+1>=posCopyPaste[pat].length()){
                    while(total_spikes<firingTimes[neur].length() && firingTimes[neur].at(total_spikes)<mult){
                        spikes_outside_pat++;
                        total_spikes++;
                    }
                    emit statusUpdate("***" + QString::number((mult-20000)/1000) + "-" + QString::number(mult/1000) + " seconds results(LAST 20 SECONDS)***");
                    emit statusUpdate("       Spikes outside pattern: " + QString::number(spikes_outside_pat));
                    emit statusUpdate("       Spikes inside pattern: " + QString::number(spikes_inside_pat));
                    emit statusUpdate("       ****SPIKE ACCURACY: " + QString::number((double) 100* spikes_inside_pat/(spikes_outside_pat+spikes_inside_pat)) + "****");
                    emit statusUpdate("       Number of Patterns: " + QString::number(pattern_counter));
                    emit statusUpdate("       Patterns hit: " + QString::number(pattern_hit));
                    emit statusUpdate("       ****PATTERN ACCURACY: " + QString::number((double) 100* pattern_hit/pattern_counter) + "****");
                    emit statusUpdate("       Average Latency from start of pattern: " + QString::number(latency_pat/spikes_inside_pat));
                    emit statusUpdate("       Average Latency from first spike in pattern: " + QString::number((latency_pat/spikes_inside_pat)-posCopyPasteSpike2[pat].at(0)%modulus_level));
                    mult = 20000;
                    spikes_outside_pat=0;
                    spikes_inside_pat=0;
                    latency_pat = 0;
                    total_spikes = 0;
                    pattern_hit = 0;
                    pattern_counter = 0;
                }
            }
        }
    }
}

/*! Initialise variables for the experiment. */
void MasquelierCodingeEptManager::initialiseVariables(){

    //Variables for Spike Train
    numCopyPasteAfferent = round(pct_aff_pat*numAfferents); //Number of afferent to include patterns
    dt = 0.001; //timestep
    maxTimeWithoutSpike = copyPasteDuration;

    QList<int> tmpPattern;
    QList<unsigned> tmpPattern2;
    tmpPattern.clear();
    tmpPattern2.clear();
    posCopyPaste.clear(); //Keeps track of where the patterns are in the spike trains
    posCopyPasteSpike.clear(); // Keeps track of the spike times of ONLY the patterns as well as corresponding afferents
    posCopyPasteSpike2.clear(); // A sorted list of the spike times of ONLY the patterns
    for(int i=1;i<=numPatterns;i++){
        posCopyPaste.append(tmpPattern);
        posCopyPasteSpike2.append(tmpPattern2);
    }
    if(patternFreq>0){
        bool skip = false;
        for(int p = 1; p< timeT/copyPasteDuration; p++){
            if (skip || p==6078)
                skip = false;
            else
                if (getRandomFloat() < 1/(1/patternFreq -1)){
                    int idx = ceil(getRandomFloat() * numPatterns);
                    posCopyPaste[idx-1].append(p);
                    skip = true;
                }
        }
    }
}

/*! Function used to be used to read spikeAffList into a txt file for testing in MATLAB */
QList<QList<unsigned> > MasquelierCodingeEptManager::readWriteSpikeTrain(){

    QList<QList<unsigned> > spikeAffList;
    spikeAffList = generateSpikeTrain();
    return spikeAffList;

}

/*! Advances the simulation by the specified number of time steps */
void MasquelierCodingeEptManager::stepNemo(unsigned numTimeSteps){
    for(unsigned i=0; i<numTimeSteps && !stopThread; ++i){
        nemoWrapper->stepSimulation();
        while((nemoWrapper->isWaitForGraphics() || nemoWrapper->getCurrentTask() == NemoWrapper::STEP_SIMULATION_TASK) && !stopThread)
            msleep(pauseInterval_ms);
    }
    msleep(pauseInterval_ms);
}


/*! Stores the parameters for the experiment */
void MasquelierCodingeEptManager::storeParameters(QHash<QString, double> &parameterMap){
    if(!parameterMap.contains("experiment_number"))
        throw SpikeStreamException("MasquelierCodingeEptManager: experiment_number parameter missing");
    experimentNumber = (int)parameterMap["experiment_number"];

    if(!parameterMap.contains("no_afferents"))
        throw SpikeStreamException("MasquelierCodingeEptManager: no_afferents parameter missing");
    numAfferents = (int)parameterMap["no_afferents"];

    if(!parameterMap.contains("no_output_neurons_(MAXIMUM 9)"))
        throw SpikeStreamException("MasquelierCodingeEptManager: no_output_neurons_(MAXIMUM 9) parameter missing");
    numOutputNeurons = (int)parameterMap["no_output_neurons_(MAXIMUM 9)"];

    if(!parameterMap.contains("no_patterns"))
        throw SpikeStreamException("MasquelierCodingeEptManager: no_patterns parameter missing");
    numPatterns = (int)parameterMap["no_patterns"];

    if(!parameterMap.contains("percentage_afferents_including_pattern"))
        throw SpikeStreamException("MasquelierCodingeEptManager: percentage_afferents_including_pattern parameter missing");
    pct_aff_pat = (float)parameterMap["percentage_afferents_including_pattern"];

    if(!parameterMap.contains("spontaneous_activity_hz"))
        throw SpikeStreamException("MasquelierCodingeEptManager: spontaneous_activity_hz parameter missing");
    spontaneousActivity = (int)parameterMap["spontaneous_activity_hz"];

    if(!parameterMap.contains("pattern_duration_ms"))
        throw SpikeStreamException("MasquelierCodingeEptManager: pattern_duration_ms parameter missing");
    copyPasteDuration = (float)parameterMap["pattern_duration_ms"];

    if(!parameterMap.contains("max_firing_rate"))
        throw SpikeStreamException("MasquelierCodingeEptManager: maximum firing rate of poissionian spike train");
    maxFiringRate = (int)parameterMap["max_firing_rate"];

    if(!parameterMap.contains("time_seconds"))
        throw SpikeStreamException("MasquelierCodingeEptManager: time of experiment");
    timeT = (int)parameterMap["time_seconds"];

    if(!parameterMap.contains("jitter"))
        throw SpikeStreamException("MasquelierCodingeEptManager: jitter parameter missing");
    jitter = (float)parameterMap["jitter"];

    if(!parameterMap.contains("pattern_frequency"))
        throw SpikeStreamException("MasquelierCodingeEptManager: pattern_frequency parameter missing");
    patternFreq = (float)parameterMap["pattern_frequency"];

    if(!parameterMap.contains("random_seed"))
        throw SpikeStreamException("MasquelierCodingeEptManager: random_seed parameter missing");
    randomSeed = (int)parameterMap["random_seed"];

    if(!parameterMap.contains("pause_interval_ms"))
        throw SpikeStreamException("MasquelierCodingeEptManager: pause_interval_ms parameter missing");
    pauseInterval_ms = (int)parameterMap["pause_interval_ms"];

}
