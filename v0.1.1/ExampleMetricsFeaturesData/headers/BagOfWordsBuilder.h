
#ifndef BAG_WORDS_BUILDER_H
#define BAG_WORDS_BUILDER_H

#include "WordFrequencyAggregator.h"
#include "PDBFeatureBuilder.h"
#include "PDBQueryExecutor.h"

class BagOfWordsBuilder : public PDBFeatureBuilder {

private:

	string myName;
	PDBQueryExecutorPtr myAggregator;
	int dictionarySize;

public:

	// constructor accepts the name of the feature that we are supposed
	// to be building, as well as aggregate info about the data set (this is a pointer
	// to a WordFrequencyAggregator object), and the size of the dictionary
	// that is to be constructed
	BagOfWordsBuilder (string myName, PDBQueryExecutorPtr aggs, int dictionarySize);

	// from the interface
	bool runsOnThisPDBData (PDBData &checkMe);
	PDBFeatureTypePtr buildFeature (PDBData &fromMe);
	string getFeatureName ();
	PDBFeatureBuilder &operator = (PDBFeatureBuilderPtr &copyMeIn);
	void *serialize(void *toHere, size_t &size);
	void *deSerialize(void *fromHere, size_t &size);
};

#endif

