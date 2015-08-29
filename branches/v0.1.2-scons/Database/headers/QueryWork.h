
#ifndef QUERY_WORK_H
#define QUERY_WORK_H

#include "PDBQueryExecutor.h"
#include "PDBWork.h"
#include "SimplePDB.h"

// this is a class, that when executed, causes a query to be run
class QueryWork : public PDBWork {

public:

	QueryWork (SimplePDB *useMe, int whichListIn, PDBQueryExecutorPtr myQuery) {
		myPDB = useMe;
		whichList = whichListIn;
		queryToRun = myQuery;
	}

	void execute () override {
		string err;
		if (myPDB->runQuery (queryToRun, whichList, err)) {
			setError (err);			
		}
		done ();
	}

	PDBQueryExecutorPtr getResult () {
		return queryToRun;
	}

private:

	SimplePDB *myPDB;
	int whichList;
	PDBQueryExecutorPtr queryToRun;
};

#endif

