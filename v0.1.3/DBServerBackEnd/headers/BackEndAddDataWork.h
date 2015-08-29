
#ifndef BACKEND_ADD_DATA_WORK_H
#define BACKEND_ADD_DATA_WORK_H

#include "PDBBuzzer.h"
#include "PDBCommWork.h"
#include "SimplePDB.h"

// execute a multi-threaded query
class BackEndAddDataWork : public PDBCommWork {

public:

	// specify the database object to open up
	BackEndAddDataWork (SimplePDBPtr addToMeIn);

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

        // clone this guy
        PDBCommWorkPtr clone () override;

private:

	// the database to query
	SimplePDBPtr addToMe;
	
};

#endif

