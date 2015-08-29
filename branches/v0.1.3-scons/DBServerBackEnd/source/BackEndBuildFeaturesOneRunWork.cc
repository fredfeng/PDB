
#ifndef BACKEND_BUILD_FEATURES_ONE_RUN_WORK_CC
#define BACKEND_BUILD_FEATURES_ONE_RUN_WORK_CC

#include "BackEndBuildFeaturesOneRunWork.h"
#include <string>
using namespace std;

BackEndBuildFeaturesOneRunWork :: BackEndBuildFeaturesOneRunWork (SimplePDBPtr workOnMeIn, 
	int whichListIn, PDBFeatureListBuilder &executeMeIn) : executeMe (executeMeIn) {
	workOnMe = workOnMeIn;
	whichList = whichListIn;
}

void BackEndBuildFeaturesOneRunWork :: execute (PDBBuzzerPtr callerBuzzer) {
	string errorMessage;
	if (workOnMe->addFeatures (whichList, executeMe, errorMessage)) {
		getLogger ()->writeLn ("BackEndQueryOneRunWork: " + errorMessage);
		callerBuzzer->buzz (PDBAlarm :: BuildFeatureError); 
	} else {
		callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
	}
}

#endif
