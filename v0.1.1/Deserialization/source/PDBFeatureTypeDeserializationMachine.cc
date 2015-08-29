
#ifndef PDBFEATURE_DESER_C_
#define PDBFEATURE_DESER_C_

#include "PDBFeatureTypeDeserializationMachine.h"
#include <iostream>
using namespace std;

PDBFeatureTypeDeserializationMachine :: PDBFeatureTypeDeserializationMachine () {}

PDBFeatureTypePtr PDBFeatureTypeDeserializationMachine :: deSerialize (void *fromHereVoid, 
	size_t &numBytes, bool &success) {

	// get a character pointer so that we can do arithmatic
	char *fromHere = (char *) fromHereVoid;

	// read in the type key
	int *code = (int *) fromHere;

	fromHere += sizeof (int);

	// run through all of the registered types, and look for one that matches
	for (PDBFeatureTypePtr p : allRegisteredTypes) {

		// we found a correct type
		if (p->getCode () == *code) {
			PDBFeatureTypePtr newObject = p->getNewFeatureTypeObject ();
			size_t bytesRead;
			newObject->deSerialize (fromHere, bytesRead);
			numBytes = bytesRead + sizeof (int);
			success = true;
			return newObject;	
		}	
	}

	// if we got here, deserialization failed because we could not find a type that matched
	success = false;
	numBytes = sizeof (int);
	PDBFeatureTypePtr empty;
	return empty;
}

PDBFeatureTypeDeserializationMachine :: PDBFeatureTypeDeserializationMachine (vector <PDBFeatureTypePtr> &allDataTypes) {
	allRegisteredTypes = allDataTypes;
}

PDBFeatureTypeDeserializationMachine &PDBFeatureTypeDeserializationMachine :: operator =  (const PDBFeatureTypeDeserializationMachine &fromMe) {
	allRegisteredTypes = fromMe.allRegisteredTypes;
}

PDBFeatureTypeDeserializationMachine :: PDBFeatureTypeDeserializationMachine (const PDBFeatureTypeDeserializationMachine &fromMe) {
	allRegisteredTypes = fromMe.allRegisteredTypes;
}

void PDBFeatureTypeDeserializationMachine :: registerNewFeatureType (PDBFeatureTypePtr regMe) {
	allRegisteredTypes.push_back (regMe);
}

#endif
