
#ifndef PDB_WORK_H
#define PDB_WORK_H

#include <memory>
#include <pthread.h>
#include <string>
using namespace std;

// create a smart pointer for PDBWork objects
class PDBWork;
typedef shared_ptr <PDBWork> PDBWorkPtr;

// this class wraps up a bit of work that needs to be accomplished by the database engine...
// the idea is that a thread will get an object of this type, then call "execute" on it, and
// will then call "done" once the work has been completed.
//
// meanwhile, the person who is waiting for the work to be done will call waitUntilDone, which
// blocks until the work has been completed

class PDBWork {

public:

	// actually go off and do the work
	virtual void execute () = 0;

	// wait until the work is done... this blocks until someone calls
	// the "done" method
	void waitUntilDone ();

	// let anyone who is blocking on a call to waitUntilDone () let that
	// we have now completed the work
	void done ();

	// returns true if there was an error when this work executed
	bool wasError ();

	// gets the error string of the work
	const string &getError ();

	// tell this guy that there was an error
	void setError (const string &errorString);

	// set up and tear down the internal mutex
	PDBWork ();
	virtual ~PDBWork ();

private:

	// true if and only if this work has been completed
	bool workDone;

	// these are used to protect the workDone variable
	pthread_mutex_t myMutex;
	pthread_cond_t myVar;

	// true if there was an error
	bool wasAnError;

	// the error string
	string errorDescription;
};

#endif
