
#ifndef BACKEND_QUERY_WORK_CC
#define BACKEND_QUERY_WORK_CC

#include "BackEndQueryBuzzer.h"
#include "BackEndQueryOneRunWork.h"
#include "BackEndQueryWork.h"
#include <string>
using namespace std;

BackEndQueryWork :: BackEndQueryWork (SimplePDBPtr queryMeIn) {
	queryMe = queryMeIn;	
	wasError = false;
	counter = 0;
}

void BackEndQueryWork :: setError (string errorMsgIn) {
	wasError = true;	
	errorMsg = errorMsgIn;	
	getLogger ()->writeLn ("BackEndQueryWork: found an error: " + errorMsgIn);
	counter++;
}

void BackEndQueryWork :: queryDone () {
	counter++;
}

PDBCommWorkPtr BackEndQueryWork :: clone () {
	return make_shared <BackEndQueryWork> (queryMe);
}

PDBBuzzerPtr BackEndQueryWork :: getLinkedBuzzer () {
	return make_shared <BackEndQueryBuzzer> (*this);
}

void BackEndQueryWork :: execute (PDBBuzzerPtr callerBuzzer) {

	// get the query
	string err, info;
	PDBQueryExecutorPtr myQuery;
	if (getCommunicator ()->acceptQuery (myQuery, err, info, queryMe->getCatalog ()->getQueryExecutorDeserializer ())) {
		getLogger ()->writeLn ("BackEndQueryWork: Was an error getting query: " + err);
                err = "could not get query: " + err;
                getCommunicator ()->sendError (err);
		callerBuzzer->buzz (PDBAlarm :: QueryError);
                return;	
	}

	getLogger ()->writeLn ("BackEndQueryWork: Got query " + info);

	// execute the query
	PDBBuzzerPtr tempBuzzer {getLinkedBuzzer ()};
	vector <PDBWorkPtr> workToDo;
	for (int i = 0; i < queryMe->numRuns (); i++) {

		// and ask a worker to do it	
		PDBWorkerPtr temp = getWorker ();
		workToDo.push_back (make_shared<BackEndQueryOneRunWork> (queryMe, i, myQuery->getEmpty ()));
		temp->execute (workToDo.back (), tempBuzzer);
	}	

	// wait until the work is done
	getLogger ()->writeLn ("BackEndQueryWork: waiting for query to complete");
	while (counter != queryMe->numRuns ()) {
		tempBuzzer->wait ();
	}

	// if there was an error, just get out of here
	if (wasError) {
		getLogger ()->writeLn ("BackEndQueryWork: Was an error running query: " + errorMsg);
                errorMsg = "could not send query result: " + errorMsg;
                getCommunicator ()->sendError (errorMsg);
		callerBuzzer->buzz (PDBAlarm :: QueryError);
		return;
	}

	// otherwise, aggregate the results
        PDBQueryExecutorPtr result = myQuery->getEmpty ();
        for (PDBWorkPtr p : workToDo) {

                // and get the result out
                BackEndQueryOneRunWork *completedWork = dynamic_cast<BackEndQueryOneRunWork *> (p.get ());
                (*result) += completedWork->getResult ();
        }

	// and send back the results
	getLogger ()->writeLn ("BackEndQueryWork: sending result back to client");
	if (getCommunicator ()->sendQueryResult (result, info)) { 
		getLogger ()->writeLn ("BackEndQueryWork: could not send query result back to client");	
		callerBuzzer->buzz (PDBAlarm :: QueryError);
		return;
	}

	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif
