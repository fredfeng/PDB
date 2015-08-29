
#ifndef FEATURE_LIST_BUILDER_H
#define FEATURE_LIST_BUILDER_H

#include "PDBFeatureBuilder.h"
#include <vector>
using namespace std;

// this class is used by a PDB instance to add one or more features to all of
// the objects that are stored in the database.  For the time being, this is 
// nothing more than a container for one or more PDBFeatureBuilder objects, that
// actually do the work of building new features from a PDBData object
class PDBFeatureListBuilder {

public:

	// gets all of the contained feature builders
	vector <PDBFeatureBuilderPtr> getFeatureBuilders ();

	// adds another feature builder
	void add (PDBFeatureBuilderPtr addMe);

private:

	vector <PDBFeatureBuilderPtr> allBuilders;
};

#endif
