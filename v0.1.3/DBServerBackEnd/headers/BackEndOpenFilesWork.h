
#ifndef BACKEND_OPEN_FILES_WORK_H
#define BACKEND_OPEN_FILES_WORK_H

#include "PDBBuzzer.h"
#include "PDBWork.h"
#include "SimplePDB.h"

// opens all of the data files associated with this database
class BackEndOpenFilesWork : public PDBWork {

public:

	// specify the database object to open up
	BackEndOpenFilesWork (SimplePDBPtr openMeIn);

	// called when there was an error during this work
	void wasError ();

	// called when one file has been opened
	void fileDone ();
	
	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// get a buzzer linked to this guy
	PDBBuzzerPtr getLinkedBuzzer () override;

private:

	// counts the number of files open
	int counter;

	// the database that we are tasked with opening
	SimplePDBPtr openMe;
};

#endif

