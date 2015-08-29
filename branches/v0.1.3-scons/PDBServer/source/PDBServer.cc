
#ifndef PDB_SERVER_CC
#define PDB_SERVER_CC

#include "EmptyPDBBuzzer.h"
#include <iostream>
#include <netinet/in.h>
#include "PDBServer.h"
#include "ServerWork.h"
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <sys/un.h>
#include <unistd.h>

using namespace std;

PDBServer :: PDBServer (int portNumberIn, int numConnectionsIn, PDBLoggerPtr myLoggerIn, PDBBufferPoolPtr myPoolIn) {

	portNumber = portNumberIn;
	numConnections = numConnectionsIn;
	myLogger = myLoggerIn;
	myPool = myPoolIn;
	isInternet = true;	
	allDone = false;	
}

PDBServer :: PDBServer (string unixFileIn, int numConnectionsIn, PDBLoggerPtr myLoggerIn, PDBBufferPoolPtr myPoolIn) {

	unixFile = unixFileIn;
	numConnections = numConnectionsIn;
	myLogger = myLoggerIn;
	myPool = myPoolIn;
	isInternet = false;	
	allDone = false;	
}

void PDBServer :: registerHandler (PDBMsgType requestType, PDBCommWorkPtr handledBy) {
	handlers[requestType] = handledBy;
}

// this is the entry point for the listener to the port 
void *callListen (void *serverInstance) {
        PDBServer *temp = static_cast<PDBServer *> (serverInstance);
        temp->listen ();
        return nullptr;
}

