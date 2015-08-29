
#ifndef PDB_WORKER_Q_C
#define PDB_WORKER_Q_C

#include "EmptyPDBBuzzer.h"
#include <iostream>
#include "LockGuard.h"
#include "NothingWork.h"
#include "PDBWorkerQueue.h"

using namespace std;

PDBWorkerQueue :: PDBWorkerQueue (PDBLoggerPtr myLoggerIn, int numWorkers) {
	pthread_mutex_init (&waitingMutex, nullptr);
	pthread_mutex_init (&workingMutex, nullptr);
	pthread_cond_init (&waitingSignal, nullptr);
	shuttingDown = false;
	numOut = 0;
	for (int i = 0; i < numWorkers; i++) {
		addAnotherWorker ();
	}
	myLogger = myLoggerIn;
}

PDBLoggerPtr PDBWorkerQueue :: getLogger () {
	return myLogger;
}

PDBWorkerQueue :: ~PDBWorkerQueue () {

	// let everyone know we are shutting down
	shuttingDown = true;

	// keep getting threads and giving them nothing to do... once they complete
	// the nothing work, they will die since shuttingDown = true.  This goes 
	// until we can't get any thrads because there are no more of them outstanding
	while (true) {

		// basically, we repeatedly ask for workers; this call will return
		// when either (a) there is a worker available, or (b) there are
		// no workers available but there are no workers in existence
		PDBWorkerPtr myWorker = getWorker ();
		if (myWorker == nullptr)
			break;

		PDBBuzzerPtr empty {make_shared <EmptyPDBBuzzer> ()};
		PDBWorkPtr nothing {make_shared <NothingWork> ()};
		myWorker->execute (nothing, empty);

	}

	// join with all of the threads
	for (pthread_t thread : threads) {
		if (pthread_join (thread, nullptr) != 0) {
			cout << "Error joining with thread as the worker queue is shutting down!!\n";
			exit (-1);
		}
	}

	// kill all of the mutexes/signal vars
	pthread_mutex_destroy (&waitingMutex);
	pthread_mutex_destroy (&workingMutex);
	pthread_cond_destroy (&waitingSignal);
	
}

PDBWorkerPtr PDBWorkerQueue :: getWorker () {

	PDBWorkerPtr myWorker;

	{
		// make sure there is a worker
		const LockGuard guard {waitingMutex};
		while (waiting.size () == 0 && numOut != 0) {
			pthread_cond_wait (&waitingSignal, &waitingMutex);
		}

		// special case: there are no workers... just return a null
		if (numOut == 0) {
			return nullptr;
		}

		// get the worker
		myWorker = waiting.back ();
		waiting.pop_back ();	
	}

	{
		// remember that he is working
		const LockGuard guard {workingMutex};
		working.insert (myWorker);		
	}

	return myWorker;
}

// this is the entry point for all of the worker threads
void *enterTheQueue (void *pdbWorkerQueueInstance) {
        PDBWorkerQueue *temp = static_cast <PDBWorkerQueue *> (pdbWorkerQueueInstance);
	temp->enter ();
        return nullptr;
}

void PDBWorkerQueue :: addAnotherWorker () {

	pthread_t thread;
	threads.push_back (thread);
	int return_code = pthread_create (&(threads[threads.size () - 1]), nullptr, enterTheQueue, this);
	if (return_code) {
		cout << "ERROR; return code from pthread_create () is " << return_code << '\n';
		exit (-1);
	} else {
		numOut++;
	}
}

void PDBWorkerQueue :: notifyAllWorkers (PDBAlarm withMe) {
	const LockGuard guard {workingMutex};
		
	// loop through all of the current workers
	for (PDBWorkerPtr p : working) {

		// sound the buzzer... 
		p->soundBuzzer (withMe);
	}
}

void PDBWorkerQueue :: enter () {

	// create the work
	PDBWorkerPtr temp {make_shared <PDBWorker> (this)};

	// then work until (a) someone told us that we need to die, or
	// (b) we are trying to shut down the worker queue 
	while (!shuttingDown) {

		// put the work on the end of the queue
		{
			const LockGuard guard {waitingMutex};
			waiting.push_back (temp);
			pthread_cond_signal (&waitingSignal);	
		}

		// and enter the work
		temp->enter ();

		// when we have exited the work, it means that we are done, do we are no longer working
		{
			const LockGuard guard {workingMutex};
			if (working.erase (temp) != 1) {
				cout << "Why did I find != 1 copies of the worker?";
				exit (-1);
			}
		}

		// make sure to kill all references to stuff that we want to be able to dellocate
		// or else we will block and that stuff will sit around forever
		temp->reset ();
	}

	// if we exited the loop, then this particular worker
	// needs to die... so decrement the count of outstanding 
	// workers and let everyone know if the number is down to zero
	{
		// decrement the count of outstanding workers
		const LockGuard guard {waitingMutex};
		numOut--;

		// if there are no outstanding workers, everyone is woken up and gets a null
		if (numOut == 0)
			pthread_cond_broadcast (&waitingSignal);
	}
}

#endif
