
#ifndef WRITE_BACK_DATA_H
#define WRITE_BACK_DATA_H

#include "PDBWork.h"
#include "SimplePDB.h"
using namespace std;

// this is a class, that when executed, causes all objects from a file to be written out
class WriteBackDataWork : public PDBWork {

public:

	WriteBackDataWork (SimplePDB *useMe, int whichFileIn) {
		myPDB = useMe;
		whichFile = whichFileIn;
	}

	void execute () override {
		string errMsg;
		if (myPDB->writeBackData (whichFile, errMsg)) {
			setError (errMsg);
		}
		done ();
	}

private:

	SimplePDB *myPDB;
	int whichFile;
};

#endif

