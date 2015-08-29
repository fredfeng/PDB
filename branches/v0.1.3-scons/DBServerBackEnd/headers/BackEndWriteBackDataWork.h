
#ifndef BACKEND_WRITE_BACK_DATA_WORK_H
#define BACKEND_WRITE_BACK_DATA_WORK_H

#include "PDBWork.h"
#include "SimplePDB.h"

// write back all of the data from one run to disk
class BackEndWriteBackDataWork : public PDBWork {

public:

	// specify the database object, and which list we are writing back
	BackEndWriteBackDataWork (SimplePDBPtr writeMe, int whichListIn);

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

private:

	// the database that we are tasked with writing back data for
	SimplePDBPtr writeMe;

	// the run that we are writing back data for
	int whichList;

};

#endif

