
#ifndef PDB_WORKER_Q_H
#define PDB_WORKER_Q_H

#include <memory>
using namespace std;

// create a smart pointer for PDBWorkerQueue objects
class PDBWorkerQueue;
typedef shared_ptr <PDBWorkerQueue> PDBWorkerQueuePtr;

#include "PDBLogger.h"
#include "PDBWorker.h"
#include <pthread.h>
#include <set>
#include <vector>

// this encapsulates a queue of workers
class PDBWorkerQueue {

public:

	// create a worker queue that has the specified number of workers... output is written 
	// to the specified logger
	PDBWorkerQueue (PDBLoggerPtr myLogger, int numWorkers);

	// destroy the queue... if there are any outstanding workers, this will not complete
	// until they have all finished their work and been destroyed
	~PDBWorkerQueue ();

	// this gets a worker thread, encapsulated within an instance of the PDBWorker class
	// If all of the threds that are managed by this particular PDBWorkerQueue are busy
	// doing work, then this call will block.  If there are no wokers at all in this
	// PDBWorkerQueue object (not even any busy ones) this returns a nullptr
	// 
	// Once a worker finishes its work (that is, it finishes executing a PDBWork object)
	// it is automatically recycled (returned to the queue).  The only way that it is
	// not recycled is if worker.needToDieWhenDone () returns true.  In this case, the
	// thread inside of the worker is allowed to die.
	//
	// IMPORTANT: if you get a PDBWorker object via this call, you MUST use it via a call
	// to execute, or else the thread inside of it becomes a zombie, and will wait forever!
	// 
	// note that this is the only interface method that can be run asynchrously, outside of
	// the main thread.  It is assumed that all other calls to the interface methods run on
	// the same thread!!
	PDBWorkerPtr getWorker ();

	// adds another worker to the queue
	void addAnotherWorker ();

	// notify all of the workers that are currently assigned (and working) regarding some 
	// event.  This basically means that worker.soundBuzzer (withMe) is called for all 
	// workers.   If, after the call to worker.soundBuzzer (withMe), it is the case that
	// worker.needToDieWhenDone () returns true, then the worker is immediately (and 
	// automatically) replaced with a new one.  In this way, a call to notifyAllWorkers
	// can be used to have a bunch of threads die in response to some event.  For example,
	// if a server goes down, then all of the threads that are interacting with it can
	// be notified via a call to notifyAllWorkers; they can then decide to kill themselves
	// and safely be abandoned by the system, to be reclaimed later.
	void notifyAllWorkers (PDBAlarm withMe);
	
	// this is how a thread gets into the queue... it calls "enter"... don't call this
	// method unless you want to be captured and used as a worker!!
	void enter ();

	// gets the logger
	PDBLoggerPtr getLogger ();

private:

	// these are the workers that are unassigned
	vector <PDBWorkerPtr> waiting;

	// the protecting mutex, and the assocaited signal variable
	pthread_mutex_t waitingMutex;
	pthread_cond_t waitingSignal;

	// these are all of the workers that are currently doing work
	set <PDBWorkerPtr> working;

	// the list of all of the threads
	vector <pthread_t> threads;

	// the protecting mutex
	pthread_mutex_t workingMutex;

	// keeps track of how many workers we have created
	int numOut;

	// true when the destructor is active
	bool shuttingDown;

	// the logger to write to
	PDBLoggerPtr myLogger;

};


#endif
