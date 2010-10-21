//SpikeStream includes
#include "PatternManager.h"
using namespace spikestream;

PatternManager::PatternManager(){
}

PatternManager::~PatternManager(){
}


/*! Loads a pattern from the specified file and returns it.
	It is the responsibility of the invoking class to delete the pattern
	when it is no longer required. */
Pattern* PatternManager::load(const QString &filePath){
	return new Pattern();
}
