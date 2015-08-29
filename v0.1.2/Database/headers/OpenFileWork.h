
#ifndef OPEN_FILE_H
#define OPEN_FILE_H

#include "PDBWork.h"
#include "SimplePDB.h"
#include <string>
using namespace std;

// this is a class, that when executed, causes a file to be opened and loaded into the DB
class OpenFileWork : public PDBWork {

public:

	OpenFileWork (SimplePDB *useMe, const string &fileToOpenIn) {
		myPDB = useMe;
		fileToOpen = fileToOpenIn;
	}

	void execute () override {
		string errorMsg;
		if (myPDB->loadUpFile (fileToOpen, errorMsg)) {
			setError (errorMsg);
		}
		done ();
	}

private:

	SimplePDB *myPDB;
	string fileToOpen;
};

#endif

