
#ifndef PDB_WORKER_H
#define PDB_WORKER_H

// create a smart pointer for PDBWorker objects
#include <memory>
using namespace std;
class PDBWorker;
typedef shared_ptr <PDBWorker> PDBWorkerPtr;

#include "PDBAlarm.h"
#include "PDBBuzzer.h"
#include "PDBWork.h"
#include "PDBWorkerQueue.h"

// this class wraps up a thread... note that PDBWorker objects should always be created
// by the PDBWorkerQueue class. 

class PDBWorker {

public:

	// constructor... accepts the work queue that is creating it
	PDBWorker (PDBWorkerQueue *parent);

	// destructor
	~PDBWorker ();

	// gets another worder from the same work queue that this guy came from... note that
	// this call can block if the work queue does not have any additional workers
	PDBWorkerPtr getWorker ();

	// asks this worker to execute runMe... this call is non-blocking.  When the task
	// is done, myBuzzer->buzz (PDBAlarm :: WorkAllDone) is called.  
	void execute (PDBWorkPtr runMe, PDBBuzzerPtr myBuzzer);

	// sounds the buzzer for this guy... the buzzer associated with the call to "execute"
	// will go off, and the event will be "withMe"... this is typically called with respect
	// to some global event
	void soundBuzzer (PDBAlarm withMe);

	// gets access to the logger
	PDBLoggerPtr getLogger ();

	// entry point for a thread
	void enter ();
	
	// this resets the contents
	void reset ();

private:

	// the work queue we came from
	PDBWorkerQueue *parent;

	// the work to run
	PDBWorkPtr runMe;

	// used to signal when we are done, or when some event happens
	PDBBuzzerPtr buzzWhenDone;

	// for coordinating the thread who is running this guy
	pthread_mutex_t workerMutex;
	pthread_cond_t workToDoSignal;

	// set to true when the worker is able to go and do the work
	bool okToExecute;
};

#endif
