
#ifndef BACKEND_NEW_STORAGE_WORK_H
#define BACKEND_NEW_STORAGE_WORK_H

#include "PDBCommWork.h"
#include "PDBBuzzer.h"
#include "PDBWork.h"
#include "SimplePDB.h"
#include <string>

using namespace std;

// tells the database to use a new storage location 
class BackEndNewStorageLocationWork : public PDBCommWork {

public:

	// specify the database object and the file to open
	BackEndNewStorageLocationWork (SimplePDBPtr openMeIn);

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// clone this guy
	PDBCommWorkPtr clone () override;

private:

	// the database that we are tasked with opening
	SimplePDBPtr openMe;

	// the file to open
	string fName;
};

#endif

