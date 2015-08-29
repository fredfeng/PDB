
#ifndef PDBFEATURE_DESER_C_
#define PDBFEATURE_DESER_C_

#include "PDBFeatureTypeDeserializationMachine.h"
#include "FlatFeature.h"
#include <iostream>
using namespace std;

PDBFeatureTypePtr PDBFeatureTypeDeserializationMachine :: deSerialize (void *fromHereVoid, 
	size_t &numBytes, bool &success) {

	// get a character pointer so that we can do arithmatic
	char *fromHere = static_cast<char *> (fromHereVoid);

	// read in the type key
	int *code = reinterpret_cast<int *> (fromHere);

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
	return {};
}

PDBFeatureTypePtr PDBFeatureTypeDeserializationMachine :: deSerialize (void *fromHereIn, size_t &numBytes) {

	bool success;
        PDBFeatureTypePtr result = deSerialize (fromHereIn, numBytes, success);
        if (success == false) {
                result = make_shared <FlatFeature> ();
                result->deSerialize (fromHereIn, numBytes);
        }

        return result;
}


PDBFeatureTypeDeserializationMachine :: PDBFeatureTypeDeserializationMachine (vector <PDBFeatureTypePtr> &allDataTypes) {
	allRegisteredTypes = allDataTypes;
}

void PDBFeatureTypeDeserializationMachine :: registerNewFeatureType (PDBFeatureTypePtr regMe) {
	allRegisteredTypes.push_back (regMe);
}

#endif
