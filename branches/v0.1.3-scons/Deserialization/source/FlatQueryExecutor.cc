
#ifndef FLAT_QUERY_C
#define FLAT_QUERY_C

#include "FlatQueryExecutor.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

FlatQueryExecutor :: FlatQueryExecutor () {
	myBytes = nullptr;
	mySize = 0;
}

FlatQueryExecutor :: ~FlatQueryExecutor () {
	free (myBytes);	
}

int FlatQueryExecutor :: getCode () {

	// the code should be encoded in the first few bytes
	if (myBytes != nullptr) {
		return *((int *) myBytes);
	}

	return 9459435;
}

PDBQueryExecutor &FlatQueryExecutor :: operator += (PDBQueryExecutorPtr rhs) {
	rhs = nullptr;
	return *this;
}

PDBQueryExecutorPtr FlatQueryExecutor :: getEmpty () {
	return make_shared <FlatQueryExecutor> ();
}

void FlatQueryExecutor :: aggregate (PDBData &aggMe) {
	
	// just so it will compile, we need to do something
	aggMe.getFeatures ();
}

bool FlatQueryExecutor ::  wasAnError (string &) {
	return false;
}

string FlatQueryExecutor :: display () {
	return "flat query executor";
}

void *FlatQueryExecutor :: serialize (void *toHere, size_t &numBytes) {
	
	// note that we DO NOT serialize the type code, which is going to
	// be written at the front of this guy
	memmove (toHere, myBytes + sizeof (int), mySize - sizeof (int));
	numBytes = mySize - sizeof (int);
	return ((char *) toHere) + numBytes;
}

void *FlatQueryExecutor :: deSerialize (void *fromHere, size_t &numBytes) {
	if (myBytes != nullptr) {
		free (myBytes);
	}
	myBytes = (char *) malloc (numBytes);
	mySize = numBytes;
	memmove (myBytes, fromHere, numBytes);
	return ((char *) fromHere) + numBytes;
}

#endif

