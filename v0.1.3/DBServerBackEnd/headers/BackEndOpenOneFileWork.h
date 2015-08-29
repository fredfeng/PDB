
#ifndef BACKEND_OPEN_ONE_FILE_WORK_H
#define BACKEND_OPEN_ONE_FILE_WORK_H

#include "PDBBuzzer.h"
#include "PDBWork.h"
#include "SimplePDB.h"
#include <string>

using namespace std;

// opens one file associated with this database
class BackEndOpenOneFileWork : public PDBWork {

public:

	// specify the database object and the file to open
	BackEndOpenOneFileWork (SimplePDBPtr openMeIn, string fName);

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

private:

	// the database that we are tasked with opening
	SimplePDBPtr openMe;

	// the file to open
	string fName;
};

#endif

