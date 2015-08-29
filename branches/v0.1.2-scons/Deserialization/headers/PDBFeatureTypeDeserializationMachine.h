
#ifndef PDBFEATURE_DESER_H_
#define PDBFEATURE_DESER_H_

#include "PDBFeatureType.h"
#include <vector>
using namespace std;

// this is a factory that is used to deserialize features
class PDBFeatureTypeDeserializationMachine {

public:

	// deserializes the next PDBFeatureType object... the number of bytes
	// processed is put into numBytes
	PDBFeatureTypePtr deSerialize (void *fromHere, size_t &numBytes, bool &success);

	// constructor... accepts a list of all of the different feature types that have
	// been registered in the catalog
	explicit PDBFeatureTypeDeserializationMachine (vector <PDBFeatureTypePtr> &allFeatureTypes);

	// register another feature type
	void registerNewFeatureType (PDBFeatureTypePtr regMe);

	PDBFeatureTypeDeserializationMachine () = default;

private:

	vector <PDBFeatureTypePtr> allRegisteredTypes;
	
};

#endif
