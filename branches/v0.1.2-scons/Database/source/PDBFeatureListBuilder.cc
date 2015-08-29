
#ifndef FEATURE_LIST_BUILDER_C
#define FEATURE_LIST_BUILDER_C

#include "PDBFeatureListBuilder.h"

vector <PDBFeatureBuilderPtr> PDBFeatureListBuilder :: getFeatureBuilders () {
	return allBuilders;
}

void PDBFeatureListBuilder :: add (PDBFeatureBuilderPtr addMe) {
	allBuilders.push_back (addMe);
}

#endif
