
#ifndef FILE_NAME_BUILDER_H
#define FILE_NAME_BUILDER_H

#include "PDBFeatureType.h"
#include "PDBFeatureBuilder.h"

class FileNameBuilder : public PDBFeatureBuilder {

private:

	string myName;

public:

	// accepts the name of the feature that we are building
	FileNameBuilder (string myName);

	// from the interface
	bool runsOnThisPDBData (PDBData &checkMe);
	PDBFeatureTypePtr buildFeature (PDBData &fromMe);
	string getFeatureName ();
	PDBFeatureBuilder &operator = (PDBFeatureBuilderPtr copyMeIn);
        void *serialize(void *toHere, size_t &size);
        void *deSerialize(void *fromHere, size_t &size);

};

#endif

