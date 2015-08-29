
#ifndef BACKEND_SHUT_DOWN_WORK_H
#define BACKEND_SHUT_DOWN_WORK_H

#include "PDBBuzzer.h"
#include "PDBCommWork.h"

// this really does nothing right now, except log the shut down message
class BackEndShutDownWork : public PDBCommWork {

public:

	// specify the database object and the file to open
	BackEndShutDownWork ();

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// clone this guy
	PDBCommWorkPtr clone () override;

};

#endif

