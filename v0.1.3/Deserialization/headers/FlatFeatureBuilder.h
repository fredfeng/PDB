
#ifndef FLAT_FEATURE_BUILDER_H
#define FLAT_FEATURE_BUILDER_H

#include <memory>
using namespace std;

class FlatFeatureBuilder;
typedef shared_ptr <FlatFeatureBuilder> FlatFeatureBuilderPtr;

#include "PDBFeatureBuilder.h"

// this class is used as a temporary data object that exists only temporarily, when
// we do not want to actually unflatten a data object... you use this when you 
// want to create a transient object that just exists long enough to re-serialize it
class FlatFeatureBuilder : public PDBFeatureBuilder {

public:

	// constructor, destructor
	FlatFeatureBuilder ();
	~FlatFeatureBuilder ();

	// these are inherited from PDBFeatureBuilder
	const string &getFeatureName() override;
	int getCode () override;
	bool runsOnThisPDBData (PDBData &checkMe) override;
	PDBFeatureTypePtr buildFeature (PDBData &fromMe, bool &wasError, string &errMsg) override;
	void *serialize (void *toHere, size_t &numBytes) override;
	void *deSerialize (void *fromHere, size_t &numBytes) override;
	PDBFeatureBuilderPtr getEmptyBuilder ();

private:

	// the bytes we store, plus the size
	char *myBytes;
	size_t mySize;

	// our name
	string myName;

	// stores the code of the REAL type we happen to be storing
	int myCode;

};

#endif

