
#ifndef FILE_NAME_BUILDER_H
#define FILE_NAME_BUILDER_H

#include "PDBFeatureBuilder.h"
#include "PDBFeatureType.h"

class FileNameBuilder : public PDBFeatureBuilder {

private:

	string myName;

public:

	// accepts the name of the feature that we are building
	explicit FileNameBuilder (const string &myName);

	// default constructor
	FileNameBuilder () = default;

	// from the interface
	bool runsOnThisPDBData (PDBData &checkMe) override;
	PDBFeatureTypePtr buildFeature (PDBData &fromMe, bool &wasError, string &errMsg) override;
	const string &getFeatureName () override;
	PDBFeatureBuilder &operator = (PDBFeatureBuilderPtr copyMeIn);
        void *serialize(void *toHere, size_t &size) override;
        void *deSerialize(void *fromHere, size_t &size) override;
	PDBFeatureBuilderPtr getEmptyBuilder () override;
	int getCode () override;

};

#endif

