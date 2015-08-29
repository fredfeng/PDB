
#ifndef PDB_BUZZ_C
#define PDB_BUZZ_C

#include "LockGuard.h"
#include "PDBBuzzer.h"

void PDBBuzzer :: buzz (PDBAlarm withMe) {

	const LockGuard guard {waitingMutex};
	handleBuzz (withMe);
	pthread_cond_broadcast (&waitingSignal);
}

void PDBBuzzer :: wait () {

        // wait until there is a buzz
        const LockGuard guard {waitingMutex};
	pthread_cond_wait (&waitingSignal, &waitingMutex);
}

PDBBuzzer :: PDBBuzzer () {
	pthread_cond_init (&waitingSignal, nullptr);
	pthread_mutex_init (&waitingMutex, nullptr);
}

PDBBuzzer :: ~PDBBuzzer () {
	pthread_cond_destroy (&waitingSignal);
	pthread_mutex_destroy (&waitingMutex);
}

#endif
