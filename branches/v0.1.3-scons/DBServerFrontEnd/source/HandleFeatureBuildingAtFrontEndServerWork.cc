
#ifndef HANDLE_FEATURE_BUILDING_FRONT_END_SERVER_WORK_C
#define HANDLE_FEATURE_BUILDING_FRONT_END_SERVER_WORK_C

#include "HandleFeatureBuildingAtFrontEndServerWork.h"
#include "HandleFeatureBuildingAtFrontEndServerWorkBuzzer.h"
#include "PDBCommWork.h"
#include "SendErrorToClientWork.h"

void HandleFeatureBuildingAtFrontEndServerWork :: wasAFatalServerError () {

	// this is going to be called when the worker was signaled with a catastrophic 
	// error on the server... 
	wasFatalError = true;
	getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: Got a server error!!\n");

	// send an error message to the client
	PDBWorkerPtr myWorker = getWorker ();
	PDBWorkPtr errorWork {make_shared <SendErrorToClientWork> (getCommunicator (), "Error at the backend server: " + info)};
	myWorker->execute (errorWork, errorWork->getLinkedBuzzer ());
}

PDBCommWorkPtr HandleFeatureBuildingAtFrontEndServerWork :: clone () {
	return {make_shared <HandleFeatureBuildingAtFrontEndServerWork> (pathOfBackend)};	
}

PDBBuzzerPtr HandleFeatureBuildingAtFrontEndServerWork :: getLinkedBuzzer () {
	return {make_shared <HandleFeatureBuildingAtFrontEndServerWorkBuzzer> (*this)};
}

HandleFeatureBuildingAtFrontEndServerWork :: HandleFeatureBuildingAtFrontEndServerWork (string pathOfBackendIn) {
	pathOfBackend = pathOfBackendIn;
	wasFatalError = false;
}
	
void HandleFeatureBuildingAtFrontEndServerWork :: execute (PDBBuzzerPtr callerBuzzer) {

	string err;
	PDBFeatureListBuilder executeMe;

	// this will be empty; hence, he won't actually deserialize; he will just create flat data
	PDBFeatureBuilderDeserializationMachine deserUsingMe;

	getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: about to get the task");

	if (!wasFatalError) {
		if (getCommunicator ()->acceptFeatureBuildingTask (executeMe, err, info, deserUsingMe)) {
			getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: Was an error getting data: " + err);
			err = "could not get feature building message (" + err + ")";
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: Got building task " + info);

	// get a communicator who will go to the server
	PDBCommunicator childCommunicator;
	if (!wasFatalError) {
		if (childCommunicator.connectToLocalServer (getLogger (), pathOfBackend, err, getCommunicator ()->getBufferPool ())) {
			getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: problem communicating to backend server");
			getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: " + err);
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

	// send the data to the server
	if (!wasFatalError) {
		if (childCommunicator.sendFeatureBuildingTask (executeMe, info)) {
			getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: problem sending work to backend " + info);
			err = "could not send work to server";
			callerBuzzer->buzz (PDBAlarm :: ServerError);
			return;
		}
	}

	// get the ack from the server
	if (!wasFatalError) {
		if (childCommunicator.acceptFeatureBuildingTaskAllDone (err, info)) {
			getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: error at backend: " + err);
			callerBuzzer->buzz (PDBAlarm :: ServerError);
			return;
		}
	}

	getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: got result from server; info was " + info);

	// finally, we send back the result...
	if (getCommunicator ()->sendFeatureBuildingTaskAllDone (info)) { 
		getLogger ()->writeLn ("HandleFeatureBuildingAtFrontEndServerWork: could not send ack back to client");
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;	
	}

	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif

