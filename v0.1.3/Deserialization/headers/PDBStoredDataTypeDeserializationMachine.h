
#ifndef PDBDATA_DESER_H_
#define PDBDATA_DESER_H_

#include "PDBStoredDataType.h"
#include <vector>
using namespace std;

// this is a factory used to deserialize PDBStoredData objects
class PDBStoredDataTypeDeserializationMachine {

public:

        // deserializes the next PDBStoredDataType object... the number of bytes
        // processed is put into numBytes... note that in the case that the tpye
        // of the feature encountered does not match any registered type, then
        // an object of type PDBUnknownStoredDataType is created and returned, and
        // exactly numBytes bytes from fromHere are used to create it
	PDBStoredDataTypePtr deSerialize (void *fromHere, size_t &numBytes, bool &success);

        // deserializes, but will not fail if a matching type code for the object
        // to be deserialized cannot be found... in this case, the object is deserialized
        // into a generic, flat object that cannot be used to do anything, but it can
        // be re-serialized.  This is used in the case that we are simply forwarding the
        // object on to someone else, and we actually don't care about doing anything more
        // than storing his bytes into an object before we re-send them over a wire
	PDBStoredDataTypePtr deSerialize (void *fromHere, size_t &numBytes);

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
