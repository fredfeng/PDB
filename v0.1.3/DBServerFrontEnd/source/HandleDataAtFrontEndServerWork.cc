
#ifndef HANDLE_DATA_FRONT_END_SERVER_WORK_C
#define HANDLE_DATA_FRONT_END_SERVER_WORK_C

#include "HandleDataAtFrontEndServerWork.h"
#include "HandleDataAtFrontEndServerWorkBuzzer.h"
#include "PDBCommWork.h"
#include "SendErrorToClientWork.h"

void HandleDataAtFrontEndServerWork :: wasAFatalServerError () {

	// this is going to be called when the worker was signaled with a catastrophic 
	// error on the server... 
	wasFatalError = true;
	getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: Got a server error!!\n");

	// send an error message to the client
	PDBWorkerPtr myWorker = getWorker ();
	PDBWorkPtr errorWork {make_shared <SendErrorToClientWork> (getCommunicator (), "Error at the backend server: " + info)};
	myWorker->execute (errorWork, errorWork->getLinkedBuzzer ());
}

PDBCommWorkPtr HandleDataAtFrontEndServerWork :: clone () {
	return {make_shared <HandleDataAtFrontEndServerWork> (pathOfBackend)};	
}

PDBBuzzerPtr HandleDataAtFrontEndServerWork :: getLinkedBuzzer () {
	return {make_shared <HandleDataAtFrontEndServerWorkBuzzer> (*this)};
}

HandleDataAtFrontEndServerWork :: HandleDataAtFrontEndServerWork (string pathOfBackendIn) {
	pathOfBackend = pathOfBackendIn;
	wasFatalError = false;
}
	
void HandleDataAtFrontEndServerWork :: execute (PDBBuzzerPtr callerBuzzer) {

	string err;
	vector <PDBData> dataToLoad;

	// these guys will be empty; hence, they won't actually deserialize; they will just create flat data
	PDBStoredDataTypeDeserializationMachine deserUsingMe;
	PDBFeatureTypeDeserializationMachine meTo;

	if (!wasFatalError) {
		if (getCommunicator ()->acceptLoadData (dataToLoad, err, info, deserUsingMe, meTo)) {
			getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: Was an error getting data: " + err);
			err = "could not get data loading message (" + err + ")";
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: Got data " + info);

	// get a communicator who will go to the server
	PDBCommunicator childCommunicator;
	if (!wasFatalError) {
		if (childCommunicator.connectToLocalServer (getLogger (), pathOfBackend, err, getCommunicator ()->getBufferPool ())) {
			getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: problem communicating to backend server");
			getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: " + err);
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	// send the data to the server
	if (!wasFatalError) {
		if (childCommunicator.sendLoadData (dataToLoad, info)) {
			getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: problem sending data to backend " + info);
			err = "could not send data to server";
			callerBuzzer->buzz (PDBAlarm :: ServerError);
			return;
		}
	}

	// get the ack from the server
	if (!wasFatalError) {
		if (childCommunicator.acceptDataLoadingTaskAllDone (err, info)) {
			getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: error at backend: " + err);
			callerBuzzer->buzz (PDBAlarm :: ServerError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: got result from server; info was " + info);

	// finally, we send back the result...
	if (getCommunicator ()->sendDataLoadingTaskAllDone (info)) { 
		getLogger ()->writeLn ("HandleDataAtFrontEndServerWork: could not send ack back to client");
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;	
	}

	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif

