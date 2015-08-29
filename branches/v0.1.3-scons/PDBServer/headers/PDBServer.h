
#ifndef PDB_SERVER_H
#define PDB_SERVER_H

#include <map>
#include <memory>
using namespace std;

// create a smart pointer for PDBServer objects
class PDBServer;
typedef shared_ptr <PDBServer> PDBServerPtr;

#include "PDBAlarm.h"
#include "PDBCommWork.h"
#include "PDBBufferPool.h"
#include "PDBLogger.h"
#include "PDBMsgType.h"
#include "PDBWork.h"
#include <string>

using namespace std;

// This class encapsulates a multi-threaded sever in PDB.  The way it works is that one simply registers 
// an event handler (encapsulated inside of a PDBWorkPtr); whenever there is a new connection that comes
// over the given port (or file in the case of a local socket) a PWBWorker is asked to handle the 
// connection using the appropriate port using a cloned version of the specified PDBWork object.
// 
class PDBServer {

public:

	// server set up for comminication over the internet, with the specified number of threads 
	// (PDBWorker objects) to handle connections to the server.  Log to the specified logger.
	// The server will use the buffer pool specified.
	PDBServer (int portNumber, int numConnections, PDBLoggerPtr myLogger, PDBBufferPoolPtr useMe);

	// server set up for communication using the local file system, with the specified number of threads
	// to handle connections to the server.  Log to the specified logger.
	// The server will use the buffer pool specified.
	PDBServer (string fileName, int numConnections, PDBLoggerPtr myLogger, PDBBufferPoolPtr useMe);

	// asks the server to handle a particular request coming over the wire with the particular work type
	void registerHandler (PDBMsgType requestType, PDBCommWorkPtr handledBy);

	// starts the server---this creates all of the threads and lets the server start taking requests; this
	// call will never return.  Note that if runMeAtStart is not null, then runMeAtStart is executed 
	// before the server starts handling requests
	void startServer (PDBWorkPtr runMeAtStart);

	// asks the server to signal all of the threads activily handling connections that a certain event
	// has occured; this effectively just has us call PDBWorker.signal (signalWithMe) for all of the 
	// workers that are currently handling requests.  Any that indicate that they have died as a result
	// of the signal are forgotten (allowed to go out of scope) and then replaced with a new PDBWorker 
	// object
	void signal (PDBAlarm signalWithMe);
	
	// tell the server to start listening for people who want to connect
	void listen ();

	// asks us to handle one request that is coming over the given PDBCommunicator; return true if this
	// is the last request over this PDBCommunicator object; buzzMeWhenDone is sent to the worker that
	// is spawned to handle the request
	bool handleOneRequest (PDBBuzzerPtr buzzMeWhenDone, PDBCommunicatorPtr myCommunicator);

private:

	// when we get a message over the input socket, we'll handle it using the registered handler
	map <PDBMsgType, PDBCommWorkPtr> handlers;

	// this is where all of our workers to handle the server requests live
	PDBWorkerQueuePtr myWorkers;

	// handles a request using the given PDBCommunicator to obtain the data
	void handleRequest (PDBCommunicatorPtr myCommunicator);

	// this is the buffer pool the server will use for communication
	PDBBufferPoolPtr myPool;

	// true when the server is done
	bool allDone;

	// the port number for an internet server
	int portNumber;

	// the number of connections to allow simultanously (a new thread is created for each of these)
	int numConnections;

	// the file to use for a UNIX file-based connection
	string unixFile;

	// where to log to
	PDBLoggerPtr myLogger;
	
	// true if this is an internet server
	bool isInternet;
	
	// used to run the server
	pthread_t listenerThread;

	// this is the socket we are listening to
	int sockFD;
};

#endif
