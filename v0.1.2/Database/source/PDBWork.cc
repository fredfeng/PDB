
#ifndef PDB_WORK_C
#define PDB_WORK_C

#include "LockGuard.h"
#include "PDBWork.h"
#include <pthread.h>

void PDBWork :: done () {

	const LockGuard guard {myMutex};
	workDone = true;
	pthread_cond_broadcast (&myVar);
}

void PDBWork :: waitUntilDone () {

	const LockGuard guard {myMutex};
	while (workDone != true) {
		pthread_cond_wait (&myVar, &myMutex);
	}
}

PDBWork :: PDBWork ()
	: workDone (false),
	  myMutex (PTHREAD_MUTEX_INITIALIZER),
	  myVar (PTHREAD_COND_INITIALIZER),
	  wasAnError (false)
{
}

PDBWork :: ~PDBWork () {
	pthread_cond_destroy (&myVar);
	pthread_mutex_destroy (&myMutex);
}

void PDBWork :: setError (const string &errorString) {
	wasAnError = true;
	errorDescription = errorString;
}

const string &PDBWork :: getError () {
	return errorDescription;
}

bool PDBWork :: wasError () {
	return wasAnError;
}

#endif

