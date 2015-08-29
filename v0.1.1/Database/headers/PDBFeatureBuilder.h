
#ifndef FEATURE_BUILDER_H
#define FEATURE_BUILDER_H

#include <vector>
#include <memory>
using namespace std;
// create a smart pointer for PDBFetureBuilder objects
class PDBFeatureBuilder;
typedef shared_ptr <PDBFeatureBuilder> PDBFeatureBuilderPtr;

#include "PDBFeatureBuilder.h"
#include "PDBFeatureType.h"
#include "PDBData.h"

// this class encapsulates the task of creating a new feature that
// will be added to a PDBData object
class PDBData;
class PDBFeatureBuilder {

public:

	// see if we are actually going to run on this particular data object...
	// it might check to make sure that this data object is of the right type,
	// or to see if the data object has not already had the feature built
	virtual bool runsOnThisPDBData (PDBData &checkMe) = 0;

	// build a new feature for this guy... PDB will not run this operation
	// on fromMe unless runsOnThisPDBData (fromMe) has returned true
	virtual PDBFeatureTypePtr buildFeature (PDBData &fromMe) = 0;

	// returns the name of the feature that this guy is being asked to build 
	virtual string getFeatureName () = 0;

	// assignment operator... PDB will make sure that we don't try to assign
	// two PDBFeatureBuilder objects whose concrete types are different!!
	PDBFeatureBuilder &operator = (PDBFeatureBuilderPtr copyMeIn);

        // serializes this particular object to the location toHere...
        // the number of bytes that are required to serialize the object is returned via
        // the parameter size, and a pointer to the next byte after the serialized object
        // is returned by the function (that is, toHere + size = returnVal)
        virtual void *serialize(void *toHere, size_t &size) = 0;

        // this deserializes this particular object from the location fromHere.  The number
        // of bytes deserialized is written to the parameter size, and a pointer to the byte
        // after the last one deserialized is returned by the function (so fromHere + size =
        // returnVal).
        virtual void *deSerialize(void *fromHere, size_t &size) = 0;
};

#endif
