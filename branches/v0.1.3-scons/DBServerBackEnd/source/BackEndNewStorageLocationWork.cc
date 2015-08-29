
#ifndef BACKEND_NEW_STORAGE_WORK_CC
#define BACKEND_NEW_STORAGE_WORK_CC

#include <string>
#include "BackEndNewStorageLocationWork.h"
using namespace std;

BackEndNewStorageLocationWork :: BackEndNewStorageLocationWork (SimplePDBPtr openMeIn) {
	openMe = openMeIn;	
}

PDBCommWorkPtr BackEndNewStorageLocationWork :: clone () {
        return make_shared <BackEndNewStorageLocationWork> (openMe);
}

void BackEndNewStorageLocationWork :: execute (PDBBuzzerPtr callerBuzzer) {
	
	string err, fName;
	if (getCommunicator ()->acceptChangeDataStorageLocation (err, fName)) {
		getLogger ()->writeLn ("BackEndNewStorageLocationWork: error reading the new storage location: " + err);
		err = "could not get new storage location (" + err + ")";
		getCommunicator ()->sendError (err);
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	getLogger ()->writeLn ("BackEndNewStorageLocationWork: Got new storage location " + fName);

	// change the location
	if (openMe->setStorage (fName, err)) {
		getLogger ()->writeLn ("BackEndNewStorageLocationWork: could not set storage location: " + err);
		err = "could not set new storage location: " + err;
		getCommunicator ()->sendError (err);
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	// and send back the results
	if (getCommunicator ()->sendDataStorageLocationChanged (fName)) {
		getLogger ()->writeLn ("BackEndNewStorageLocationWork: could not ack storage location change to client");
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	getLogger ()->writeLn ("BackEndNewStorageLocationWork: Processed new storage location " + fName);
	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif
