
#ifndef HANDLE_DATA_FRONT_END_SERVER_WORK_BUZZER_H
#define HANDLE_DATA_FRONT_END_SERVER_WORK_BUZZER_H

#include "EmptyPDBBuzzer.h"
#include "HandleDataAtFrontEndServerWork.h"
#include "PDBAlarm.h"

class HandleDataAtFrontEndServerWorkBuzzer : public EmptyPDBBuzzer {

public:

	HandleDataAtFrontEndServerWorkBuzzer (HandleDataAtFrontEndServerWork &parent) : parent (parent) {}

	void handleBuzz (PDBAlarm withMe) override {
		if (withMe == PDBAlarm :: FatalServerError) {
			parent.wasAFatalServerError ();
		}
	}

private:

	HandleDataAtFrontEndServerWork &parent;
};

#endif
