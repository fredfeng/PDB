
#ifndef PDBFEATURE_BUILDER_DESER_H_
#define PDBFEATURE_BUILDER_DESER_H_

#include <vector>
#include "PDBFeatureBuilder.h"
using namespace std;

// this is a factory that is used to deserialize feature builders
class PDBFeatureBuilderDeserializationMachine {

public:

	// deserializes the next PDBFeatureBuilder object... the number of bytes
	// processed is put into numBytes... note that in the case that the tpye
	// of the feature encountered does not match any registered type, then
	// an object of type PDBUnknownFeatureBuilder is created and returned, and
	// exactly numBytes bytes from fromHere are used to create it
	PDBFeatureBuilderPtr deSerialize (void *fromHere, size_t &numBytes, bool &success);

	// deserializes, but will not fail if a matching type code for the object
	// to be deserialized cannot be found... in this case, the object is deserialized
	// into a generic, flat object that cannot be used to do anything, but it can
	// be re-serialized.  This is used in the case that we are simply forwarding the
	// object on to someone else, and we actually don't care about doing anything more
	// than storing his bytes into an object before we re-send them over a wire
	PDBFeatureBuilderPtr deSerialize (void *fromHere, size_t &numBytes);

	// constructor... accepts a list of all of the different feature types that have
	// been registered in the catalog
	explicit PDBFeatureBuilderDeserializationMachine (vector <PDBFeatureBuilderPtr> &allFeatureBuilders);

	// register another feature type
	void registerNewFeatureBuilders (PDBFeatureBuilderPtr regMe);

	PDBFeatureBuilderDeserializationMachine () = default;

private:

	vector <PDBFeatureBuilderPtr> allRegisteredTypes;
	
};

#endif
