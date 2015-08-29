
#ifndef PDB_COMM_WORK_H
#define PDB_COMM_WORK_H

#include <memory>

// "Comm" here stands for communication; so this object encapsulates some work that
// requires communicating via a socket (ether over the internet, or locally, via a
// UNIX socket

// create a smart pointer for PDBCommWork objects
using namespace std;
class PDBCommWork;
typedef shared_ptr <PDBCommWork> PDBCommWorkPtr;

#include <pthread.h>
#include <string>
#include "PDBCommunicator.h"
#include "PDBWork.h"

class PDBCommWork : public PDBWork {

public:

	// gets a new worker of this type and returns it
	virtual PDBCommWorkPtr clone () = 0;

	// accesses the communicator buried in this guy
	PDBCommunicatorPtr getCommunicator ();

	// sets the logger and the commuicator
	void setGuts (PDBCommunicatorPtr toMe);

	// inherited from PDBWork
        // ********************************
	// virtual void execute (PDBBuzzerPtr callerBuzzer);
	// void execute (PDBWorker useMe, PDBBuzzerPtr callerBuzzer);
	// PDBWorkerPtr getWorker ();
	// PDBBuzzerPtr getLinkedBuzzer ();

private:

	// this is responsible for talking over the network
	PDBCommunicatorPtr myCommunicator;

};

#endif
