
#ifndef SERVER_WORK_BUZZER_H
#define SERVER_WORK_BUZZER_H

#include "EmptyPDBBuzzer.h"
#include "PDBAlarm.h"
#include "PDBBuzzer.h"
#include "ServerWork.h"

class ServerWorkBuzzer : public EmptyPDBBuzzer {

public:

	ServerWorkBuzzer (ServerWork &parent) : parent (parent) {}

        void handleBuzz (PDBAlarm withMe) override {
                if (withMe != PDBAlarm :: WorkAllDone) {
                        parent.handleError ();
                }
        }

private:

	ServerWork &parent;
};

#endif
