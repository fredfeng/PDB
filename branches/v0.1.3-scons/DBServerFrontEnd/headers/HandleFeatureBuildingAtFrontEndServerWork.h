
#ifndef HANDLE_FEATURE_FRONT_END_SERVER_WORK_H
#define HANDLE_FEATURE_FRONT_END_SERVER_WORK_H

#include "PDBBuzzer.h"
#include "PDBCommWork.h"
#include "PDBFeatureBuilderDeserializationMachine.h"
#include <string>

using namespace std;

// this is a class that encapsultes the protocol for accepting data to load from a client
class HandleFeatureBuildingAtFrontEndServerWork : public PDBCommWork {
	
public:

	// this is to be called to let the worker know that the backend has totally
	// died and so reconnection is necessary
	void wasAFatalServerError ();

	// from the interface; accept a query, then forward it on to the backend, accept
	// a result from the backend, and forward the result to the client
	void execute (PDBBuzzerPtr callerBuzzer) override;

	// from the interface; used when the server needs to build one of these
	PDBCommWorkPtr clone () override;

	// from the interface; return a buzzer linked to this guy
	PDBBuzzerPtr getLinkedBuzzer () override;

	// constructor; accepts the location of the file that will be used to house the
	// socket enabling communication with the backend
	HandleFeatureBuildingAtFrontEndServerWork (string pathOfBackendIn);

private:

	// true when we are told that the server has died
	bool wasFatalError;

	// lets us know what file to connect to for the backened socket
	string pathOfBackend;

	// the info string
	string info;
};

#endif

