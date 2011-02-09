#ifndef UTIL_H
#define UTIL_H

//SpikeStream includes
#include "Point3D.h"
#include "WeightlessNeuron.h"

//Qt includes
#include <QString>
#include <QList>

//Other includes
#include <stdint.h>
#include <gmpxx.h>


namespace spikestream {

	typedef unsigned char byte;

	/*! Set of static utility methods. */
	class Util {
		public:
			static void addTraining(WeightlessNeuron& neuron, QString trainingPattern, int output);
			bool bitsEqual(unsigned char* byteArr, QString bitPattStr, int output);
			static void fillByteArray(unsigned char*& byteArr, int& arrLen, QString byteStr);
			static void fillSelectionArray(bool* array, int arraySize, int selectionSize);
			static bool getBool(const QString& str);
			static bool getBoolParameter(const QString& paramName, QHash<QString, double>& paramMap);
			static float getFloat(const QString& str);
			static int getInt(const QString& str);
			static unsigned int getUInt(const QString& str);
			static QList<unsigned int> getUIntList(const QString& str);
			static double getDouble(const QString& str);
			static double getDoubleParameter(const QString& paramName, QHash<QString, double>& paramMap);
			static double getPositiveDoubleParameter(const QString& paramName, QHash<QString, double>& paramMap);
			static float getFloatParameter(const QString& paramName, QHash<QString, double>& paramMap);
			static int getIntParameter(const QString& paramName, QHash<QString, double>& paramMap);
			static unsigned getUIntParameter(const QString& paramName, QHash<QString, double>& paramMap);
			static int getRandom(int min, int max);
			static double getRandomDouble(double min, double max);
			static float getRandomFloat(float min, float max);
			static unsigned getRandomUInt(unsigned min, unsigned max);
			static QString getRootDirectory();
			static bool isNumber(const QString& str);
			static mpf_class factorial(unsigned int num);
			static float min(float n1, float n2, float n3);
			static void printBinary(uint64_t number);
			static void printByteArray(byte* byteArr, int arrLen);
			static void printByteArray(const QByteArray& byteArr);
			static void printBoolArray(bool arr[], int arrLen);
			static void printParameterMap(const QHash<QString, double>& paramMap);
			static double rDouble(double num, int numPlaces);
			static int rInt(double num);
			static unsigned int rUInt(double num);
			static void safeCStringCopy(char target[], const char source[], int targetSize);
			static void seedRandom(int seed = 0);
			static void setGeneralization(QHash<unsigned int, WeightlessNeuron*>& weiNeurMap, double gen);
			static double toPositive(double num);
			static float toPositive(float num);
			static int toPositive(int num);

	};

}

#endif//UTIL_H

