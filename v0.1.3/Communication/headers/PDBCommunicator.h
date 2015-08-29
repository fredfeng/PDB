
#ifndef PDB_COMMUN_H
#define PDB_COMMUN_H

#include <memory>
using namespace std;

// create a smart pointer for PDBCommunicator objects
class PDBCommunicator;
typedef shared_ptr <PDBCommunicator> PDBCommunicatorPtr;

#include "PDBLogger.h"
#include "PDBMsgType.h"
#include "PDBBufferPool.h"
#include "PDBQueryExecutor.h"
#include "PDBQueryExecutorDeserializationMachine.h"
#include "PDBFeatureBuilderDeserializationMachine.h"

// This class the encoding/decoding of IPC sockets messages in PDB 
// 
class PDBCommunicator {

public:
	
	// here are the ways that you can set up a PDBCommunicator...
	// the first two assume that the PDBCommunicator is on the server side:

	// #1, you can connect to a Unix domain socket that is pointed at a file (this returns a true
	// if there is an error, and in this case it sets errMsg to describe the error)
	bool pointToFile (PDBLoggerPtr logToMeIn, int socketFDIn, string& errMsg, PDBBufferPoolPtr myPoolIn); 

	// #2, you can connect to an internet socket... 
	bool pointToInternet (PDBLoggerPtr logToMeIn, int socketFDIn, string& errMsg, PDBBufferPoolPtr myPoolIn); 

	// the next two assume we are on the client side:

	// #3 connect to a server on the internet...
	bool connectToInternetServer (PDBLoggerPtr logToMeIn, int portNumber, string serverAddress,
        	string &errMsg, PDBBufferPoolPtr myPoolIn);

	// #4, connect to a local server via a UNIX domain socket
	bool connectToLocalServer (PDBLoggerPtr logToMeIn, string fName, string& errMsg, PDBBufferPoolPtr myPoolIn);

	// see what someone is sending us... this will block until a message comes over the 
	PDBMsgType getTypeOfNextMsg ();

	// note that the file descriptor corresponding to the socket is always closed by the destructor!
	~PDBCommunicator ();

	// default constructor
	PDBCommunicator ();

	// get access to this guy's buffer pool
	PDBBufferPoolPtr getBufferPool ();

	/******************************* METHODS FOR SENDING DATA ************************************/
	//
	// All of these methods are meant to send something to another process; the process
	// could be located on a remote machine, or it could be located on this  machine.
	//
	// All of these communication functions return a true on error, false otherwise. 
	//
	// Each of the methods also contains the ability to send a string called "info"
	// which can be used for logging/debugging at the other end of the connection.
	// This information string should not have any effect on the process at the
	// other end, but it could conceibably be printed/logged for later analysis.
	// 

	// tell the server to close this connection
	bool sendCloseConnection (string info);

	// sends an error message to the specified destination
	bool sendError (string errMsg);

	// sends a query to the specified destination
	bool sendQuery (PDBQueryExecutorPtr executeMe, string info);

	// sends a query result to the specified destination
	bool sendQueryResult (PDBQueryExecutorPtr result, string info);

	// sends a feature building task to the specified location
	bool sendFeatureBuildingTask (PDBFeatureListBuilder &executeMe, string info);

	// lets someone know that a feature building task has been completed
	bool sendFeatureBuildingTaskAllDone (string info);

	// sends a data loading task to the specified destination
	bool sendLoadData (vector<PDBData> &loadMe, string info);

	// lets someone know that the data loading task has been completed
	bool sendDataLoadingTaskAllDone (string info);

	// asks to change the file storage location
	bool sendChangeDataStorageLocation (string fName);

	// sends that the location has been changed
	bool sendDataStorageLocationChanged (string info);

	// asks for the server to shut down
	bool sendShutDown (string info);

	// sends that the server has been shut down
	bool sendShutDownComplete (string info);

	// all of these send a specific type of binary code to someone... allows them the ability
	// to execute code that can use the various types of data objects that are sent around 
	// from the client side
	bool sendBinaryCodeForPDBFeatureBuilder (void *code, size_t numBytes, string info);
	bool sendBinaryCodeForPDBStoredDataType (void *code, size_t numBytes, string info);
	bool sendBinaryCodeForPDBFeatureType (void *code, size_t numBytes, string info);
	bool sendBinaryCodeForPDBQueryExecutor (void *code, size_t numBytes, string info);
	bool sendBinaryCodeForPDBMetric (void *code, size_t numBytes, string info);
	
	// lets someone know that the binary code that has been sent has now been received
	bool sendCodeForPDBFeatureBuilderLoaded (string info);
	bool sendCodeForPDBStoredDataTypeLoaded (string info);
	bool sendCodeForPDBFeatureTypeLoaded (string info);
	bool sendCodeForPDBQueryExecutorLoaded (string info);
	bool sendCodeForPDBMetricLoaded (string info);

	// finally, this sends only a text message
	bool sendInfo (string info);

