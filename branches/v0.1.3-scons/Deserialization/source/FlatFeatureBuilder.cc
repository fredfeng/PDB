
#ifndef FLAT_FEATURE_BUILDER_C
#define FLAT_FEATURE_BUILDER_C

#include "FlatFeatureBuilder.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

FlatFeatureBuilder :: FlatFeatureBuilder () {
	myBytes = nullptr;
	mySize = 0;
	myCode = -1;
	myName = "FlatFeatureBuilder";
}

FlatFeatureBuilder :: ~FlatFeatureBuilder () {
	free (myBytes);	
}

int FlatFeatureBuilder :: getCode () {

	// if we have some data, return *its* code, not our own
	if (myCode != -1)
		return myCode;

	return 5977254;
}

const string &FlatFeatureBuilder :: getFeatureName() {
	return myName;
}

bool FlatFeatureBuilder :: runsOnThisPDBData (PDBData &) {
	return false;
}

PDBFeatureBuilderPtr FlatFeatureBuilder :: getEmptyBuilder () {
	return make_shared <FlatFeatureBuilder> ();
}

PDBFeatureTypePtr FlatFeatureBuilder :: buildFeature (PDBData &, bool &, string &) {
	return nullptr;
}

void *FlatFeatureBuilder :: serialize (void *toHere, size_t &numBytes) {
        memmove (toHere, myBytes, mySize);
        numBytes = mySize;
        return ((char *) toHere) + numBytes;
}

void *FlatFeatureBuilder :: deSerialize (void *fromHere, size_t &numBytes) {
	if (myBytes != nullptr) {
		free (myBytes);
	}

	// get the type code
	myCode = *((int *) fromHere);
	char *fromHereIn = sizeof (int) + (char *) fromHere; 

	// and now get the bytes
	myBytes = (char *) malloc (numBytes);
	mySize = numBytes;
	memmove (myBytes, fromHereIn, mySize);

	// note that we deserialized the type code as well
	numBytes += sizeof (int);

	return ((char *) fromHere) + numBytes;
}

#endif

