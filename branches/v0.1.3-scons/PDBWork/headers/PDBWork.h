
#ifndef PDB_WORK_H
#define PDB_WORK_H

#include <memory>
using namespace std;

// create a smart pointer for PDBWork objects
class PDBWork;
typedef shared_ptr <PDBWork> PDBWorkPtr;

#include "PDBBuzzer.h"
#include "PDBLogger.h"
#include "PDBWorker.h"
#include "PDBWorkerQueue.h"
#include <pthread.h>
#include <string>

// this class wraps up a bit of work that needs to be accomplished...
// the idea is that one first obtains a PDBWorker that has been loaded
// with this work; they call PDBWorker.execute () which executes the 
// work, and then they get the result.  

class PDBWork {

public:

	// actually go off and do the work... the worker can optionally invoke
	// any of the handlers embedded in callerBuzzer to ask the caller to
	// deal with errors
	virtual void execute (PDBBuzzerPtr callerBuzzer) = 0;

	// gets a buzzer that is "linked" to this object; that is, a PDBBuzzer
	// object that (optionally) overrides PDBBuzzer.handleBuzz () and performs
	// a callback to this object, invoking specialized code that handles the
	// event.  For example, we might create a buzzer object with a  PDBBuzzer.handleBuzz ()
	// that catches any PDBAlarm (other than PDBAlarm :: WorkAllDone) and then
	// calls a method on this object to handle the  event
	virtual PDBBuzzerPtr getLinkedBuzzer ();

	// this is called by the PDBWorker class to initiate the work
	void execute (PDBWorkerQueue *parent, PDBBuzzerPtr callerBuzzer);

	// gets another worker from the PDBWorkQueue that was used to
	// create the PDBWorker who is running this guy... 
	PDBWorkerPtr getWorker ();

	// gets access to the logger
	PDBLoggerPtr getLogger ();

private:

	// this is the work queue that this dude came from... used to supply
	// additional workers, if they are requested
	PDBWorkerQueue *parent;
};

#endif
