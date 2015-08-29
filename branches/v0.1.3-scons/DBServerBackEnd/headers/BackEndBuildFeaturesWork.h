
#ifndef BACKEND_BUILD_FEATURES_WORK_H
#define BACKEND_BUILD_FEATURES_WORK_H

#include "PDBBuzzer.h"
#include "PDBCommWork.h"
#include "SimplePDB.h"
#include <string>

using namespace std;

// execute a multi-threaded feature building task
class BackEndBuildFeaturesWork : public PDBCommWork {

public:

	// specify the database object to open up
	BackEndBuildFeaturesWork (SimplePDBPtr buildMeIn);

	// called when there was an error during this work
	void setError ();

	// called when one of the sub-workers has finished
	void taskDone ();
	
	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

        // clone this guy
        PDBCommWorkPtr clone () override;

	// returns a buzzer linked to this guy
	PDBBuzzerPtr getLinkedBuzzer () override;

private:

	// in case of an error
	string errorMsg;

	// tells us if there was an error
	bool wasError;

	// the database to query
	SimplePDBPtr buildMe;
	
	// the number of completed sub-tasks
	int counter;
};

#endif

