#ifndef TESTNEMOLIBRARY_H
#define TESTNEMOLIBRARY_H

//Qt includes
#include <QtTest>
#include <QString>

#include <nemo.h>

class TestNemoLibrary : public QObject {
	Q_OBJECT

	private slots:
		/*void testNemoDLL1();
		void testNemoDLL2();
		void testNemoConfiguration(); */
		void testNemoSTDP();

	private:
		//===================  VARIABLES  ================
		static synapse_id synapseIDCntr;


		//===================  METHODS  ===================
		void checkNemoOutput(nemo_status_t result, const QString& errorMessage);
		/*static synapse_id getSynapseID();

		//Nemo simulation methods
		static void addExcitatoryNeuron(nemo_network_t c_net, unsigned nidx);
		static void addExcitatorySynapses(nemo_network_t c_net, unsigned source, unsigned ncount, unsigned scount);
		static void addInhibitorySynapses(nemo_network_t c_net, unsigned source, unsigned ncount, unsigned scount);
		static void addInhibitoryNeuron(nemo_network_t c_net, unsigned nidx);
		void c_runSimulation(const nemo_network_t net, const nemo_configuration_t conf, unsigned seconds,	std::vector<unsigned>* fcycles,	std::vector<unsigned>* fnidx);*/


};

#endif//TESTNEMOLIBRARY_H
