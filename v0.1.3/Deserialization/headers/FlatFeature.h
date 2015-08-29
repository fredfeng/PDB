
#ifndef FLAT_FEATURE_H
#define FLAT_FEATURE_H

#include <memory>
using namespace std;

class FlatFeature;
typedef shared_ptr <FlatFeature> FlatFeaturePtr;

#include "PDBFeatureType.h"

// this class is used as a temporary data object that exists only temporarily, when
// we do not want to actually unflatten a data object... you use this when you 
// want to create a transient object that just exists long enough to re-serialize it
class FlatFeature : public PDBFeatureType {

public:

	// constructor, destructor
	FlatFeature ();
	~FlatFeature ();

	// these are inherited from PDBFeatureType
	const string &getTypeName() override;
	int getCode () override;
	void *serialize (void *toHere, size_t &numBytes) override;
	void *deSerialize (void *fromHere, size_t &numBytes) override;
	PDBFeatureTypePtr getNewFeatureTypeObject () override;
	string display () override;

private:

	char *myBytes;
	size_t mySize;
	string myName;
};

#endif

