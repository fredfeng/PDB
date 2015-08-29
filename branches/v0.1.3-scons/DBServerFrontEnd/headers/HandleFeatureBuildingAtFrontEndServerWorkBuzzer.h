
#ifndef HANDLE_FEATURE_BUILDING_FRONT_END_SERVER_WORK_BUZZER_H
#define HANDLE_FEATURE_BUILDING_FRONT_END_SERVER_WORK_BUZZER_H

#include "EmptyPDBBuzzer.h"
#include "PDBAlarm.h"
#include "PDBBuzzer.h"
#include "HandleFeatureBuildingAtFrontEndServerWork.h"

class HandleFeatureBuildingAtFrontEndServerWorkBuzzer : public EmptyPDBBuzzer {

public:

	HandleFeatureBuildingAtFrontEndServerWorkBuzzer (HandleFeatureBuildingAtFrontEndServerWork &parent) : parent (parent) {}

        void handleBuzz (PDBAlarm withMe) override {
                if (withMe == PDBAlarm :: FatalServerError) {
                        parent.wasAFatalServerError ();
                }
        }

private:

	HandleFeatureBuildingAtFrontEndServerWork &parent;
};

#endif
