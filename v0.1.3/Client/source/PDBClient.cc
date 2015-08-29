
#ifndef PDB_CLIENT_C
#define PDB_CLIENT_C

#include "PDBBufferPool.h"
#include "PDBClient.h"
#include "PDBLogger.h"
#include <signal.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

void PDBClient :: connectToServer (string serverAddress, int portNumber, 
	bool &wasError, string &errMsg, size_t bufferMem, string logFile) {

	// used to count up the messages
	counter = 0;

	// create the logger
	myLoggerPtr = make_shared <PDBLogger> (logFile);

	// get a buffer pool... we choose 5 arbitrarily... since this is not multi-threaded, we won't need
	// many pages
	PDBBufferPoolPtr tempPool {make_shared <PDBBufferPool> (5, bufferMem)}; 
	if (myCommunicator.connectToInternetServer (myLoggerPtr, portNumber, serverAddress, errMsg, tempPool)) {
		myLoggerPtr->writeLn ("PDBClient: " + errMsg);
		wasError = true;
		return;
	}

	// this is here so that we will recover from a socket closing unexpectedly on us
	signal (SIGPIPE, SIG_IGN);

	// no error yet!!
	wasError = false;
}

PDBCatalog &PDBClient :: getCatalog () {
	return catalog;
}

void PDBClient :: buildFeatures (PDBFeatureListBuilder &runMe, bool &wasError, string &errMsg) {
	
	// send the query to the server
	char buffer[100];
	sprintf (buffer, "op #%d", counter++);
	string info (buffer);
	myLoggerPtr->writeLn ("PDBClient: sending builder to the server");
	if (myCommunicator.sendFeatureBuildingTask (runMe, info)) {	
		errMsg = "Could not send feature builder to server.";
		wasError = true;
		return;
	}

	myLoggerPtr->writeLn ("PDBClient: sent feature builder to server");

	// and get the result back...
	if (myCommunicator.acceptFeatureBuildingTaskAllDone (errMsg, info)) {
		wasError = true;
		myLoggerPtr->writeLn ("PDBClient: " + errMsg);
		return;
	} 

	myLoggerPtr->writeLn ("PDBClient: got back query; info was " + info);

	wasError = false;
	
}

PDBQueryExecutorPtr PDBClient :: runQuery (PDBQueryExecutorPtr runMe, bool &wasError, string &errMsg) {

	// send the query to the server
	char buffer[100];
	sprintf (buffer, "op #%d", counter++);
	string info (buffer);
	myLoggerPtr->writeLn ("PDBClient: sending query to server");
	if (myCommunicator.sendQuery (runMe, info)) {	
		errMsg = "Could not send query to server.";
		wasError = true;
		return nullptr;
	}

	myLoggerPtr->writeLn ("PDBClient: sent query to server");
	PDBQueryExecutorPtr returnVal;
	
	// and get the result back... we first need some way to deserialize the result
	PDBQueryExecutorDeserializationMachine deserUsingMe;
	deserUsingMe.registerNewQueryExecutor (runMe);
	if (myCommunicator.acceptQueryResult (returnVal, errMsg, info, deserUsingMe)) {
		wasError = true;
		myLoggerPtr->writeLn ("PDBClient: " + errMsg);
		return nullptr;
	} 

	myLoggerPtr->writeLn ("PDBClient: got back query; info was " + info);

	wasError = false;
	return returnVal;
}

void PDBClient :: addData (PDBData &dataToStore, bool &wasError, string &errMsg) {

	vector <PDBData> dataToStoreList;
	dataToStoreList.push_back (dataToStore);
	addData (dataToStoreList, wasError, errMsg);
}

void PDBClient :: addData (vector <PDBData> &dataToStore, bool &wasError, string &errMsg) {

	// send the data to the server
	char buffer[100];
	sprintf (buffer, "op #%d", counter++);
	string info (buffer);
	myLoggerPtr->writeLn ("PDBClient: sending data to server");

	// send the data over
	if (myCommunicator.sendLoadData (dataToStore, info)) {
		errMsg = "Could not send data to server.";
		myLoggerPtr->writeLn ("PDBClient: error " + errMsg);
		wasError = true;
		return;
	}

	// wait until the ack comes back
	myLoggerPtr->writeLn ("PDBClient: done sending data to server");
	if (myCommunicator.acceptDataLoadingTaskAllDone (errMsg, info)) {
		wasError = true;
		myLoggerPtr->writeLn ("PDBClient: error " + errMsg);
		return;
	} 

	myLoggerPtr->writeLn ("PDBClient: got data sending ack from server; info was " + info);
	wasError = false;
	return;
}

void PDBClient :: shutDownServer (bool &wasError, string &errMsg) {

	char buffer[100];
	sprintf (buffer, "op #%d", counter++);
	string info (buffer);

	// send the shut down message over
	if (myCommunicator.sendShutDown (info)) {
		errMsg = "Could not send shut down message to server.";
		myLoggerPtr->writeLn ("PDBClient: error " + errMsg);
		wasError = true;
		return;
	}

	// wait until the ack comes back
	if (myCommunicator.acceptShutDownComplete (errMsg, info)) {
		wasError = true;
		myLoggerPtr->writeLn ("PDBClient: error " + errMsg);
		return;
	} 

	myLoggerPtr->writeLn ("PDBClient: got shut down ack from server; info was " + info);
	wasError = false;
	return;
}

void PDBClient :: changeStorageLocation (string fName, bool &wasError, string &errMsg) {

	myLoggerPtr->writeLn ("PDBClient: sending new storage location to server: " + fName);

	if (myCommunicator.sendChangeDataStorageLocation (fName)) {
		errMsg = "Could not send change storage message to server.";
		myLoggerPtr->writeLn ("PDBClient: error " + errMsg);
		wasError = true;
		return;
	}

	string info;
	if (myCommunicator.acceptChangeDataStorageLocationDone (errMsg, info)) {
		myLoggerPtr->writeLn ("PDBClient: error " + errMsg);
		wasError = true;
		return;
	} 

	myLoggerPtr->writeLn ("PDBClient: done sending new storage location to server; info was " + info);
	wasError = false;
}

#endif
