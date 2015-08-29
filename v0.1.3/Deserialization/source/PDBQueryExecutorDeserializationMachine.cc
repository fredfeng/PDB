
#ifndef PDB_QUERY_DESER_C_
#define PDB_QUERY_DESER_C_

#include "FlatQueryExecutor.h"
#include "PDBQueryExecutorDeserializationMachine.h"
#include <iostream>
using namespace std;

PDBQueryExecutorPtr PDBQueryExecutorDeserializationMachine :: deSerialize (void *fromHereVoid, 
	size_t &numBytes, bool &success) {

	// get a character pointer so that we can do arithmatic
	char *fromHere = static_cast<char *> (fromHereVoid);

	// read in the type key
	int *code = reinterpret_cast<int *> (fromHere);
	fromHere += sizeof (int);

	// run through all of the registered types, and look for one that matches
	for (PDBQueryExecutorPtr p : allRegisteredTypes) {

		// we found a correct type
		if (p->getCode () == *code) {
			PDBQueryExecutorPtr newObject = p->getEmpty ();
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

PDBQueryExecutorPtr PDBQueryExecutorDeserializationMachine :: deSerialize (void *fromHereIn, size_t &numBytes) {

	bool success;
        PDBQueryExecutorPtr result = deSerialize (fromHereIn, numBytes, success);
        if (success == false) {
                result = make_shared <FlatQueryExecutor> ();
                result->deSerialize (fromHereIn, numBytes);
        }

        return result;
}


PDBQueryExecutorDeserializationMachine :: PDBQueryExecutorDeserializationMachine (vector <PDBQueryExecutorPtr> &allDataTypes) {
	allRegisteredTypes = allDataTypes;
}

void PDBQueryExecutorDeserializationMachine :: registerNewQueryExecutor (PDBQueryExecutorPtr regMe) {
	allRegisteredTypes.push_back (regMe);
}

#endif
