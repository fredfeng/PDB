
#ifndef BACKEND_BUILD_FEATURES_ONE_RUN_WORK_H
#define BACKEND_BUILD_FEATURES_ONE_RUN_WORK_H

#include "PDBBuzzer.h"
#include "PDBFeatureListBuilder.h"
#include "PDBWork.h"
#include "SimplePDB.h"

// queries one run of records in this database
class BackEndBuildFeaturesOneRunWork : public PDBWork {

public:

	// specify the database objec, the run to query, and the query to run
	BackEndBuildFeaturesOneRunWork (SimplePDBPtr workOnMe, int whichListIn, PDBFeatureListBuilder &executeMe);

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

private:

	// the database that we are tasked with querying
	SimplePDBPtr workOnMe;

	// the run to build the features for
	int whichList;

	// the task to run
	PDBFeatureListBuilder &executeMe;
};

#endif

