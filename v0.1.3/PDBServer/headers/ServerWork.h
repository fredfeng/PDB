
#ifndef SERVER_WORK_H
#define SERVER_WORK_H

#include "PDBBuzzer.h"
#include "PDBCommWork.h"
#include "PDBServer.h"

// does all of the work associated with one connection to the server 
class ServerWork : public PDBCommWork {

public:

	// specify the server we are working on
	ServerWork (PDBServer &workOnMe);

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// error handler
	void handleError ();

	// clone this guy
	PDBCommWorkPtr clone () override;

	// get buzzer linked to this guy
	PDBBuzzerPtr getLinkedBuzzer () override;

private:

	// calling handleError sets this to true
	bool wasEnError;

	// the server we are doing the work for
	PDBServer &workOnMe;
};

#endif

