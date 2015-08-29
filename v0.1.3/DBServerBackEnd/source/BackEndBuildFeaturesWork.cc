
#ifndef BACKEND_BUILD_FEATURES_WORK_CC
#define BACKEND_BUILD_FEATURES_WORK_CC

#include "BackEndBuildFeaturesWork.h"
#include "BackEndBuildFeaturesBuzzer.h"
#include "BackEndBuildFeaturesOneRunWork.h"
#include "BackEndWriteBackDataWork.h"
#include <string>
using namespace std;

BackEndBuildFeaturesWork :: BackEndBuildFeaturesWork (SimplePDBPtr buildMeIn) {
	buildMe = buildMeIn;	
	wasError = false;
	counter = 0;
}

void BackEndBuildFeaturesWork :: setError () {
	wasError = true;	
	getLogger ()->writeLn ("BackEndBuildFeaturesWork: found an error during feature building");
	counter++;
}

void BackEndBuildFeaturesWork :: taskDone () {
	counter++;
}

PDBCommWorkPtr BackEndBuildFeaturesWork :: clone () {
        return make_shared <BackEndBuildFeaturesWork> (buildMe);
}


PDBBuzzerPtr BackEndBuildFeaturesWork :: getLinkedBuzzer () {
	return {make_shared <BackEndBuildFeaturesBuzzer> (*this)};
}

void BackEndBuildFeaturesWork :: execute (PDBBuzzerPtr callerBuzzer) {

	// get the feature building task
	string err = "", info = "";
	PDBFeatureListBuilder executeMe;
	if (getCommunicator ()->acceptFeatureBuildingTask (executeMe, err, info, 
		buildMe->getCatalog ()->getFeatureBuilderDeserializationMachine ())) {

                getLogger ()->writeLn ("BackEndBuildFeaturesWork: Was an error getting feature builder: " + err);
                err = "could not get feature builder: " + err;
                getCommunicator ()->sendError (err);
                return;
	}

	getLogger ()->writeLn ("BackEndBuildFeaturesWork: got features to add: " + info);

	// send a feature building task to each run
	PDBBuzzerPtr tempBuzzer = getLinkedBuzzer ();
	for (int i = 0; i < buildMe->numRuns (); i++) {

		// and ask a worker to do it	
		PDBWorkerPtr temp = getWorker ();
		temp->execute (make_shared <BackEndBuildFeaturesOneRunWork> (buildMe, i, executeMe), tempBuzzer);
	}

	// wait until the work is done
	while (counter != buildMe->numRuns ()) {
		tempBuzzer->wait ();
	}

	// if there was an error, just get out of here
	if (wasError) {
                getLogger ()->writeLn ("BackEndBuildFeaturesWork: Was an error running feature builder: " + errorMsg);
                errorMsg = "could not build feature";
                getCommunicator ()->sendError (errorMsg);
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	// reset everything
	counter = 0;
	wasError = false;
        getLogger ()->writeLn ("BackEndBuildFeaturesWork: about to do the writebacks");

	// now construct all of the work tasked with writing back the results
	for (int i = 0; i < buildMe->getNumMappedFiles (); i++) {

		// and ask a worker to do it	
		PDBWorkerPtr temp = getWorker ();
		temp->execute (make_shared <BackEndWriteBackDataWork> (buildMe, i), tempBuzzer);
	}

	// wait until the work is done
	while (counter < buildMe->getNumMappedFiles ()) {
		tempBuzzer->wait ();
	}

	// if there was an error, just get out of here
	if (wasError) {
                getLogger ()->writeLn ("BackEndBuildFeaturesWork: Was an error writing back after feature building: " + errorMsg);
                errorMsg = "could not write back after feature building: " + errorMsg;
                getCommunicator ()->sendError (errorMsg);
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}
	
        getLogger ()->writeLn ("BackEndBuildFeaturesWork: done with the writebacks");

	// otherwise, let the caller know we are done
	if (getCommunicator ()->sendFeatureBuildingTaskAllDone ("")) {
		getLogger ()->writeLn ("BackEndQueryWork: could not send query result back to client");
		callerBuzzer->buzz (PDBAlarm :: GenericError);
		return;
	}

	callerBuzzer->buzz (PDBAlarm :: WorkAllDone);
}

#endif
