
#ifndef PDBFEATURE_DESER_H_
#define PDBFEATURE_DESER_H_

#include "PDBFeatureType.h"
#include <vector>
using namespace std;

// this is a factory that is used to deserialize features
class PDBFeatureTypeDeserializationMachine {

public:

	// deserializes the next PDBFeatureType object... the number of bytes
	// processed is put into numBytes... note that in the case that the tpye
	// of the feature encountered does not match any registered type, then
	// an object of type PDBUnknownFeatureType is created and returned, and
	// exactly numBytes bytes from fromHere are used to create it
	PDBFeatureTypePtr deSerialize (void *fromHere, size_t &numBytes, bool &success);

	// deserializes, but will not fail if a matching type code for the object
	// to be deserialized cannot be found... in this case, the object is deserialized
	// into a generic, flat object that cannot be used to do anything, but it can
	// be re-serialized.  This is used in the case that we are simply forwarding the
	// object on to someone else, and we actually don't care about doing anything more
	// than storing his bytes into an object before we re-send them over a wire
	PDBFeatureTypePtr deSerialize (void *fromHere, size_t &numBytes);

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
