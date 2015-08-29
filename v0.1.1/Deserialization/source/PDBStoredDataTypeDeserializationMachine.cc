
#ifndef PDBDATA_DESER_C_
#define PDBDATA_DESER_C_

#include <iostream>
#include "PDBStoredDataTypeDeserializationMachine.h"
using namespace std;

PDBStoredDataTypeDeserializationMachine :: PDBStoredDataTypeDeserializationMachine () {}

PDBStoredDataTypePtr PDBStoredDataTypeDeserializationMachine :: deSerialize (void *fromHereIn, size_t &numBytes, bool &success) {

	// so we can do pointer arithmatic
	char *fromHere = (char *) fromHereIn;

	// read in the type key
	int *code = (int *) fromHere;

	fromHere += sizeof (int);

	// run through all of the registered types, and look for one that matches
	for (PDBStoredDataTypePtr p : allRegisteredTypes) {

		// we found a correct type
		if (p->getCode () == *code) {
			PDBStoredDataTypePtr newObject = p->getNewStoredDataTypeObject ();
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
	PDBStoredDataTypePtr empty;
	return empty;
}

PDBStoredDataTypeDeserializationMachine :: PDBStoredDataTypeDeserializationMachine (vector <PDBStoredDataTypePtr> &allDataTypes) {
	allRegisteredTypes = allDataTypes;
}

PDBStoredDataTypeDeserializationMachine &PDBStoredDataTypeDeserializationMachine :: operator =  (const PDBStoredDataTypeDeserializationMachine &fromMe) {
	allRegisteredTypes = fromMe.allRegisteredTypes;
}

PDBStoredDataTypeDeserializationMachine :: PDBStoredDataTypeDeserializationMachine (const PDBStoredDataTypeDeserializationMachine &fromMe) {
	allRegisteredTypes = fromMe.allRegisteredTypes;
}

void PDBStoredDataTypeDeserializationMachine :: registerNewStoredDataType (PDBStoredDataTypePtr regMe) {
	allRegisteredTypes.push_back (regMe);
}

#endif