	// and this acks that the text has been recevied
	bool sendInfoHasBeenReceived (string info);

	/******************************* METHOD FOR IGNORING MESSAGE ************************************/
	// this ignores the next message, effecitively deleting it from the socket
	string ignoreMessage ();	

	/******************************* METHODS FOR RECEIVING MESSAGES ************************************/
	// these are the analogs of the "send" methods above, and accept the various message types
	// returns a true if there was some sort of error (or if an error message was sent) and in this
	// case errMsg is set so that it describes the error

	// accepts a request to close the conneciton
	bool acceptCloseConnection (string &errMsg, string &info);

	// accept an error message... by definition, this always returns true, since there was an error
	bool acceptError (string &errMsg);
	
	// get a query from the specified destination
	bool acceptQuery (PDBQueryExecutorPtr &executeMe, string &errMsg, string &info, 
		PDBQueryExecutorDeserializationMachine &deserUsingMe);

	// get a query result from the specified destination
	bool acceptQueryResult (PDBQueryExecutorPtr &result, string &errMsg, string &info,
		PDBQueryExecutorDeserializationMachine &deserUsingMe);

	// gets a feature building task from the specified location
	bool acceptFeatureBuildingTask (PDBFeatureListBuilder &executeMe, string &errMsg, string &info,
		PDBFeatureBuilderDeserializationMachine &deserUsingMe);

	// wait until a feature building task has been completed
	bool acceptFeatureBuildingTaskAllDone (string &errMsg, string &info);

	// sends a data loading task from the specified destination
	bool acceptLoadData (vector <PDBData> &loadMe, string &errMsg, string &info, 
		PDBStoredDataTypeDeserializationMachine &deserUsingMe, PDBFeatureTypeDeserializationMachine &usingMeTo);

	// wait until the data loading task has been completed
	bool acceptDataLoadingTaskAllDone (string &errMsg, string &info);

	// accepts a new stoage location 
	bool acceptChangeDataStorageLocation (string &errMsg, string &fName);

	// accepts that the storage location change is done
	bool acceptChangeDataStorageLocationDone (string &errMsg, string &info);

	// accepts the shut down
	bool acceptShutDown (string &errMsg, string &info);

	// accepts that a shut down is complete
	bool acceptShutDownComplete (string &errMsg, string &info);

	// get binary code from someone
	bool acceptBinaryCodeForPDBFeatureBuilder (void *code, size_t numBytes, string &errMsg, string &info);
	bool acceptBinaryCodeForPDBStoredDataType (void *code, size_t numBytes, string &errMsg, string &info);
	bool acceptBinaryCodeForPDBFeatureType (void *code, size_t numBytes, string &errMsg, string &info);
	bool acceptBinaryCodeForPDBQueryExecutor (void *code, size_t numBytes, string &errMsg, string &info);
	bool acceptBinaryCodeForPDBMetric (void *code, size_t numBytes, string &errMsg, string &info);
	
	// wait until someone says that binary code has been received 
	bool acceptCodeForPDBFeatureBuilderLoaded (string &errMsg, string &info);
	bool acceptCodeForPDBStoredDataTypeLoaded (string &errMsg, string &info);
	bool acceptCodeForPDBFeatureTypeLoaded (string &errMsg, string &info);
	bool acceptCodeForPDBQueryExecutorLoaded (string &errMsg, string &info);
	bool acceptCodeForPDBMetricLoaded (string &errMsg, string &info);

	// finally, this gets a text message from someone
	bool acceptInfo (string &errMsg, string &info);

	// and this acks that the text has been received
	bool acceptInfoHasBeenReceived (string &errMsg, string &info);

private:

	// write from start to end to the output socket; log the string in errMsgIn if there is a problem, and
	// in this case, set errMsgOut = errMsgIn.  Return true on error
	bool doTheWrite (char *cur, char *start, string errMsgIn);

	// send a text message; the type of message written is msgType
	bool sendText (PDBMsgType msgType, string text);

	// sends a query; the type of message is msgType
	bool sendQuery (PDBQueryExecutorPtr executeMe, string info, PDBMsgType msgType);

	// accepts a text message
	bool acceptText (PDBMsgType &msgTypeIn, string &err, string &text);

	// accepts a query; the type of the message to accept is msgType
	bool acceptQuery (PDBQueryExecutorPtr &queryToRead, string &errMsg, string &info, PDBMsgType msgType,
        	PDBQueryExecutorDeserializationMachine &machine);

	// the next message type
	PDBMsgType curMsgType;

	// the size of the next message; keep these two delarations together so they can be read into at once!
	size_t msgSize;
	
	// tells us if we have read the message type or not
	bool readCurMsgType;

	// this points to the location that we read/write from
	int socketFD;

	// for logging
	PDBLoggerPtr logToMe;

	// the buffer pool
	PDBBufferPoolPtr myPool;

	// true if we should disconnect when done
	bool needToSendDisconnectMsg;
};

#endif
