
#ifndef PDBDATA_DESER_H_
#define PDBDATA_DESER_H_

#include <vector>
#include "PDBStoredDataType.h"
using namespace std;

// this is a factory used to deserialize PDBStoredData objects
class PDBStoredDataTypeDeserializationMachine {

public:

	// deserializes the next PDBStoredDataType object... the number of bytes processed
	// is written into numBytes... success is set to true if everything works
	PDBStoredDataTypePtr deSerialize (void *fromHere, size_t &numBytes, bool &success);

	// constructor... accepts a list of all of the different feature types that have
	// been registered in the catalog
	PDBStoredDataTypeDeserializationMachine (vector <PDBStoredDataTypePtr> &allDataTypes);

	// copy constructor
	PDBStoredDataTypeDeserializationMachine (const PDBStoredDataTypeDeserializationMachine &fromMe);

	// copy operator
	PDBStoredDataTypeDeserializationMachine &operator = (const PDBStoredDataTypeDeserializationMachine &fromMe);
	
	// register another type
	void registerNewStoredDataType (PDBStoredDataTypePtr regMe);

	PDBStoredDataTypeDeserializationMachine ();

private:

	vector <PDBStoredDataTypePtr> allRegisteredTypes;
	
};

#endif
