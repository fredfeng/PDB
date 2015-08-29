
#ifndef BACKEND_OPEN_ONE_FILE_WORK_CC
#define BACKEND_OPEN_ONE_FILE_WORK_CC

#include "BackEndOpenOneFileWork.h"
#include <string>
using namespace std;

BackEndOpenOneFileWork :: BackEndOpenOneFileWork (SimplePDBPtr openMeIn, string fNameIn) {
	openMe = openMeIn;	
	fName = fNameIn;
}

void BackEndOpenOneFileWork :: execute (PDBBuzzerPtr callerBuzzer) {
	string errorMessage;
	if (openMe->loadUpFile (fName, errorMessage)) {
		getLogger ()->writeLn ("BackEndOpenOneFileWork: " + errorMessage);
		callerBuzzer->buzz (PDBAlarm :: OpenFileError); 
	} else {
		callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
	}

	getLogger ()->writeLn ("BackEndOpenOneFileWork: opened " + fName);
}

#endif
