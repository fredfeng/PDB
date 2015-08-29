
#ifndef ADD_DATA_H
#define ADD_DATA_H

#include "PDBData.h"
#include "PDBWork.h"
#include "SimplePDB.h"
#include <vector>
using namespace std;

// this is a class, that when executed, causes a set of data items to be loaded into the DB
class AddDataWork : public PDBWork {

public:

	AddDataWork (SimplePDB *useMe, vector <PDBData> &dataToAddIn) : dataToAdd (dataToAddIn) {
		myPDB = useMe;
	}

	void execute () override {
		string errMsg;
		if (myPDB->storeData (dataToAdd, errMsg))
			setError (errMsg);
		done ();
	}

private:

	SimplePDB *myPDB;
	vector <PDBData> &dataToAdd;
};

#endif

