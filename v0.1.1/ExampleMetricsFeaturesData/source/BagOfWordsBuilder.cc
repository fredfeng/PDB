
#ifndef BAG_WORDS_BUILDER_C
#define BAG_WORDS_BUILDER_C

#include "BagOfWordsBuilder.h"
#include "SparseVectorFeatureType.h"

BagOfWordsBuilder :: BagOfWordsBuilder (string myNameIn, PDBQueryExecutorPtr myAggregatorIn, int dictionarySizeIn) {
	myName = myNameIn;
	myAggregator = myAggregatorIn;
	dictionarySize = dictionarySizeIn;
}

PDBFeatureBuilder &BagOfWordsBuilder :: operator = (PDBFeatureBuilderPtr &copyMeIn) {
	BagOfWordsBuilder *copyMe = (BagOfWordsBuilder *) copyMeIn.get ();	
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

PDBFeatureTypePtr BagOfWordsBuilder :: buildFeature (PDBData &fromMe) {

	// get a new aggregator
	PDBQueryExecutorPtr newAggregator = myAggregator->getEmpty ();

	// aggregate this one document
	newAggregator->aggregate (fromMe);

	// get the two aggrgators
	WordFrequencyAggregator *me = (WordFrequencyAggregator *) newAggregator.get ();	
	WordFrequencyAggregator *him = (WordFrequencyAggregator *) myAggregator.get ();	

	// and build up the vector
	PDBFeatureTypePtr retVal (new SparseVectorFeatureType);
	SparseVectorFeatureType *myVec = (SparseVectorFeatureType *) retVal.get ();

	for (int i = 0; i < me->getNumDistinctWords (); i++) {
		string temp = me->getKthMostFrequent (i);
		int pos = him->getRank (temp);
		if (pos < dictionarySize) {
			myVec->add (pos, me->getCount (temp));
		}	
	}
	myVec->normalize ();
	return retVal;
}

string BagOfWordsBuilder :: getFeatureName () {
	return myName;
}

// these do not do anything... they will only be called in a distributed database
// implementation in order to ship serialized objects around the cluster
void *BagOfWordsBuilder :: serialize(void *toHere, size_t &size) {
        return toHere;
}

void *BagOfWordsBuilder :: deSerialize(void *fromHere, size_t &size) {
        return fromHere;
}

#endif

