
#ifndef HANDLE_QUERY_FRONT_END_SERVER_WORK_BUZZER_H
#define HANDLE_QUERY_FRONT_END_SERVER_WORK_BUZZER_H

#include "PDBAlarm.h"
#include "EmptyPDBBuzzer.h"
#include "HandleQueryAtFrontEndServerWork.h"

class HandleQueryAtFrontEndServerWorkBuzzer : public EmptyPDBBuzzer {

public:

	HandleQueryAtFrontEndServerWorkBuzzer (HandleQueryAtFrontEndServerWork &parent) : parent (parent) {}

        void handleBuzz (PDBAlarm withMe) override {
                if (withMe == PDBAlarm :: FatalServerError) {
                        parent.wasAFatalServerError ();
                }
        }

private:

	HandleQueryAtFrontEndServerWork &parent;
};

#endif
