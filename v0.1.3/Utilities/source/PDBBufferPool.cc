
#ifndef PDB_BUFFER_POOL
#define PDB_BUFFER_POOL

#include <iostream>
#include "LockGuard.h"
#include "PDBBufferPool.h"

using namespace std;

PDBBufferPagePtr PDBBufferPool :: get () {

	// wait until there is a page
	const LockGuard guard {waitingMutex};
	while (myPages.size () == 0) {
		pthread_cond_wait (&waitingSignal, &waitingMutex);
	}

	PDBBufferPagePtr returnVal {myPages[myPages.size () - 1]};
	myPages.pop_back ();	
	return returnVal;
}

PDBBufferPool :: PDBBufferPool (int numPagesIn, size_t memPerPageIn) {

	// add all of the pages we need
	for (int i = 0; i < numPagesIn; i++) {
		PDBBufferPagePtr temp {make_shared <PDBBufferPage> (memPerPageIn, this)};
		myPages.push_back (temp);
	}

	// allocate the concurrency control stuff
	pthread_cond_init (&waitingSignal, nullptr);
	pthread_mutex_init (&waitingMutex, nullptr);
}

PDBBufferPool :: ~PDBBufferPool () {

	// free all of the pages; otherwise, when myPages is destroyed, the pages inside
	// will be recycled, which will lead to an infinute loop (recycling adds them back
	// into the buffer
	while (myPages.size () > 0) {
		PDBBufferPagePtr temp = get ();
		temp->freePage ();
	}

	pthread_cond_destroy (&waitingSignal);
	pthread_mutex_destroy (&waitingMutex);
}

void PDBBufferPool :: returnBytes (char *rawBytes, size_t numBytes) {
	const LockGuard guard {waitingMutex};
	PDBBufferPagePtr temp {make_shared <PDBBufferPage> (rawBytes, numBytes, this)};
	myPages.push_back (temp);	
}

#endif
