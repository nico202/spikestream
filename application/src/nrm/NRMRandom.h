#ifndef NRMRANDOM_H
#define NRMRANDOM_H

/*! Seed for the random number */
struct RandomSeed {
	unsigned int lo;
	unsigned int hi;
};

class NRMRandom {
	public:
		static void setSeed(unsigned int seed);

		static int getRandNum(int num);

	private:
		//====================  VARIABLES  =======================
		static RandomSeed seed;

		//=====================  METHODS  ========================
		static long generateRandNum(void);
		static void multiply64Bit(unsigned int& multiplicand, unsigned int multiplier, unsigned int& overflow);
};


#endif // NRMRANDOM_H
