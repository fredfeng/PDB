
#ifndef HANDLE_QUERY_FRONT_END_SERVER_WORK_C
#define HANDLE_QUERY_FRONT_END_SERVER_WORK_C

#include "HandleQueryAtFrontEndServerWork.h"
#include "HandleQueryAtFrontEndServerWorkBuzzer.h"
#include "PDBCommWork.h"
#include "SendErrorToClientWork.h"

void HandleQueryAtFrontEndServerWork :: wasAFatalServerError () {

	// this is going to be called when the worker was signaled with a catastrophic 
	// error on the server... 
	wasFatalError = true;
	getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: Got a server error!!\n");

	// send an error message to the client
	PDBWorkerPtr myWorker = getWorker ();
	PDBWorkPtr errorWork {make_shared <SendErrorToClientWork> (getCommunicator (), "Error at the backend server: " + info)};
	myWorker->execute (errorWork, errorWork->getLinkedBuzzer ());
}

PDBCommWorkPtr HandleQueryAtFrontEndServerWork :: clone () {
	return {make_shared <HandleQueryAtFrontEndServerWork> (pathOfBackend)};
}

PDBBuzzerPtr HandleQueryAtFrontEndServerWork :: getLinkedBuzzer () {
	return {make_shared <HandleQueryAtFrontEndServerWorkBuzzer> (*this)};	
}

HandleQueryAtFrontEndServerWork :: HandleQueryAtFrontEndServerWork (string pathOfBackendIn) {
	pathOfBackend = pathOfBackendIn;
	wasFatalError = false;
}
	
void HandleQueryAtFrontEndServerWork :: execute (PDBBuzzerPtr callerBuzzer) {

	// the error string
	string err;

	// the query to process
	PDBQueryExecutorPtr myQuery;

	// get the query
	PDBQueryExecutorDeserializationMachine deserUsingMe;
	if (!wasFatalError) {
		if (getCommunicator ()->acceptQuery (myQuery, err, info, deserUsingMe)) {
			getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: Was an error getting query: " + err);
			err = "could not get query message: " + err;
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	} 

	getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: Got query " + info);

	// get a communicator so that we can connect to the server
	PDBCommunicator childCommunicator;
	if (!wasFatalError) {
		if (childCommunicator.connectToLocalServer (getLogger (), pathOfBackend, err, getCommunicator ()->getBufferPool ())) {
			getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: problem communicating to backend server");
			getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: " + err);
			getCommunicator ()->sendError (err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;
		}
	}

        // send query to the backend server
        if (!wasFatalError) {
		if (childCommunicator.sendQuery (myQuery, info)) {
			getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: problem sending query to backend " + info);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			getCommunicator ()->sendError ("error when trying to send query to backend");
			return;
		}
        }

        // get the result
        if (!wasFatalError) {
		if (childCommunicator.acceptQueryResult (myQuery, err, info, deserUsingMe)) {
			getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: error at backend: " + err);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			getCommunicator ()->sendError ("could not get query result " + err);
			return;
		}
        }

	getLogger ()->writeLn ("SendQueryToServerWork: got result from server " + info);

	// finally, we send back the result...
        if (!wasFatalError) {
		if (getCommunicator ()->sendQueryResult (myQuery, info)) { 
			getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: could not send query result back to client " + info);
			callerBuzzer->buzz (PDBAlarm :: GenericError);
			return;	
		}
	}

	getLogger ()->writeLn ("HandleQueryAtFrontEndServerWork: done sending result to client " + info);
	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif

