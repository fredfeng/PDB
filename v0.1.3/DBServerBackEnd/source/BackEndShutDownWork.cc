
#ifndef BACKEND_SHUT_DOWN_WORK_CC
#define BACKEND_SHUT_DOWN_WORK_CC

#include <string>
#include "BackEndShutDownWork.h"
using namespace std;

BackEndShutDownWork :: BackEndShutDownWork () {}

PDBCommWorkPtr BackEndShutDownWork :: clone () {
	PDBCommWorkPtr returnVal {make_shared <BackEndShutDownWork> ()};
	return returnVal;
}

void BackEndShutDownWork :: execute (PDBBuzzerPtr callerBuzzer) {
	
	string err = "", info = "";
	if (getCommunicator ()->acceptShutDown (err, info)) {
		getLogger ()->writeLn ("BackEndShutDownWork: error getting shut down message " + err);
		err = "error reading shut down message: " + err;
		getCommunicator ()->sendError (err);
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	getLogger ()->writeLn ("BackEndShutDownWork: Got shutdown message " + info);

	// and send back the results
	info = "";
	if (getCommunicator ()->sendShutDownComplete (info)) {
		getLogger ()->writeLn ("BackEndShutDownWork: could not ack the server shutdown");
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif
