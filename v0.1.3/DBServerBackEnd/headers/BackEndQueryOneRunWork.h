
#ifndef BACKEND_QUERY_ONE_RUN_WORK_H
#define BACKEND_QUERY_ONE_RUN_WORK_H

#include "PDBBuzzer.h"
#include "PDBQueryExecutor.h"
#include "SimplePDB.h"

// queries one run of records in this database
class BackEndQueryOneRunWork : public PDBWork {

public:

	// specify the database object, the run to query, and the query to run
	BackEndQueryOneRunWork (SimplePDBPtr queryMe, int whichListIn, PDBQueryExecutorPtr runMeIn);

	// do the actual work
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// get the result of the query
	PDBQueryExecutorPtr getResult ();

private:

	// the database that we are tasked with querying
	SimplePDBPtr queryMe;

	// the run to query
	int whichList;

	// the query
	PDBQueryExecutorPtr runMe;
};

#endif

