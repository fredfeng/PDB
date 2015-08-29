
#ifndef BACKEND_ADD_DATA_WORK_CC
#define BACKEND_ADD_DATA_WORK_CC

#include "BackEndAddDataWork.h"
#include <string>
using namespace std;

BackEndAddDataWork :: BackEndAddDataWork (SimplePDBPtr addToMeIn) {
	addToMe = addToMeIn;	
}

PDBCommWorkPtr BackEndAddDataWork :: clone () {
        PDBCommWorkPtr returnVal {make_shared <BackEndAddDataWork> (addToMe)};
	return returnVal;
}

void BackEndAddDataWork :: execute (PDBBuzzerPtr callerBuzzer) {

	string err, info;

	// get the data to add
	vector <PDBData> dataToAdd;
	if (getCommunicator ()->acceptLoadData (dataToAdd, err, info, addToMe->getCatalog ()->getStoredDataTypeDeserializer (),
		addToMe->getCatalog ()->getFeatureTypeDeserializer ())) {
		getLogger ()->writeLn ("BackEndAddDataWork: Was an error getting data to add: " + err);
                err = "could not get data to add (" + err + ")";
                getCommunicator ()->sendError (err);
		callerBuzzer->buzz (PDBAlarm :: GenericError);
                return;	
	}

	getLogger ()->writeLn ("BackEndAddDataWork: got data to add; info was " + info);

	if (addToMe->storeData (dataToAdd, err)) {
		getLogger ()->writeLn ("BackEndAddDataWork: failed to add data: " + err);
		getCommunicator ()->sendError ("failed to add data (" + err + ")");
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	// and send back the results
	getLogger ()->writeLn ("BackEndAddDataWork: done adding data " + info);
	if (getCommunicator ()->sendDataLoadingTaskAllDone (info)) { 
		getLogger ()->writeLn ("BackEndAddDataWork: could not send ack back to client");	
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	getLogger ()->writeLn ("BackEndAddDataWork: done sending ack " + info);
	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif
