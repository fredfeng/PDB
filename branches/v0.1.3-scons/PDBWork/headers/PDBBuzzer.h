
#ifndef PDB_BUZZ_H 
#define PDB_BUZZ_H

#include <memory>
using namespace std;

// create a smart pointer for PDBBuzzer objects
class PDBBuzzer;
typedef shared_ptr <PDBBuzzer> PDBBuzzerPtr;

// the way that this works is that anyone who is waiting on the buzzer (via a call to wait) is
// blocked until someone calls buzz.  After the call to buzz:
// 
// (1) first, the appropriate handleBuzz function is called (either the one with the message or 
//	without the message)
// (2) second, anyone blocked on wait is woken up
//
// The way that this is used is that when one has work to do, they will create a piece of work,
// as well as a worker, and then get the buzzer associated with that worker.  They then ask the
// worker to do the work, and (optionally) they block on the work.  When the work finishes, it
// will typically call buzz (PDBAlarm :: WorkAllDone), causing anyone blocked on the work to 
// wake up.  
#include "PDBAlarm.h"
#include <pthread.h>
#include <string>

class PDBBuzzer {

public:

	// sounds the buzzer, causing the appropriate action to be taken
	// and anyone blocked on "wait" to wake up... note that just the
	// first alarm is processed; others are ignored
	void buzz (PDBAlarm withMe);

	// similar, but allows a string (presumably holding an appropriate
	// message) to be passed to the buzzer
	void buzz (PDBAlarm withMe, string message);

	// blocks until someone calls buzz 
	void wait ();

	// called with withMe when someone calles buzz (withMe)
	virtual void handleBuzz (PDBAlarm withMe) = 0;

	// constructor, destructor
	PDBBuzzer ();
	~PDBBuzzer ();

private:

	pthread_mutex_t waitingMutex;
	pthread_cond_t waitingSignal;

};

#endif
