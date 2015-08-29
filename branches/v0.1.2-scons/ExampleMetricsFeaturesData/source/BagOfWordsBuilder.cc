
#ifndef BAG_WORDS_BUILDER_C
#define BAG_WORDS_BUILDER_C

#include "BagOfWordsBuilder.h"
#include "SparseVectorFeatureType.h"

BagOfWordsBuilder :: BagOfWordsBuilder (const string &myNameIn, PDBQueryExecutorPtr myAggregatorIn, int dictionarySizeIn) {
	myName = myNameIn;
	myAggregator = myAggregatorIn;

	// this is necessary so that the reads to the aggregator are thread safe... if we don't do this, then it
	// is possible that a query to the aggregator will trigger a sort... if this happens, we can be in trouble
	// if multiple threads sort at the same time.  So here, we do the sort only one time
	WordFrequencyAggregator *me = dynamic_cast<WordFrequencyAggregator *> (myAggregator.get ());
	me->getKthMostFrequent (0);

	dictionarySize = dictionarySizeIn;
}

PDBFeatureBuilder &BagOfWordsBuilder :: operator = (PDBFeatureBuilderPtr &copyMeIn) {
	BagOfWordsBuilder *copyMe = dynamic_cast<BagOfWordsBuilder *> (copyMeIn.get ());
	myName = copyMe->myName;
	myAggregator = copyMe->myAggregator;
	dictionarySize = copyMe->dictionarySize;
	return *this;
}

bool BagOfWordsBuilder :: runsOnThisPDBData (PDBData &checkMe) {
	if (checkMe.getData ()->getCode () == 494853243)
		return true;
	else
		return false;
}

PDBFeatureTypePtr BagOfWordsBuilder :: buildFeature (PDBData &fromMe, bool &wasError, string &) {

	wasError = false;

	// get a new aggregator
	PDBQueryExecutorPtr newAggregator = myAggregator->getEmpty ();

	// aggregate this one document
	newAggregator->aggregate (fromMe);

	// get the two aggrgators
	WordFrequencyAggregator *me = dynamic_cast<WordFrequencyAggregator *> (newAggregator.get ());
	WordFrequencyAggregator *him = dynamic_cast<WordFrequencyAggregator *> (myAggregator.get ());

	// and build up the vector
	PDBFeatureTypePtr retVal {make_shared<SparseVectorFeatureType> ()};
	SparseVectorFeatureType *myVec = dynamic_cast<SparseVectorFeatureType *> (retVal.get ());

	for (int i = 0; i < me->getNumDistinctWords (); i++) {
		const string &temp = me->getKthMostFrequent (i);
		int pos = him->getRank (temp);
		if (pos < dictionarySize) {
			myVec->add (pos, me->getCount (temp));
		}	
	}
	myVec->normalize ();
	return retVal;
}

const string &BagOfWordsBuilder :: getFeatureName () {
	return myName;
}

// these do not do anything... they will only be called in a distributed database
// implementation in order to ship serialized objects around the cluster
void *BagOfWordsBuilder :: serialize(void *toHere, size_t &) {
        return toHere;
}

void *BagOfWordsBuilder :: deSerialize(void *fromHere, size_t &) {
        return fromHere;
}

#endif

