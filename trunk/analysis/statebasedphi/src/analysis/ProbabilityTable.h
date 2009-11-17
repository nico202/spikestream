#ifndef PROBABILITYTABLE_H
#define PROBABILITYTABLE_H

namespace spikestream {

    /*! Holds a list of entries for different states, e.g. 11010, 11000, etc. */
    class ProbabilityTable{
	public:
	    ProbabilityTable(int size);
	    ~ProbabilityTable();
	    double getProbability(const QString& key);
	    QHash<QString, double>* getProbabilityValueMap();
	    int numCols() { return numEntries; }
	    void setProbability(const QString& key, double value);

	private:
	    //======================  VARIABLES  ======================
	    /*! Size of the table */
	    int numEntries;

	    /*! Map linking QString key, which is a series of 1's and 0's with the probability value */
	    QHash<QString, double> probValueMap;

	    //======================  METHODS  ========================
	    void buildProbabilityTable();
    };

}

#endif//PROBABILITYTABLE_H

