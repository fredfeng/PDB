
#ifndef SERVER_WORK_CC
#define SERVER_WORK_CC

#include "ServerWork.h"
#include "ServerWorkBuzzer.h"

ServerWork :: ServerWork (PDBServer &workOnMeIn) : workOnMe (workOnMeIn)  {
	wasEnError = false;
}

PDBCommWorkPtr ServerWork :: clone () {
	PDBCommWorkPtr returnVal {make_shared <ServerWork> (workOnMe)};
	return returnVal;
}

void ServerWork :: handleError () {
	wasEnError = true;
	getLogger ()->writeLn ("ServerWork: got an error");
}

PDBBuzzerPtr ServerWork :: getLinkedBuzzer () {
	return make_shared <ServerWorkBuzzer> (*this);
}

void ServerWork :: execute (PDBBuzzerPtr callerBuzzer) {

	// while there is still something to do on this connection
	getLogger ()->writeLn ("ServerWork: about to handle a request");
	PDBBuzzerPtr myBuzzer {getLinkedBuzzer ()};
	while (!wasEnError && !workOnMe.handleOneRequest (myBuzzer, getCommunicator ())) {
		getLogger ()->writeLn ("ServerWork: just handled another request");
	}

	getLogger ()->writeLn ("ServerWork: done with this server work");
	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif
