
#ifndef PDB_COMMUN_C
#define PDB_COMMUN_C

#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include "PDBCommunicator.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/uio.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

PDBCommunicator :: PDBCommunicator () {
	needToSendDisconnectMsg = false;
	readCurMsgType = false;	
	socketFD = -1;
}

PDBBufferPoolPtr PDBCommunicator :: getBufferPool () {
	return myPool;
}

bool PDBCommunicator :: pointToInternet (PDBLoggerPtr logToMeIn, int socketFDIn, string& errMsg, PDBBufferPoolPtr myPoolIn) {

	// first, connect to the backend
	logToMe = logToMeIn;

	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof (cli_addr);
	logToMe->writeLn ("PDBCommunicator: about to wait for request from Internet");
	socketFD = accept (socketFDIn, (struct sockaddr *) &cli_addr, &clilen);
	if (socketFD < 0) {
		logToMe->writeLn ("PDBCommunicator: could not get FD to internet socket");
		logToMe->writeLn (strerror (errno));
		errMsg = "Could not get socket ";
		errMsg += strerror (errno);
		return true;
	}
	
	// remember the communication buffer pool
	myPool = myPoolIn;

	logToMe->writeLn ("PDBCommunicator: got request from Internet");
	return false;
}

bool PDBCommunicator :: connectToInternetServer (PDBLoggerPtr logToMeIn, int portNumber, string serverAddress, 
	string &errMsg, PDBBufferPoolPtr myPoolIn) { 

	logToMe = logToMeIn;
	myPool = myPoolIn;

        // set up the socket
        struct sockaddr_in serv_addr;
        struct hostent *server;

        socketFD = socket (AF_INET, SOCK_STREAM, 0);
        if (socketFD < 0) {
		logToMe->writeLn ("PDBCommunicator: could not get FD to internet socket");
		logToMe->writeLn (strerror (errno));
		errMsg = "Could not get socket to backend ";
		errMsg += strerror (errno);
		return true;
        }

	logToMe->writeLn ("PDBCommunicator: Got internet socket");
	logToMe->writeLn ("PDBCommunicator: About to check the database for the host name");

        server = gethostbyname (serverAddress.c_str ());
        if (server == nullptr) {
		logToMe->writeLn ("PDBCommunicator: could not get host by name");
		logToMe->writeLn (strerror (errno));
		errMsg = "Could not get host by name ";
		errMsg += strerror (errno);
		return true;
        }

	logToMe->writeLn ("PDBCommunicator: About to connect to the remote host");

        bzero ((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy ((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons (portNumber);
        if (:: connect (socketFD, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
		logToMe->writeLn ("PDBCommunicator: could not get host by name");
		logToMe->writeLn (strerror (errno));
		errMsg = "Could not connect to server ";
		errMsg += strerror (errno);
		return true;
        }

	// note that we need to close this up when we are done
	needToSendDisconnectMsg = true;

	logToMe->writeLn ("PDBCommunicator: Successfully connected to the remote host");
	return false;
}

bool PDBCommunicator :: connectToLocalServer (PDBLoggerPtr logToMeIn, string fName, string& errMsg, PDBBufferPoolPtr myPoolIn) {

	logToMe = logToMeIn;
	myPool = myPoolIn;

	struct sockaddr_un server;
	socketFD = socket (AF_UNIX, SOCK_STREAM, 0);
	if (socketFD < 0) {
		logToMe->writeLn ("PDBCommunicator: could not get FD to local server socket");
		logToMe->writeLn (strerror (errno));
		errMsg = "Could not get FD to local server socket ";
		errMsg += strerror (errno);
		return true;
	}
	
	server.sun_family = AF_UNIX;
	strcpy (server.sun_path, fName.c_str ());
	if (:: connect (socketFD, (struct sockaddr *) &server, sizeof (struct sockaddr_un)) < 0) {
		logToMe->writeLn ("PDBCommunicator: could not connect to local server socket");
		logToMe->writeLn (strerror (errno));
		errMsg = "Could not connect to local server socket ";
		errMsg += strerror (errno);
		return true;
	}

	// note that we need to close this up when we are done
	needToSendDisconnectMsg = true;

	return false;
}

bool PDBCommunicator :: pointToFile (PDBLoggerPtr logToMeIn, int socketFDIn, string& errMsg, PDBBufferPoolPtr myPoolIn) {

	// connect to the backend
	logToMe = logToMeIn;

	logToMe->writeLn ("PDBCommunicator: about to wait for request from same machine");
	socketFD = accept (socketFDIn, 0, 0);
	if (socketFD < 0) {
		logToMe->writeLn ("PDBCommunicator: could not get FD to local socket");
		logToMe->writeLn (strerror (errno));
		errMsg = "Could not get socket ";
		errMsg += strerror (errno);
		return true;
	}

	logToMe->writeLn ("PDBCommunicator: got request from same machine");

	// remember the communication buffer pool
	myPool = myPoolIn;

	return false;
}

PDBCommunicator :: ~PDBCommunicator () {

	// tell the server that we are disconnecting (note that needToSendDisconnectMsg is
	// set to true only if we are a client and we want to close a connection to the server
	if (needToSendDisconnectMsg && socketFD > 0) {
		logToMe->writeLn ("PDBCommunicator: closing connection to the server");
		sendCloseConnection ("Client is closing connection");
	}

	if (socketFD >= 0) {
		close (socketFD);
	}
}

PDBMsgType PDBCommunicator :: getTypeOfNextMsg () {

	// read in the next message
	char buffer[sizeof (PDBMsgType) + sizeof (size_t)];
	int size = sizeof (PDBMsgType) + sizeof (size_t);

	// make sure we got enough bytes... if we did not, then error out
	if (read (socketFD, buffer, size) < size) {
		logToMe->writeLn ("PDBCommunicator: could not read next message type");
		logToMe->writeLn (strerror (errno));
		curMsgType = PDBMsgType :: NoMsg;
		msgSize = 0;

	// OK, we did get enough bytes
	} else {
		curMsgType = *((PDBMsgType *) buffer);
		msgSize = *((size_t *) (sizeof (PDBMsgType) + buffer));
	}
	readCurMsgType = true;	
	return curMsgType;
}

bool PDBCommunicator :: doTheWrite (char *end, char *start, string errMsgIn) {

	// and do the write
	while (end != start) {	
		
		// write some bytes
		ssize_t numBytes = write (socketFD, start, end - start);

		// make sure they went through
		if (numBytes <= 0) {
			logToMe->writeLn ("PDBCommunicator: " + errMsgIn);
			logToMe->writeLn (strerror (errno));
			return true;
		}
		
		start += numBytes;
	}

	return false;
}

// message format:
// 1) PDBMsgType :: ErrMsg or PDBMsgType :: InfoMsg
// 2) total message length in bytes
// 3) string
// 
bool PDBCommunicator :: sendText (PDBMsgType msgType, string text) {

	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// write the message type
	char *start, *cur;
	start = cur = myMem->getRaw ();
	*((PDBMsgType *) cur) = msgType;
	cur += sizeof (PDBMsgType);

	// reserve some space to write the message length
	cur += sizeof (size_t);

	// write the c-string
	memcpy (cur, text.c_str (), text.length () + 1);
	cur += strlen (text.c_str ()) + 1;

	// record the length
	*((size_t *) (start + sizeof (PDBMsgType))) = cur - start - sizeof (PDBMsgType) - sizeof (size_t);
	
	return doTheWrite (cur, start, "could not write text message: " + text);
}

bool PDBCommunicator :: sendDataStorageLocationChanged (string info) {
	return sendText (PDBMsgType :: StoreDataNewFileDoneMsg, info);
}

bool PDBCommunicator :: sendChangeDataStorageLocation (string fName) {
	return sendText (PDBMsgType :: StoreDataNewFileMsg, fName);
}

bool PDBCommunicator :: sendShutDown (string info) {
	return sendText (PDBMsgType :: ShutDownMsg, info);
}

bool PDBCommunicator :: sendShutDownComplete (string info) {
	return sendText (PDBMsgType :: ShutDownDoneMsg, info);
}

bool PDBCommunicator :: sendError (string err) {
	logToMe->writeLn ("PDBCommunicator: sending error: " + err);
	return sendText (PDBMsgType :: ErrMsg, err);
}

bool PDBCommunicator :: sendCloseConnection (string info) {
	return sendText (PDBMsgType :: CloseConnection, info);
}

bool PDBCommunicator :: sendInfo (string info) {
	return sendText (PDBMsgType :: InfoMsg, info);
}

bool PDBCommunicator :: sendDataLoadingTaskAllDone (string info) {
	return sendText (PDBMsgType :: LoadDataDoneMsg, info);
}

bool PDBCommunicator :: sendFeatureBuildingTaskAllDone (string info) {
	return sendText (PDBMsgType :: FeatureBuildTaskDoneMsg, info);
}

// message format:
// 1) PDBMsgType :: BuildFeaturesMsg
// 2) total message length in bytes
// 3) info string
// 4) size of the task
bool PDBCommunicator :: sendFeatureBuildingTask (PDBFeatureListBuilder &executeMe, string info) {

	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// write the message type
	char *start, *cur;
	cur = start = myMem->getRaw ();
	*((PDBMsgType *) cur) = PDBMsgType :: BuildFeaturesMsg;
	cur += sizeof (PDBMsgType);

	// reserve some space to write the message length
	cur += sizeof (size_t);

	// write the c-string for the info
	memcpy (cur, info.c_str (), info.length () + 1);
	cur += info.length () + 1;

	size_t size;
	cur = (char *) executeMe.serialize (cur, size);

	// record the length
	*((size_t *) (start + sizeof (PDBMsgType))) = cur - start - sizeof (PDBMsgType) - sizeof (size_t);
	return doTheWrite (cur, start, "could not write send query message");
}

// message format:
// 1) PDBMsgType :: QueryMsg --or-- PDBMsgType :: QueryResultMsg
// 2) total message length in bytes
// 3) info string length
// 4) info string
// 6) size of query result
// 7) query result
// 
bool PDBCommunicator :: sendQuery (PDBQueryExecutorPtr executeMe, string info, PDBMsgType msgType) {

	// make sure the caller did not screw up
	if (msgType != PDBMsgType :: QueryMsg && msgType != PDBMsgType :: QueryResultMsg) {
		cout << "Why are you sending a bad type using PDBCommunicator :: sendQuery?";
		exit (-1);
	}
	
	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// write the message type
	char *start, *cur;
	cur = start = myMem->getRaw ();
	*((PDBMsgType *) cur) = msgType;
	cur += sizeof (PDBMsgType);

	// reserve some space to write the message length
	cur += sizeof (size_t);

	// write the c-string for the info
	memcpy (cur, info.c_str (), info.length () + 1);
	cur += info.length () + 1;

	// write out the query
	// begin with the code
	*((int *) (cur)) = executeMe->getCode ();
	cur += sizeof (int);	
	
	size_t size;
	cur = (char *) executeMe->serialize (cur, size);

	// record the length
	*((size_t *) (start + sizeof (PDBMsgType))) = cur - start - sizeof (PDBMsgType) - sizeof (size_t);
	
	return doTheWrite (cur, start, "could not write send query message");
}
	
bool PDBCommunicator :: sendQuery (PDBQueryExecutorPtr queryResult, string info) { 
	return sendQuery (queryResult, info, PDBMsgType :: QueryMsg);
}

bool PDBCommunicator :: sendQueryResult (PDBQueryExecutorPtr queryResult, string info) {
	return sendQuery (queryResult, info, PDBMsgType :: QueryResultMsg);
}		

bool PDBCommunicator :: acceptError (string &errorMessage) {
	PDBMsgType temp;
	string info;
	temp = PDBMsgType :: ErrMsg;
	return acceptText (temp, errorMessage, info);
}

bool PDBCommunicator :: acceptCloseConnection (string &errMsg, string &info) {
	PDBMsgType temp;
	temp = PDBMsgType :: CloseConnection;
	return acceptText (temp, errMsg, info);
}

bool PDBCommunicator :: acceptDataLoadingTaskAllDone (string &errMsg, string &info) {
	PDBMsgType temp;
	temp = PDBMsgType :: LoadDataDoneMsg;
	return acceptText (temp, errMsg, info);
}

bool PDBCommunicator :: acceptInfo (string &errMsg, string &info) {
	PDBMsgType temp;
	temp = PDBMsgType :: InfoMsg;
	return acceptText (temp, errMsg, info);
}

bool PDBCommunicator :: acceptShutDown (string &errMsg, string &info) {
	PDBMsgType temp;
	temp = PDBMsgType :: ShutDownMsg;
	return acceptText (temp, errMsg, info);
}

bool PDBCommunicator :: acceptFeatureBuildingTaskAllDone (string &errMsg, string &info) {
	PDBMsgType temp;
	temp = PDBMsgType :: FeatureBuildTaskDoneMsg;
	return acceptText (temp, errMsg, info);
}

bool PDBCommunicator :: acceptShutDownComplete (string &errMsg, string &info) {
	PDBMsgType temp;
	temp = PDBMsgType :: ShutDownDoneMsg;
	return acceptText (temp, errMsg, info);
}

bool PDBCommunicator :: acceptChangeDataStorageLocation (string &errMsg, string &fName) {
	logToMe->writeLn ("PDBCommunicator: got request to chage storage location");
	PDBMsgType temp;
	temp = PDBMsgType :: StoreDataNewFileMsg;
	return acceptText (temp, errMsg, fName);
}

bool PDBCommunicator :: acceptChangeDataStorageLocationDone (string &errMsg, string &info) {
	PDBMsgType temp;
	temp = PDBMsgType :: StoreDataNewFileDoneMsg;
	return acceptText (temp, errMsg, info);
}

bool PDBCommunicator :: acceptText (PDBMsgType &msgTypeIn, string &err, string &text) {

	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// get the message type
	if (!readCurMsgType) {
		getTypeOfNextMsg ();				
	}
	readCurMsgType = false;
	
	// loop and read the message
	char *start = myMem->getRaw ();
	char *cur = start;
	while (cur - start < (long) msgSize) {

		ssize_t numBytes = read (socketFD, cur, msgSize - (cur - start));
		if (numBytes <= 0) {
			logToMe->writeLn ("PDBCommunicator: error reading socket when trying to accept text message");
			logToMe->writeLn (strerror (errno));
			return true;
		}
		cur += numBytes;
	}

	// read in the info string
	string infoBack (start);
	cur += infoBack.length () + 1;	

	// make sure the length was good
	if (infoBack.length () + 1 != msgSize) {
		logToMe->writeLn ("PDBCommunicator: text message length does not match total message length");
		return true;
	}
	
	if (curMsgType == PDBMsgType :: ErrMsg) {
		msgTypeIn = PDBMsgType :: ErrMsg;
		err = infoBack;
		logToMe->writeLn ("PDBCommunicator: Got an error message");
		return true;
	} else if (curMsgType != msgTypeIn) {
		cout << "PDBCommunicator: error, expected message type " << msgTypeIn << " got " << curMsgType << "\n";
		exit (-1);
	}

	// everything was OK!
	text = infoBack;	
	return false;
}

bool PDBCommunicator :: acceptFeatureBuildingTask (PDBFeatureListBuilder &executeMe, string &errMsg, string &info,
	PDBFeatureBuilderDeserializationMachine &deserUsingMe) {

	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// get the message type
	if (!readCurMsgType) {
		getTypeOfNextMsg ();				
	}
	readCurMsgType = false;

	// make sure the type is acceptable
	if (curMsgType != PDBMsgType :: BuildFeaturesMsg) {
		logToMe->writeLn ("PDBCommunicator: Trying to accept a feature building task, but didn't get BuildFeaturesMsg");
		return true;
	}

	// loop and read the message
	char *start = myMem->getRaw ();
	char *cur = start;
	while (cur - start < (long) msgSize) {
		ssize_t numBytes = read (socketFD, cur, msgSize - (cur - start));
		if (numBytes <= 0) {
			logToMe->writeLn ("PDBCommunicator: error reading socket when trying to accept feature building task");
			logToMe->writeLn (strerror (errno));
			return true;
		}
		cur += numBytes;
	}

	// read in the info string
	string infoBack (start);
	info = infoBack;
	start += info.length () + 1;	

	// and read in the task itself---get the number of bytes to deserialize
	size_t byteCount = msgSize - (info.length () + 1);

	// send that bytecount so that we can use the generic deserializer
	cur = (char *) executeMe.deSerialize (deserUsingMe, start, byteCount);

	if (byteCount != msgSize - (info.length () + 1)) {
		logToMe->writeLn ("PDBCommunicator: Feature builder bytes deserialized is not what was expected!!");
		errMsg = "Feature builder bytes deserialized was wrong";
		return true;
	}	

	// everything was OK!
	return false;
}

bool PDBCommunicator :: acceptQuery (PDBQueryExecutorPtr &queryToRead, string &errMsg, string &info, PDBMsgType msgType,
	 PDBQueryExecutorDeserializationMachine &machine) {

	// make sure the caller did not screw up
	if (msgType != PDBMsgType :: QueryMsg && msgType != PDBMsgType :: QueryResultMsg) {
		cout << "Why are you sending a bad type using PDBCommunicator :: acceptQuery?";
		exit (-1);
	}

	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// get the message type
	if (!readCurMsgType) {
		getTypeOfNextMsg ();				
	}

	// see if we got an error message
	if (curMsgType == PDBMsgType :: ErrMsg) {
		logToMe->writeLn ("PDBCommunicator: Got error back from query");	
		return acceptError (errMsg);
	}

	// put this down here, since if we accept an error (above) we don't need to re-read the message type
	readCurMsgType = false;

	// make sure the type is acceptable
	if (msgType != curMsgType) {
		logToMe->writeLn ("PDBCommunicator: Trying to accept a query, but message type was not QueryMsg or QueryResultMsg");
		return true;
	}

	// loop and read the message
	char *start = myMem->getRaw ();
	char *cur = start;
	while (cur - start < (long) msgSize) {
		ssize_t numBytes = read (socketFD, cur, msgSize - (cur - start));
		if (numBytes <= 0) {
			logToMe->writeLn ("PDBCommunicator: error reading socket when trying to accept query");
			logToMe->writeLn (strerror (errno));
			return true;
		}
		cur += numBytes;
	}

	// read in the info string
	string infoBack (start);
	info = infoBack;
	start += info.length () + 1;	

	// and read in the query itself---get the number of bytes to deserialize
	size_t byteCount = msgSize - (info.length () + 1);

	// send that bytecount so that we can use the generic deserializer
	queryToRead = machine.deSerialize (start, byteCount);
	if (byteCount != msgSize - (info.length () + 1)) {
		logToMe->writeLn ("PDBCommunicator: Query bytes deserialized is not what was expected!!");
		errMsg = "Query bytes deserialized was wrong";
		return true;
	}	

	// everything was OK!
	return false;
}

bool PDBCommunicator :: acceptQuery (PDBQueryExecutorPtr &executeMe, string &errMsg, string &info, 
	PDBQueryExecutorDeserializationMachine &deserWithMe) {

	return acceptQuery (executeMe, errMsg, info, PDBMsgType :: QueryMsg, deserWithMe);	
}

bool PDBCommunicator :: acceptQueryResult (PDBQueryExecutorPtr &result, string &errMsg, string &info,
	PDBQueryExecutorDeserializationMachine &deserWithMe) {

	return acceptQuery (result, errMsg, info, PDBMsgType :: QueryResultMsg, deserWithMe);
}

// message format:
// 1) PDBMsgType :: LoadDataMsg
// 2) total message length in bytes
// 3) info string length
// 4) info string
// 5) number of data object to load
// 6) size of first object
// 7) first object
// 8) size of second object
// 9) second object
// 10) ...
//
bool PDBCommunicator :: sendLoadData (vector<PDBData> &loadMe, string info) {

	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// write the message type
	char *start, *cur;
	start = cur = myMem->getRaw ();
	*((PDBMsgType *) cur) = PDBMsgType :: LoadDataMsg;
	cur += sizeof (PDBMsgType);

	// reserve some space to write the message length
	cur += sizeof (size_t);

	// write the c-string for the info
	memcpy (cur, info.c_str (), info.length () + 1);
	cur += info.length () + 1;

	// write out the number of data objects
	*((int *) cur) = loadMe.size ();
        cur += sizeof (int);

	// and serialize them
	for (PDBData p : loadMe) {
		size_t *init = (size_t *) cur;
		cur += sizeof (size_t);
		size_t numBytes;
		cur = (char *) p.serialize (cur, numBytes);
		*init = numBytes;
	}

	// record the length
	*((size_t *) (start + sizeof (PDBMsgType))) = cur - start - sizeof (PDBMsgType) - sizeof (size_t);

	return doTheWrite (cur, start, "could not write send load data message");
}

bool PDBCommunicator :: acceptLoadData (vector <PDBData> &loadMe, string &errMsg, string &info, 
	PDBStoredDataTypeDeserializationMachine &deserUsingMe, PDBFeatureTypeDeserializationMachine &usingMeTo) {

	// get a page of buffer memory
	PDBBufferPagePtr myMem = myPool->get ();

	// get the message type
	if (!readCurMsgType) {
		getTypeOfNextMsg ();				
	}
	readCurMsgType = false;

	// see if we got an error message
	if (curMsgType == PDBMsgType :: ErrMsg) {
		return acceptError (errMsg);
	}

	// make sure the type is acceptable
	if (curMsgType != PDBMsgType :: LoadDataMsg) {
		logToMe->writeLn ("PDBCommunicator: Trying to accept some data to load, but got a weird message type");
		errMsg = "bad message type";
		return true;
	}

	// loop and read the message
	char *start = myMem->getRaw ();
	char *cur = start;
	while (cur - start < (long) msgSize) {
		ssize_t numBytes = read (socketFD, cur, msgSize - (cur - start));
		if (numBytes <= 0) {
			logToMe->writeLn ("PDBCommunicator: error reading socket when trying to accept data to load");
			logToMe->writeLn (strerror (errno));
			errMsg = "bad read";
			return true;
		}
		cur += numBytes;
	}

	// read in the info string
	string infoBack (start);
	info = infoBack;
	start += info.length () + 1;	

	// read in the number of objects
	int numObjects = *((int *) start);
	start += sizeof (int);

	// read in all of the objcts
	loadMe.clear ();
	for (int i = 0; i < numObjects; i++) {

		// get the size
		size_t objectSize = *((size_t *) start);
		start += sizeof (size_t);

		// get the object
		PDBData tempData;
		tempData.deSerialize (start, objectSize, deserUsingMe, usingMeTo);
		loadMe.push_back (tempData);	
		start += objectSize;	
	}

	if ((long) msgSize != start - myMem->getRaw ()) {
		logToMe->writeLn ("PDBCommunicator: Data bytes deserialized is not what was expected!!");
		errMsg = "bad message size";
		return true;
	}	
	
	return false;
}
#endif

