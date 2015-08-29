
#ifndef PDBFEATURELIST_H_
#define PDBFEATURELIST_H_

#include "PDBFeatureType.h"
#include "PDBFeatureTypeDeserializationMachine.h"
#include <memory>
#include <string>
#include <vector>
using namespace std;

// this encapsulates a list of extracted features that are associated with a 
// piece of data (such as a program element)
class PDBFeatureList {

public:

	PDBFeatureList () = default;

	// returns all of the features
	vector <PDBFeatureTypePtr> getFeatures ();

	// adds a new feature to the list of features, giving it the specified name 
	void addFeature (PDBFeatureTypePtr addMe, const string &name);

	// gets the number of features stored
	int getNumFeatures ();

	// gets the ith feature
	PDBFeatureTypePtr getIthFeature (int i);

	// gets the feature with the specified name
	PDBFeatureTypePtr getFeatureNamed (const string &nameToLookFor, bool &success);

	// serializes an instance of this particular feature list to the location toHere...
	// the number of bytes that are required to serialize the object is returned via
	// the parameter size, and a pointer to the next byte after the serialized object
	// is returned by the function (that is, toHere + size = returnVal)
	void *serialize (void *toHere, size_t &size);

	// this deserializes this feature list from the location fromHere.  The number
	// of bytes deserialized is written to the parameter size, and a pointer to the byte
	// after the last one deserialized is returned by the function (so fromHere + size =
	// returnVal).  The object machine is used to actually perfrom the deserialization of
	// the features
	void *deSerialize (void *fromHere, size_t &size, PDBFeatureTypeDeserializationMachine &machine);

private:

	// the actual list of features
        vector <PDBFeatureTypePtr> myFeatures;

	// and the names of the features
	vector <string> featureNames;
};

#endif
