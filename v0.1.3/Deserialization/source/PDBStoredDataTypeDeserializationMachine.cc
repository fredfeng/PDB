
#ifndef PDBDATA_DESER_C_
#define PDBDATA_DESER_C_

#include "FlatStoredData.h"
#include "PDBStoredDataTypeDeserializationMachine.h"
#include <iostream>
using namespace std;

PDBStoredDataTypePtr PDBStoredDataTypeDeserializationMachine :: deSerialize (void *fromHereIn, size_t &numBytes, bool &success) {

	// so we can do pointer arithmatic
	char *fromHere = static_cast<char *> (fromHereIn);

	// read in the type key
	int *code = reinterpret_cast<int *> (fromHere);

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
	return {};
}

PDBStoredDataTypePtr PDBStoredDataTypeDeserializationMachine :: deSerialize (void *fromHereIn, size_t &numBytes) {

	bool success;
	PDBStoredDataTypePtr result = deSerialize (fromHereIn, numBytes, success);
	if (success == false) {
		result = make_shared <FlatStoredData> ();
		result->deSerialize (fromHereIn, numBytes);
	}

	return result;
}

PDBStoredDataTypeDeserializationMachine :: PDBStoredDataTypeDeserializationMachine (vector <PDBStoredDataTypePtr> &allDataTypes) {
	allRegisteredTypes = allDataTypes;
}

void PDBStoredDataTypeDeserializationMachine :: registerNewStoredDataType (PDBStoredDataTypePtr regMe) {
	allRegisteredTypes.push_back (regMe);
}

#endif
