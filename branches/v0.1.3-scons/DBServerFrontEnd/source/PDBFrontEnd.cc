
#ifndef PDB_FRONTEND_CC
#define PDB_FRONTEND_CC

#include "HandleFeatureBuildingAtFrontEndServerWork.h"
#include "HandleDataAtFrontEndServerWork.h"
#include "HandleNewStorageAtFrontEndServerWork.h"
#include "HandleQueryAtFrontEndServerWork.h"
#include "HandleShutDownAtFrontEndServerWork.h"
#include <iostream>
#include "PDBFrontEnd.h"
#include <signal.h>
#include <stdio.h>

using namespace std;

void *nastyGlobalVariable;

// this signal is used as a way to let us know that the server backend has shut down,
// and so anyone who is waiting on the server needs to realize that they are never going
// to get a response
void handleBackendShutdown (int signal) {

	// by definition, this is what we get when the backened shuts down
	if (signal != SIGUSR1) {
		cout << "Why did I get a strange signal??\n";
		exit (1);
	}
	
	// this lets all of the workers know that the server backend shut down
	((PDBServer *) nastyGlobalVariable)->signal (PDBAlarm :: FatalServerError); 
}

PDBFrontEnd :: PDBFrontEnd (int portNumber, int numConnections, string IPCfile, int memPerConnection, string logFile) {

	// set up the logger
	PDBLoggerPtr tempLogger {make_shared <PDBLogger> (logFile)};

	// first we create a signal handler to deal with backend failures
	// Setup the signal handler
	struct sigaction sa;
    	sa.sa_handler = &handleBackendShutdown;
    	sa.sa_flags = 0;
 
	// block every signal during the handler
	sigfillset (&sa.sa_mask);
	
	// regsiter the event handler
	if (sigaction (SIGUSR1, &sa, NULL) != 0) {
		cout << "Bad!  I could not set up a signal hander for backend shutdowns.\n";
		perror ("Error was ");
	}

	tempLogger->writeLn ("Set up the signal handler for a dead backend.");

	// next, we set up the server
	PDBBufferPoolPtr tempPool {make_shared <PDBBufferPool> (numConnections, memPerConnection)};
	tempLogger->writeLn ("Set up the buffer pool.");

	externalServer = make_shared <PDBServer> (portNumber, numConnections, tempLogger, tempPool);
	nastyGlobalVariable = externalServer.get ();
	tempLogger->writeLn ("Set up the server.");

	// we now register a handler for each of the different request types that can come to the server

	// here's how to handle queries
	externalServer->registerHandler (PDBMsgType :: QueryMsg, 
		make_shared <HandleQueryAtFrontEndServerWork> (IPCfile));

	// here's how to handle data
	PDBStoredDataTypeDeserializationMachine tempDataMachine;
	PDBFeatureTypeDeserializationMachine tempDataMachineTwo;
	externalServer->registerHandler (PDBMsgType :: LoadDataMsg, 
		make_shared <HandleDataAtFrontEndServerWork> (IPCfile));

	// handle a server shut down request
	externalServer->registerHandler (PDBMsgType :: ShutDownMsg, 
		make_shared <HandleShutDownAtFrontEndServerWork> (IPCfile));

	// handle a change the storage location request
	externalServer->registerHandler (PDBMsgType :: StoreDataNewFileMsg,
		make_shared <HandleNewStorageAtFrontEndServerWork> (IPCfile));

	// handle a request to build some features
	externalServer->registerHandler (PDBMsgType :: BuildFeaturesMsg,
		make_shared <HandleFeatureBuildingAtFrontEndServerWork> (IPCfile));

	// all of the other handlers are going to be registered here!!
	// ...
	// ...

	// and start the server!  This will run forever
	externalServer->startServer (nullptr);
}

#endif

