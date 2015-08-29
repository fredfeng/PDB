
#ifndef HANDLE_SHUT_DOWN_FRONT_END_SERVER_WORK_BUZZER_H
#define HANDLE_SHUT_DOWN_FRONT_END_SERVER_WORK_BUZZER_H

#include "PDBAlarm.h"
#include "EmptyPDBBuzzer.h"
#include "HandleQueryAtFrontEndServerWork.h"

class HandleShutDownAtFrontEndServerWorkBuzzer : public EmptyPDBBuzzer {

public:

	HandleShutDownAtFrontEndServerWorkBuzzer (HandleShutDownAtFrontEndServerWork &parent) : parent (parent) {}

        void handleBuzz (PDBAlarm withMe) override {
                if (withMe == PDBAlarm :: FatalServerError) {
                        parent.wasAFatalServerError ();
                }
        }

private:

	HandleShutDownAtFrontEndServerWork &parent;
};

#endif
