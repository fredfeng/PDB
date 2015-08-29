
#ifndef BACKEND_WRITE_BACK_DATA_WORK_CC
#define BACKEND_WRITE_BACK_DATA_WORK_CC

#include "BackEndWriteBackDataWork.h"
#include <string>
using namespace std;

BackEndWriteBackDataWork :: BackEndWriteBackDataWork (SimplePDBPtr writeMeIn, int whichListIn) {
	writeMe = writeMeIn;
	whichList = whichListIn;
}

void BackEndWriteBackDataWork :: execute (PDBBuzzerPtr callerBuzzer) {
	string errorMessage;
	if (writeMe->writeBackData (whichList, errorMessage)) {
		getLogger ()->writeLn ("BackEndWriteBackDataWork: " + errorMessage);
		callerBuzzer->buzz (PDBAlarm :: BuildFeatureError); 
	} else {
		callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
	}
}

#endif
