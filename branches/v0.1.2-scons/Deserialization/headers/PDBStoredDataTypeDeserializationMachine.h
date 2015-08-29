
#ifndef PDBDATA_DESER_H_
#define PDBDATA_DESER_H_

#include "PDBStoredDataType.h"
#include <vector>
using namespace std;

// this is a factory used to deserialize PDBStoredData objects
class PDBStoredDataTypeDeserializationMachine {

public:

	// deserializes the next PDBStoredDataType object... the number of bytes processed
	// is written into numBytes... success is set to true if everything works
	PDBStoredDataTypePtr deSerialize (void *fromHere, size_t &numBytes, bool &success);

	// constructor... accepts a list of all of the different feature types that have
	// been registered in the catalog
	explicit PDBStoredDataTypeDeserializationMachine (vector <PDBStoredDataTypePtr> &allDataTypes);
	
	// register another type
	void registerNewStoredDataType (PDBStoredDataTypePtr regMe);

	PDBStoredDataTypeDeserializationMachine () = default;

private:

	vector <PDBStoredDataTypePtr> allRegisteredTypes;
	
};

#endif
