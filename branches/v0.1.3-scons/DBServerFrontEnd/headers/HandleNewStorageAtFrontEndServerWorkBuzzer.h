
#ifndef HANDLE_NEW_STORAGE_FRONT_END_SERVER_WORK_BUZZER_H
#define HANDLE_NEW_STORAGE_FRONT_END_SERVER_WORK_BUZZER_H

#include "EmptyPDBBuzzer.h"
#include "PDBAlarm.h"
#include "PDBBuzzer.h"
#include "HandleNewStorageAtFrontEndServerWork.h"

class HandleNewStorageAtFrontEndServerWorkBuzzer : public EmptyPDBBuzzer {

public:

	HandleNewStorageAtFrontEndServerWorkBuzzer (HandleNewStorageAtFrontEndServerWork &parent) : parent (parent) {}

        void handleBuzz (PDBAlarm withMe) override {
                if (withMe == PDBAlarm :: FatalServerError) {
                        parent.wasAFatalServerError ();
                }
        }

private:

	HandleNewStorageAtFrontEndServerWork &parent;
};

#endif
