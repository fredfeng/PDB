
#ifndef BACK_END_OPEN_FILES_WORK_BUZZER_H
#define BACK_END_OPEN_FILES_WORK_BUZZER_H

#include "BackEndOpenFilesWork.h"
#include "EmptyPDBBuzzer.h"
#include "PDBAlarm.h"

class BackEndOpenFilesWorkBuzzer : public EmptyPDBBuzzer {

public:

	BackEndOpenFilesWorkBuzzer (BackEndOpenFilesWork &parent) : parent (parent) {}

	void handleBuzz (PDBAlarm withMe) override {
		if (withMe == PDBAlarm :: OpenFileError) {
			parent.wasError ();
		} else if (withMe == PDBAlarm :: WorkAllDone) {
			parent.fileDone ();
		}
	}

private:

	BackEndOpenFilesWork &parent;
};

#endif
