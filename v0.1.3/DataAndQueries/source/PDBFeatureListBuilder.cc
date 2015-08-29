
#ifndef FEATURE_LIST_BUILDER_C
#define FEATURE_LIST_BUILDER_C

#include <iostream>
#include "PDBFeatureListBuilder.h"

using namespace std;

vector <PDBFeatureBuilderPtr> PDBFeatureListBuilder :: getFeatureBuilders () {
	return allBuilders;
}

void PDBFeatureListBuilder :: add (PDBFeatureBuilderPtr addMe) {
	allBuilders.push_back (addMe);
}

void *PDBFeatureListBuilder :: serialize (void *toHereIn, size_t &numBytes) {

	// write out the number of builders
	char *toHere = (char *) toHereIn;
	*((int *) toHere) = allBuilders.size ();
	toHere += sizeof (int);

	// now write out each builder
	for (PDBFeatureBuilderPtr p : allBuilders) {

		// reserve some space for the size of the builder
		size_t *sizeSlot = (size_t *) toHere;
		toHere += sizeof (size_t);

		// write the code
		*((int *) toHere) = p->getCode ();
		toHere += sizeof (int);

		// write the data
		toHere = (char *) p->serialize (toHere, numBytes);	

		// and write the number of bytes
		*sizeSlot = numBytes;	
	}	

	// compute the overall size and return
	numBytes = (size_t) (toHere - (char *) toHereIn);
	return toHere;
}

void *PDBFeatureListBuilder :: deSerialize (PDBFeatureBuilderDeserializationMachine &machine, 
	void *fromHereIn, size_t &numBytes) {

	// make sure we have no residual data
	allBuilders.clear ();

	// get the number of builders
	char *fromHere = (char *) fromHereIn;	
	int size = *((int *) fromHere);
	fromHere += sizeof (int);

	// now read in each builder	
	for (int i = 0; i < size; i++) {

		// read in the size of the builder
		size_t *sizeSlot = (size_t *) fromHere;
		size_t bytes = *sizeSlot;
		fromHere += sizeof (size_t);

		// deserialize
		allBuilders.push_back (machine.deSerialize (fromHere, bytes));
		fromHere += bytes;
	}	

	// compute the overall size and return
	numBytes = (size_t) (fromHere - (char *) fromHereIn);
	return fromHere;
}

#endif
