#ifndef PROBABILITYTABLE_H
#define PROBABILITYTABLE_H

namespace spikestream {

    /*! Holds a list of entries for different states, e.g. 11010, 11000, etc. */
    class ProbabilityTable{
	public:
	    ProbabilityTable(int size);
	    ~ProbabilityTable();
	    double get(const QString& key);
	    QHash<QString, double>::iterator begin() { return probValueMap.begin(); }
	    QHash<QString, double>::iterator end() { return probValueMap.end(); }
	    int getNumberOfElements() { return numElements; }
	    int getNumberOfRows() { return probValueMap.size(); }
	    void set(const QString& key, double value);

	private:
	    //======================  VARIABLES  ======================
	    /*! Size of the table */
	    int numElements;

	    /*! Map linking QString key, which is a series of 1's and 0's with the probability value */
	    QHash<QString, double> probValueMap;

	    //======================  METHODS  ========================
	    void buildProbabilityTable();
    };

}

#endif//PROBABILITYTABLE_H

