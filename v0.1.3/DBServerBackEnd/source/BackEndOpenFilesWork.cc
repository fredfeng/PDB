
#ifndef BACKEND_OPEN_FILES_WORK_CC
#define BACKEND_OPEN_FILES_WORK_CC

#include "BackEndOpenFilesWork.h"
#include "BackEndOpenOneFileWork.h"
#include "BackEndOpenFilesWorkBuzzer.h"
#include <string>
using namespace std;

BackEndOpenFilesWork :: BackEndOpenFilesWork (SimplePDBPtr openMeIn) {
	openMe = openMeIn;	
	counter = 0;
}

void BackEndOpenFilesWork :: wasError () {
	cout << "Could not open up one of the files.\n";
	exit (-1);
}

void BackEndOpenFilesWork :: fileDone () {
	counter++;
}

PDBBuzzerPtr BackEndOpenFilesWork :: getLinkedBuzzer () {
	return make_shared <BackEndOpenFilesWorkBuzzer> (*this);
}

void BackEndOpenFilesWork :: execute (PDBBuzzerPtr callerBuzzer) {

	// open each storage location
	PDBBuzzerPtr tempBuzzer = getLinkedBuzzer ();
	getLogger ()->writeLn ("BackEndOpenFilesWork: opening files");

	int target = 0;
	for (string s : openMe->getCatalog ()->getStorageLocations ()) {

		// and ask a worker to do it	
		PDBWorkerPtr temp = getWorker ();
		temp->execute (make_shared <BackEndOpenOneFileWork> (openMe, s), tempBuzzer);
		target++;
	}	

	// and wait until the work is done
	while (counter != target) {
		tempBuzzer->wait ();
	}

	getLogger ()->writeLn ("BackEndOpenFilesWork: all files opened");
	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif
