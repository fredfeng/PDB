
#ifndef FLAT_DATA_C
#define FLAT_DATA_C

#include "FlatStoredData.h"
#include <stdlib.h>
#include <string.h>

FlatStoredData :: FlatStoredData () {
	myBytes = nullptr;
	mySize = 0;
	myName = "FlatStoredData";
}

FlatStoredData :: ~FlatStoredData () {
	free (myBytes);	
}

const string &FlatStoredData :: getName () {
	return myName;
}

int FlatStoredData :: getCode () {
	return 5924354;
}

PDBStoredDataTypePtr FlatStoredData :: getNewStoredDataTypeObject () {
	return make_shared <FlatStoredData> ();
}

string FlatStoredData :: display () {
	return "flat stored data";
}

void *FlatStoredData :: serialize (void *toHere, size_t &numBytes) {
	memmove (toHere, myBytes, mySize);
	numBytes = mySize;
	return ((char *) toHere) + numBytes;
}

void *FlatStoredData :: deSerialize (void *fromHere, size_t &numBytes) {
	if (myBytes != nullptr) {
		free (myBytes);
	}
	myBytes = (char *) malloc (numBytes);
	mySize = numBytes;
	memmove (myBytes, fromHere, mySize);
	return ((char *) fromHere) + numBytes;
}

#endif

