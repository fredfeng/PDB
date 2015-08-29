
#ifndef FLAT_FEATURE_C
#define FLAT_FEATURE_C

#include "FlatFeature.h"
#include <stdlib.h>
#include <string.h>

FlatFeature :: FlatFeature () {
	myBytes = nullptr;
	mySize = 0;
	myName = "FlatFeature";
}

FlatFeature :: ~FlatFeature () {
	free (myBytes);	
}

const string &FlatFeature :: getTypeName () {
	return myName;
}

int FlatFeature :: getCode () {
	return 5929354;
}

PDBFeatureTypePtr FlatFeature :: getNewFeatureTypeObject () {
	return make_shared <FlatFeature> ();
}

string FlatFeature :: display () {
	return "flat feature";
}

void *FlatFeature :: serialize (void *toHere, size_t &numBytes) {
	memmove (toHere, myBytes, mySize);
	numBytes = mySize;
	return ((char *) toHere) + numBytes;
}

void *FlatFeature :: deSerialize (void *fromHere, size_t &numBytes) {
	if (myBytes != nullptr) {
		free (myBytes);
	}
	myBytes = (char *) malloc (numBytes);
	mySize = numBytes;
	memmove (myBytes, fromHere, mySize);
	return ((char *) fromHere) + numBytes;
}

#endif

