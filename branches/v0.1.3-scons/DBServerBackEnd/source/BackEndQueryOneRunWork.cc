
#ifndef BACKEND_QUERY_ONE_RUN_WORK_CC
#define BACKEND_QUERY_ONE_RUN_WORK_CC

#include "BackEndQueryOneRunWork.h"
#include <string>
using namespace std;

BackEndQueryOneRunWork :: BackEndQueryOneRunWork (SimplePDBPtr queryMeIn, 
	int whichListIn, PDBQueryExecutorPtr runMeIn) {
	queryMe = queryMeIn;
	whichList = whichListIn;
	runMe = runMeIn;
}

PDBQueryExecutorPtr BackEndQueryOneRunWork :: getResult () {
	return runMe;
}

void BackEndQueryOneRunWork :: execute (PDBBuzzerPtr callerBuzzer) {
	string errorMessage;
	if (queryMe->runQuery (runMe, whichList, errorMessage)) {
		getLogger ()->writeLn ("BackEndQueryOneRunWork: " + errorMessage);
		callerBuzzer->buzz (PDBAlarm :: QueryError); 
	} else {
		callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
	}
}

#endif
