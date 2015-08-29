
#ifndef HANDLE_NEW_STORAGE_FRONT_END_SERVER_WORK_C
#define HANDLE_NEW_STORAGE_FRONT_END_SERVER_WORK_C

#include "HandleNewStorageAtFrontEndServerWork.h"
#include "HandleNewStorageAtFrontEndServerWorkBuzzer.h"
#include "PDBCommWork.h"
#include "SendErrorToClientWork.h"

void HandleNewStorageAtFrontEndServerWork :: wasAFatalServerError () {

	// this is going to be called when there was some sort of problem sending/receiving 
	wasFatalError = true;
	getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: Got a server error!\n");

	// send an error message to the client
	PDBWorkerPtr myWorker = getWorker ();
	PDBWorkPtr errorWork {make_shared <SendErrorToClientWork> (getCommunicator (), "Error at the backend server: " + fName)};
	myWorker->execute (errorWork, errorWork->getLinkedBuzzer ());
}

PDBCommWorkPtr HandleNewStorageAtFrontEndServerWork :: clone () {
	return {make_shared <HandleNewStorageAtFrontEndServerWork> (pathOfBackend)};	
}

PDBBuzzerPtr HandleNewStorageAtFrontEndServerWork :: getLinkedBuzzer () {
	return {make_shared <HandleNewStorageAtFrontEndServerWorkBuzzer> (*this)};
}

HandleNewStorageAtFrontEndServerWork :: HandleNewStorageAtFrontEndServerWork (string pathOfBackendIn) {
	pathOfBackend = pathOfBackendIn;
	wasFatalError = false;
}
	
void HandleNewStorageAtFrontEndServerWork :: execute (PDBBuzzerPtr buzzMeWhenDoneIn) {

	string err, info;

	if (!wasFatalError) {
		if (getCommunicator ()->acceptChangeDataStorageLocation (err, fName)) {
			getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: Was an error getting storage change: " + err);
			err = "could not get storage change: " + err;
			getCommunicator ()->sendError (err);
			buzzMeWhenDoneIn->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: Got new storage location: " + fName);

	// get a communicator to connect to the server, and then connect
	PDBCommunicator childCommunicator;
	if (!wasFatalError) {
		if (childCommunicator.connectToLocalServer (getLogger (), pathOfBackend, err, getCommunicator ()->getBufferPool ())) {
			getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: problem communicating to backend server");
			getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: " + err);
			getCommunicator ()->sendError (err);
			buzzMeWhenDoneIn->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	// send the change result to the server
	if (!wasFatalError) {
		if (childCommunicator.sendChangeDataStorageLocation (fName)) {
			getLogger ()->writeLn ("SendNewStorageLocationToServerWork: problem sending new storage location to backend");
			err = "could not send data to server";
			buzzMeWhenDoneIn->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: Sent new storage location to server: " + fName);

	// get the result
	if (!wasFatalError) {
		if (childCommunicator.acceptChangeDataStorageLocationDone (err, info)) {
			getLogger ()->writeLn ("SendNewStorageLocationToServerWork: error at backend: " + err);
			buzzMeWhenDoneIn->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	// finally, we send back the result...
	if (!wasFatalError) {
		if (getCommunicator ()->sendDataStorageLocationChanged (info)) { 
			getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: could not send ack back to client");
			buzzMeWhenDoneIn->buzz (PDBAlarm :: GenericError);
			return;	
		}
	}

	getLogger ()->writeLn ("HandleNewStorageAtFrontEndServerWork: Sent ack to client. Info was " + info);
	buzzMeWhenDoneIn->buzz (PDBAlarm :: WorkAllDone);
}

#endif

