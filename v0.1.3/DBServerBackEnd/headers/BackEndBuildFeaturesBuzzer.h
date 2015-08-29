
#ifndef BACKEND_WRITE_BACK_DATA_WORK_BUZZER_H
#define BACKEND_WRITE_BACK_DATA_WORK_BUZZER_H

#include "BackEndBuildFeaturesWork.h"
#include "PDBAlarm.h"
#include "PDBBuzzer.h"

class BackEndBuildFeaturesBuzzer : public PDBBuzzer {

public:

	BackEndBuildFeaturesBuzzer (BackEndBuildFeaturesWork &parent) : parent (parent) {}

	void handleBuzz (PDBAlarm withMe) override {
		if (withMe == PDBAlarm :: WorkAllDone) {
			parent.taskDone ();
		} else if (withMe == PDBAlarm :: BuildFeatureError) {
			parent.setError ();
		}
	}

private:

	BackEndBuildFeaturesWork &parent;
};

#endif