void PDBServer :: listen () {

	// two cases: first, we are connecting to the internet
	if (isInternet) {

		// wait for an internet socket
		sockFD = socket (AF_INET, SOCK_STREAM, 0);
		if (sockFD < 0) {
                	myLogger->writeLn ("PDBServer: could not get FD to internet socket");
            		myLogger->writeLn (strerror (errno));
			exit (0);
		}

		// bind the socket FD
		struct sockaddr_in serv_addr;
		bzero ((char *) &serv_addr, sizeof (serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons (portNumber);
		int retVal = :: bind (sockFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
		if (retVal < 0) {
                	myLogger->writeLn ("PDBServer: could not bind to local socket");
            		myLogger->writeLn (strerror (errno));
			exit (0);
		}

		myLogger->writeLn ("PDBServer: about to listen to the Internet for a connection");

		// set the backlog on the socket
		if (:: listen (sockFD, 5) != 0) {
                	myLogger->writeLn ("PDBServer: listen error");
            		myLogger->writeLn (strerror (errno));
			exit (0);
		}

		myLogger->writeLn ("PDBServer: ready to go!");

		// wait for someone to try to connect
		while (!allDone) {

			// set up communication with a client
			PDBCommunicatorPtr myCommunicator {make_shared <PDBCommunicator> ()};
			string errMsg;
			if (myCommunicator->pointToInternet (myLogger, sockFD, errMsg, myPool)) {
				myLogger->writeLn ("PDBServer: could not point to an internet socket: " + errMsg);
				continue;
			}

			handleRequest (myCommunicator);
		}

	// second, we are connecting to a local UNIX socket
	} else {

        	myLogger->writeLn ("PDBServer: getting socket to file");
        	sockFD = socket (PF_UNIX, SOCK_STREAM, 0);
		if (sockFD < 0) {
                	myLogger->writeLn ("PDBServer: could not get FD to local socket");
            		myLogger->writeLn (strerror (errno));
			exit (0);
		}

		// bind the socket FD
		struct sockaddr_un serv_addr;
		bzero ((char *) &serv_addr, sizeof (serv_addr));
		serv_addr.sun_family = AF_UNIX;
        	snprintf (serv_addr.sun_path, sizeof (serv_addr.sun_path), "%s", unixFile.c_str ());

		if (:: bind (sockFD, (struct sockaddr *) &serv_addr, sizeof (struct sockaddr_un))) {
                	myLogger->writeLn ("PDBServer: could not bind to local socket");
            		myLogger->writeLn (strerror (errno));
			exit (0);
		}

		myLogger->writeLn ("PDBServer: socket has name");
		myLogger->writeLn (serv_addr.sun_path);

		myLogger->writeLn ("PDBServer: about to listen to the file for a connection");

		// set the backlog on the socket
		if (:: listen (sockFD, 5) != 0) {
                	myLogger->writeLn ("PDBServer: listen error");
            		myLogger->writeLn (strerror (errno));
			exit (0);
		}

		myLogger->writeLn ("PDBServer: ready to go!");

		// wait for someone to try to connect
		while (!allDone) {

			// set up communication with a client
			PDBCommunicatorPtr myCommunicator {make_shared <PDBCommunicator> ()};
			string errMsg;
			if (myCommunicator->pointToFile (myLogger, sockFD, errMsg, myPool)) {
				myLogger->writeLn ("PDBServer: could not point to an local UNIX socket: " + errMsg);
				continue;
			}

			handleRequest (myCommunicator);
		}
	}

	// let the main thread know we are done
	allDone = true;
}

void PDBServer :: handleRequest (PDBCommunicatorPtr myCommunicator) {
	PDBCommWorkPtr tempWork {make_shared <ServerWork> (*this)};
	tempWork->setGuts (myCommunicator);
	PDBWorkerPtr tempWorker = myWorkers->getWorker ();
	tempWorker->execute (tempWork, tempWork->getLinkedBuzzer ());
}
	
bool PDBServer :: handleOneRequest (PDBBuzzerPtr callerBuzzer, PDBCommunicatorPtr myCommunicator) {

	// figure out what type of message the client is sending us
	PDBMsgType requestID = myCommunicator->getTypeOfNextMsg ();

	// if there was a request to close the connection, just get outta here
	if (requestID == PDBMsgType :: CloseConnection) {
		string err, info;
		if (myCommunicator->acceptCloseConnection (err, info)) {
			myLogger->writeLn ("PDBServer: close connection request, but was an error: " + err);
		} else {
			myLogger->writeLn ("PDBServer: close connection request: " + info);
		}
		return true;
	}

	// and get a worker plus the appropriate work to service it
	if (handlers.count (requestID) == 0) {
		
		// there is not one, so send back an appropriate message
		myLogger->writeLn ("PDBServer: could not find an appropriate handler");
		myCommunicator->sendError ("bad request; could not find a handler");

	// in this case, got a handler
	} else {

		// get the handler
		myLogger->writeLn ("PDBServer: found an appropriate handler");
		PDBCommWorkPtr tempWork = handlers[requestID]->clone ();
		tempWork->setGuts (myCommunicator);

		// get a worker to run the handler (this blocks if no workers available)
		PDBWorkerPtr tempWorker = myWorkers->getWorker ();
		tempWorker->execute (tempWork, callerBuzzer);

		callerBuzzer->wait ();
		myLogger->writeLn ("PDBServer: handler has completed its work");
	}

	// if we are asked to shut down...
	if (requestID == PDBMsgType :: ShutDownMsg) {
		
		// kill the FD and let everyone know we are done
		allDone = true;
		close (sockFD);
		return true;

	} else {
		return false;
	}
}

void PDBServer :: signal (PDBAlarm signalWithMe) {
	myWorkers->notifyAllWorkers (signalWithMe);
}

void PDBServer :: startServer (PDBWorkPtr runMeAtStart) {

	// ignore broken pipe signals
	:: signal (SIGPIPE, SIG_IGN);

	// create the workers
	myWorkers = make_shared <PDBWorkerQueue> (myLogger, numConnections);

	// if there was some work to execute to start things up, then do it
	if (runMeAtStart != nullptr) {
		PDBBuzzerPtr buzzMeWhenDone = runMeAtStart->getLinkedBuzzer ();
		PDBWorkerPtr tempWorker = myWorkers->getWorker ();
		tempWorker->execute (runMeAtStart, buzzMeWhenDone);
		buzzMeWhenDone->wait ();
	}

	// listen to the socket
	int return_code = pthread_create (&listenerThread, nullptr, callListen, this);
	if (return_code) {
		cout << "ERROR; return code from pthread_create () is " << return_code << '\n';
		exit (-1);
	}

	// and now just sleep 
	while (!allDone) {
		sleep (1);
	}
}

#endif
