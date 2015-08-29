
#ifndef ADD_FEATURES_H
#define ADD_FEATURES_H

#include "PDBFeatureListBuilder.h"
#include "PDBWork.h"
#include "SimplePDB.h"

// this is a class, that when executed, causes zero or more features to be added to all
// of the data objects in a particular run
class AddFeaturesWork : public PDBWork {

public:

	AddFeaturesWork (SimplePDB *useMe, int whichListIn, PDBFeatureListBuilder &builderIn) {
		myPDB = useMe;
		whichList = whichListIn;
		builder = builderIn;
	}

	void execute () override {
		string errMsg;
		if (myPDB->addFeatures (whichList, builder, errMsg))
			setError (errMsg);
		done ();
	}

private:

	SimplePDB *myPDB;
	int whichList;
	PDBFeatureListBuilder builder;
};

#endif

