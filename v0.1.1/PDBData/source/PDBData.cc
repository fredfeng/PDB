
#ifndef PDBDATA_C
#define PDBDATA_C

#include "PDBData.h"
#include <sstream>
#include <iostream>

PDBData :: PDBData () {}

void PDBData :: setData (PDBStoredDataTypePtr &dataToStore) {
	myStoredData = dataToStore;
}

void PDBData :: setKey (PDBFeatureList &extractedFeatures) {
	mySearchKey = extractedFeatures;
}

PDBData :: PDBData (PDBStoredDataTypePtr &dataToStore, PDBFeatureList &extractedFeatures) {
	// this just simply remembers references to the objects we are asked to store
	myStoredData = dataToStore;
	mySearchKey = extractedFeatures;
}

PDBData :: PDBData (PDBStoredDataTypePtr &dataToStore) {
	myStoredData = dataToStore;
	PDBFeatureList empty;
	mySearchKey = empty;
}

void PDBData :: buildFeatures (PDBFeatureListBuilder &useMe) {
	vector <PDBFeatureBuilderPtr> builders = useMe.getFeatureBuilders ();

	// loop through each of the indiviaul feature builders
	for (PDBFeatureBuilderPtr p : builders) {

		// if this guy runs on this data object, then add the new feature
		if (p->runsOnThisPDBData (*this)) {
			PDBFeatureTypePtr newFeature = p->buildFeature (*this);
			getFeatures ().addFeature (newFeature, p->getFeatureName ());
		}		
	}
}

PDBData :: PDBData (const PDBData &fromMe) {
	myStoredData = fromMe.myStoredData;
	mySearchKey = fromMe.mySearchKey;
}

PDBData &PDBData :: operator = (const PDBData &fromMe) {
	myStoredData = fromMe.myStoredData;
	mySearchKey = fromMe.mySearchKey;
	return *this;
}

PDBData :: ~PDBData() {}

PDBFeatureList &PDBData :: getFeatures () {
	return mySearchKey;
}

PDBStoredDataTypePtr PDBData :: getData () {
	return myStoredData;
}
 
void *PDBData :: serialize (void *toHereIn, size_t &size) {

	char *toHere = (char *) toHereIn;

	// write out the code for the stored ata
	*(int *) toHere = myStoredData->getCode ();
	toHere += sizeof (int);

	// now serialize the two objects
	size_t tot1, tot2;	
	toHere = (char *) myStoredData->serialize (toHere, tot1);
	toHere = (char *) mySearchKey.serialize (toHere, tot2);
	size = tot1 + tot2 + sizeof (int);
	return toHere;
}

void *PDBData :: deSerialize (void *fromHereIn, size_t &size, PDBStoredDataTypeDeserializationMachine &dataDeserializer,
        PDBFeatureTypeDeserializationMachine &featureDeserializer) {

	// so that we can do pointer arithmatic
	char *fromHere = (char *) fromHereIn;

	// just deserialize the two objects
	size_t tot1, tot2;
	bool success;

	// deserialize the data first
	myStoredData = dataDeserializer.deSerialize (fromHere, tot1, success);
	if (!success) {
		cerr << "Error when trying to deserialize a stored data object.\n";
		exit (1);
	}

	// and then deserialize the list of features
	fromHere += tot1;
	fromHere = (char *) mySearchKey.deSerialize (fromHere, tot2, featureDeserializer);
	size = tot1 + tot2;
	return fromHere;
}

string PDBData :: display () {
	ostringstream output;	
	if (myStoredData.get () != 0)
		output << "{[" << myStoredData->display () << "]";
	for (PDBFeatureTypePtr p : mySearchKey.getFeatures ()) {
		output << "[" << p->display () << "]";	
	}
	output << "}";
	return output.str ();
}

#endif
