
#ifndef BAG_WORDS_BUILDER_C
#define BAG_WORDS_BUILDER_C

#include "BagOfWordsBuilder.h"
#include <iostream>
#include "SparseVectorFeatureType.h"
#include <string.h>

using namespace std;

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

PDBFeatureBuilderPtr BagOfWordsBuilder :: getEmptyBuilder() {
	return make_shared <BagOfWordsBuilder> ();
}

bool BagOfWordsBuilder :: runsOnThisPDBData (PDBData &checkMe) {
	if (checkMe.getData ()->getCode () == 494853243)
		return true;
	else
		return false;
}

int BagOfWordsBuilder :: getCode () {
	return 493249443;
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
		string temp = me->getKthMostFrequent (i);
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
void *BagOfWordsBuilder :: serialize(void *toHereIn, size_t &size) {

	// write the dictionary size
	char *toHere = (char *) toHereIn;
	*((int *) toHere) = dictionarySize;
	toHere += sizeof (int);

	// write the name of the feature
	memmove (toHere, myName.c_str (), myName.size () + 1);
	toHere += myName.size () + 1;

	// write the aggregate query result
	toHere = (char *) myAggregator->serialize (toHere, size);

	// prepare the results
	size = toHere - (char *) toHereIn;
        return toHere;
}

void *BagOfWordsBuilder :: deSerialize(void *fromHereIn, size_t &size) {

	// read the dictionary size
	char *fromHere = (char *) fromHereIn;
	dictionarySize = *((int *) fromHere);
	fromHere += sizeof (int);

	// read the name of the feature
	string tempString (fromHere);
	myName = tempString;
	fromHere += tempString.size () + 1;

	// read the query result
	myAggregator = make_shared <WordFrequencyAggregator> ();
	fromHere = (char *) myAggregator->deSerialize (fromHere, size);

	// and prepare the results
	size = fromHere - (char *) fromHereIn;
        return fromHere;
}

#endif

