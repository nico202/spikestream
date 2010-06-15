#ifndef NRMRANDOM_H
#define NRMRANDOM_H

namespace spikestream {

	/*! Seed for the random number */
	struct RandomSeed {
		unsigned int lo;
		unsigned int hi;
	};


	/*! Class used to generate random numbers in the same way as that used by NRM.
		This class is critical because NRM does not store connection patterns and relies
		on using the same random number generator with the same seed to re-generate the connection
		pattern when needed. Therefore, to import networks using NRM we have to have the same
		random number generator, which is not the standard C++ one. */
	class NRMRandom {
		public:
			static int getRandNum(int num);
			static void setSeed(unsigned int seed);

		private:
			//====================  VARIABLES  =======================
			static RandomSeed seed;

			//=====================  METHODS  ========================
			static long generateRandNum(void);
			static void multiply64Bit(unsigned int& multiplicand, unsigned int multiplier, unsigned int& overflow);
	};

}


#endif // NRMRANDOM_H
