
#ifndef BACKEND_QUERY_WORK_H
#define BACKEND_QUERY_WORK_H

#include "PDBBuzzer.h"
#include "PDBCommWork.h"
#include "SimplePDB.h"
#include <string>

using namespace std;

// execute a multi-threaded query
class BackEndQueryWork : public PDBCommWork {

public:

	// specify the database object to open up
	BackEndQueryWork (SimplePDBPtr queryMeIn);

	// called when there was an error during this work
	void setError (string errorMsgIn);

	// called when one of the sub-workers has finished
	void queryDone ();
	
	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// make a copy
	PDBCommWorkPtr clone () override;

	// get the default buzzer
	PDBBuzzerPtr getLinkedBuzzer () override;

private:

	// in case of an error
	string errorMsg;

	// tells us if there was an error
	bool wasError;

	// the database to query
	SimplePDBPtr queryMe;
	
	// the number of completed sub-tasks
	int counter;
};

#endif

