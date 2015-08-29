
#ifndef PDB_BACKEND_CC
#define PDB_BACKEND_CC

#include "BackEndAddDataWork.h"
#include "BackEndBuildFeaturesWork.h"
#include "BackEndNewStorageLocationWork.h"
#include "BackEndOpenFilesWork.h"
#include "BackEndQueryWork.h"
#include "BackEndShutDownWork.h"
#include "PDBBackEnd.h"
#include "PDBCommWork.h"
#include "PDBLogger.h"
#include "PDBMsgType.h"

PDBBackEnd :: PDBBackEnd (int numThreads, string IPCfile, int memPerConnection, string logFile, string catalogFile) {

	// next, we set up the server

	// the "+4" comes from the fact that a few extra threads will be running the server itself, and not really working
	PDBBufferPoolPtr tempPool = make_shared <PDBBufferPool> (numThreads + 4, memPerConnection);
	PDBLoggerPtr tempLogger = make_shared <PDBLogger> (logFile);
	PDBServerPtr backendServer = make_shared <PDBServer> (IPCfile, numThreads + 4, tempLogger, tempPool);

	// first, now up the database 
	SimplePDBPtr databasePtr = make_shared <SimplePDB> (numThreads, tempPool);
	string openErrorMessage;
	if (databasePtr->openDB (catalogFile, openErrorMessage)) {
		cout << "Error starting up the backend: " << openErrorMessage << "\n";
		exit (-1);
	}

	// and we now register a handler for each of the different request types that can come to the server

	// here's how to handle queries
	backendServer->registerHandler (PDBMsgType :: QueryMsg, {make_shared <BackEndQueryWork> (databasePtr)});

	// here's how to handle additional data
	backendServer->registerHandler (PDBMsgType :: LoadDataMsg, {make_shared <BackEndAddDataWork> (databasePtr)});

	// here's how to handle building features
	backendServer->registerHandler (PDBMsgType :: BuildFeaturesMsg, {make_shared <BackEndBuildFeaturesWork> (databasePtr)});
	
	// here's how to handle shutting down the server
	backendServer->registerHandler (PDBMsgType :: ShutDownMsg, {make_shared <BackEndShutDownWork> ()});
	
	// here's how to handle changing the storage location
	backendServer->registerHandler (PDBMsgType :: StoreDataNewFileMsg, {make_shared <BackEndNewStorageLocationWork> (databasePtr)});

	// and start the server!  This will run until a shutdown message
	PDBWorkPtr startupWork {make_shared <BackEndOpenFilesWork> (databasePtr)};	
	backendServer->startServer (startupWork);
	cout << "Done with backend server\n";
}

#endif

