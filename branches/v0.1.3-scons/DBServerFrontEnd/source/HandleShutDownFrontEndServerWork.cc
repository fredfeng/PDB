
#ifndef HANDLE_SHUT_DOWN_FRONT_END_SERVER_WORK_C
#define HANDLE_SHUT_DOWN_FRONT_END_SERVER_WORK_C

#include "HandleShutDownAtFrontEndServerWork.h"
#include "HandleShutDownAtFrontEndServerWorkBuzzer.h"
#include "PDBCommWork.h"
#include "SendErrorToClientWork.h"
#include <unistd.h>

void HandleShutDownAtFrontEndServerWork :: wasAFatalServerError () {

	// there was a fatal error!
	wasFatalError = true;
	getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: Got a server error!!\n");

	// send an error message to the client
	PDBWorkerPtr myWorker = getWorker ();
	PDBWorkPtr errorWork {make_shared <SendErrorToClientWork> (getCommunicator (), "Error at the backend server: " + info)};	
	myWorker->execute (errorWork, errorWork->getLinkedBuzzer ());
}

PDBCommWorkPtr HandleShutDownAtFrontEndServerWork :: clone () {
	return {make_shared <HandleShutDownAtFrontEndServerWork> (pathOfBackend)};	
}

PDBBuzzerPtr HandleShutDownAtFrontEndServerWork :: getLinkedBuzzer () {
	return {make_shared <HandleShutDownAtFrontEndServerWorkBuzzer> (*this)};
}

HandleShutDownAtFrontEndServerWork :: HandleShutDownAtFrontEndServerWork (string pathOfBackendIn) {
	pathOfBackend = pathOfBackendIn;
	wasFatalError = false;
}
	
void HandleShutDownAtFrontEndServerWork :: execute (PDBBuzzerPtr callerBuzzer) {

	// the error string
	string err;

	// get the shutdown message
	if (!wasFatalError) {
		if (getCommunicator ()->acceptShutDown (err, info)) {
			getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: Was an error getting shutdown message: " + err);
			err = "could not get storage change: " + err;
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: " + info);

	// get a communicator so that we can connect to the server
	PDBCommunicator childCommunicator;
	if (!wasFatalError) {
		if (childCommunicator.connectToLocalServer (getLogger (), pathOfBackend, err, getCommunicator ()->getBufferPool ())) {
			getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: problem communicating to backend server");
			getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: " + err);
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	// send shut down to the backend server
	if (!wasFatalError) {
		if (childCommunicator.sendShutDown (info)) {
			getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: problem sending shut down to backend");
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	// get the result
	if (!wasFatalError) {
		if (childCommunicator.acceptShutDownComplete (err, info)) {
			getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: error at backend: " + err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: got ack from server " + info);

	// send shut down complete
	if (!wasFatalError) {
		if (getCommunicator ()->sendShutDownComplete (info)) {
			getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: problem sending ack to client");
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	// sleep for a few seconds, just to give the ack a chance to be processed
	getLogger ()->writeLn ("HandleShutDownAtFrontEndServerWork: shut down complete");
	sleep (4);
	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif

