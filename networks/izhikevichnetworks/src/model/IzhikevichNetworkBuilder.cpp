//SpikeStream includes
#include "Globals.h"
#include "NetworkInfo.h"
#include "NeuronGroupInfo.h"
#include "NeuronGroup.h"
#include "IzhikevichNetworkBuilder.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Other includes
#include <algorithm>
using namespace std;


/*! Constructor */
IzhikevichNetworkBuilder::IzhikevichNetworkBuilder(){
	//Initialize variables
	currentTask = 0;
	newNetwork = NULL;
}



/*! Destructor */
IzhikevichNetworkBuilder::~IzhikevichNetworkBuilder(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

//Inherited from QThread
void IzhikevichNetworkBuilder::run(){
	//Initialize thread variables
	clearError();
	stopThread = false;

	try{
		//Check network has been set
		if(newNetwork == NULL)
			throw SpikeStreamException("Network has not been set.");

		//Execute appropriate method
		switch (currentTask){
			case ADD_POLYCHRONIZATION_NETWORK_TASK:

			break;
			default:
				throw SpikeStreamException ("Task not defined or not recognized");
		}
	}
	catch(SpikeStreamException& ex)
		setError(ex.getMessage());
	catch(...)
		setError("Unknown exception thrown by IzhikevichNetworkBuilder::run()");

	//Finish off
	stopThread = true;
	currentTask = NO_TASK_DEFINED;
}


/*! Launches thread that adds polychronization network based on Izhikevich E.M. (2006). Polychronization: Computation With Spikes. Neural Computation,18:245-282
	Code based on Izhikevich's code: http://www.izhikevich.org/publications/spnet.htm */
void IzhikevichNetworkBuilder::startAddPolychronizationNetwork(const QString& networkName, const QString& networkDescription){
	currentTask = ADD_POLYCHRONIZATION_NETWORK_TASK;
	start();
}


/*----------------------------------------------------------*/
/*-----                PRIVATE METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Adds parity gate training to the neuron.
    A parity gate fires if it receives an odd number of spikes. */
void IzhikevichNetworkBuilder::addPolychronizationNetwork(){
	//Variables
	const	int		Ne = 800;		// excitatory neurons
	const	int		Ni = 200;		// inhibitory neurons
	const	int		N  = Ne+Ni;		// total number of neurons
	const	int		M  = 100;		// the number of synapses per neuron
	const	int		D  = 20;		// maximal axonal conduction delay
			float	sm = 10.0;		// maximal synaptic strength
	int		post[N][M];				// indeces of postsynaptic neurons
	float	s[N][M], sd[N][M];		// matrix of synaptic weights and their derivatives
	short	delays_length[N][D];	// distribution of delays
	short	delays[N][D][M];		// arrangement of delays
	int		N_pre[N], I_pre[N][3*M], D_pre[N][3*M];	// presynaptic information
	float	*s_pre[N][3*M], *sd_pre[N][3*M];		// presynaptic weights
	float	LTP[N][1001+D], LTD[N];	// STDP functions
	float	a[N], d[N];				// neuronal dynamics parameters
	float	v[N], u[N];				// activity variables
	int		N_firings;				// the number of fired neurons
	const int N_firings_max=100*N;	// upper limit on the number of fired neurons per sec
	int		firings[N_firings_max][2]; // indeces and timings of spikes

	//Initialize seed
	int randomSeed = 45;
	srand(randomSeed);

	//Create connections
	for (i=0;i<N;i++){//Work through neurons
		for (j=0;j<M;j++) {//Work through all connections per neuron
			do {
				exists = 0;		// avoid multiple synapses

				//Excitatory neurons connect to any neurons
				if (i<Ne)
					r = getrandom(N);

				//Inhibitory neurons only connect to excitatory neurons
				else
					r = getrandom(Ne);// inh -> exc only

				//Check for self connections
				if (r==i)
					exists=1;									// no self-synapses

				//Check to see if synapse already exists
				for (k=0;k<j;k++)
					if (post[i][k]==r)
						exists = 1;	// synapse already exists
			}
			while (exists == 1);
			post[i][j]=r;//Neuron number i, connecion number j connects to neuron number r
		}
	}

	//Create synaptic weights
	for (i=0;i<Ne;i++)
		for (j=0;j<M;j++)
			s[i][j]=6.0;  // initial exc. synaptic weights

	for (i=Ne;i<N;i++)
		for (j=0;j<M;j++)
			s[i][j]=-5.0; // inhibitory synaptic weights

	for (i=0;i<N;i++)
		for (j=0;j<M;j++)
			sd[i][j]=0.0; // synaptic derivatives

	//Create delays
	for (i=0;i<N;i++) {
		short ind=0;
		if (i<Ne){
			for (j=0;j<D;j++) {
				delays_length[i][j]=M/D;	// uniform distribution of exc. synaptic delays
				for (k=0;k<delays_length[i][j];k++)
					delays[i][j][k]=ind++;
			}
		}
		else {
			for (j=0;j<D;j++)
				delays_length[i][j]=0;
			delays_length[i][0]=M;			// all inhibitory delays are 1 ms
			for (k=0;k<delays_length[i][0];k++)
				delays[i][0][k]=ind++;
		}
	}
	//Get excitatory and inhibitory neuron types
	NetworkDao networkDao(Globals::getNetworkDao()->getDBInfo());
	NeuronType exNeurType = networkDao.getNeuronType("Izhikevich Excitatory Neuron");
	NeuronType inhibNeurType = networkDao.getNeuronType("Izhikevich Inhibitory Neuron");

	//Create excitatory and inhibitory neuron groups
	QHash<QString, double> parameterMap;
	NeuronGroup* exNeurGrp = new NeuronGroup(NeuronGroupInfo(0, "Excitatory neuron group", "Excitatory neuron group", parameterMap, exNeurType));
	NeuronGroup* inhibNeurGrp = new NeuronGroup(NeuronGroupInfo(0, "Inhibitory neuron group",  "Inhibitory neuron group", parameterMap, inhibNeurType));

	//Add the neurons to the groups
	float xPos, yPos, zPos;
	for(unsigned xCntr = 0; xCntr < numXNeur; ++xCntr){
		for(unsigned yCntr = 0; yCntr < numYNeur; ++yCntr){
			for(unsigned zCntr = 0; zCntr < numZNeur; ++zCntr){
				xPos = xStart + xCntr*xSpacing;
				yPos = yStart + yCntr*ySpacing;
				zPos = zStart + zCntr*zSpacing;
				if(ranNumGen() < proportionExcitatoryNeurons){
					exNeurGrp->addNeuron(xPos, yPos, zPos);
				}
				else{
					inhibNeurGrp->addNeuron(xPos, yPos, zPos);
				}
			}
		}
	}

}


