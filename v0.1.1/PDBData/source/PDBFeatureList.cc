
#ifndef PDBFEATURELIST_C_
#define PDBFEATURELIST_C_

#include "PDBFeatureList.h"
#include <iostream>

PDBFeatureList :: PDBFeatureList () {}
PDBFeatureList :: ~PDBFeatureList () {}

vector <PDBFeatureTypePtr> PDBFeatureList :: getFeatures () {

	vector <PDBFeatureTypePtr> returnVal;
	for (PDBFeatureTypePtr p : myFeatures) {
		returnVal.push_back (p);	
	}
	return returnVal;
}

void PDBFeatureList :: addFeature (PDBFeatureTypePtr addMe, string name) {

	// first, see if a feature with this name is already there
	for (int i = 0; i < featureNames.size (); i++) {
		if (featureNames[i] == name) {
			myFeatures[i] = addMe;
			return;
		}
	}

	// if it is not there, then add it
	myFeatures.push_back (addMe);
	featureNames.push_back (name);
}

int PDBFeatureList :: getNumFeatures () {
	return myFeatures.size ();
}

PDBFeatureList :: PDBFeatureList (PDBFeatureList &fromMe) {
	myFeatures = fromMe.myFeatures;
	featureNames = fromMe.featureNames;
}

PDBFeatureList &PDBFeatureList :: operator = (const PDBFeatureList &fromMe) {
	myFeatures = fromMe.myFeatures;
	featureNames = fromMe.featureNames;
	return *this;
}

PDBFeatureTypePtr PDBFeatureList :: getIthFeature (int i) {
	if (i >= getNumFeatures ()) {
		cerr << "You asked for feature " << i << " when the number of features was " 
			<< getNumFeatures () << ". Death!!";
		exit (EXIT_FAILURE);
	}
	return myFeatures[i];
}

PDBFeatureTypePtr PDBFeatureList :: getFeatureNamed (string &findMe, bool &success) {

	// look for the feature
	for (int i = 0; i < featureNames.size (); i++) {
		if (featureNames[i] == findMe) {
			success = true;
			return myFeatures[i];
		}
	}		

	// if we don't find it, then return a bogus feature
	success = false;
	return shared_ptr <PDBFeatureType> ();
	
}

void *PDBFeatureList :: serialize (void *toHereIn, size_t &size) {

	// so we can do pointer arithmatic
	char *toHere = (char *) toHereIn;

	// write out the number of entries
	int *temp = (int *) toHere;
	*temp = featureNames.size ();
	toHere += sizeof (int);

	// now, write out each entry
	for (int i = 0; i < featureNames.size (); i++) {

		// write out the name of the feature
		for (int j = 0; j < featureNames[i].length (); j++) {
			*toHere = featureNames[i].at (j);
			toHere++;		
		}

		// null terminate it
		*toHere = 0;
		toHere++;

		// now, write out the code
		temp = (int *) toHere;
		*temp = myFeatures[i]->getCode ();
		toHere += sizeof (int);

		// and write out the feature
		size_t tempSize;
		toHere = (char *) myFeatures[i]->serialize (toHere, tempSize);
	}

	// and get outta here
	size = toHere - (char *) toHereIn;
	return toHere;
}

void *PDBFeatureList :: deSerialize (void *fromHereIn, size_t &size, PDBFeatureTypeDeserializationMachine &machine) {

	// so we can do arithmatic
	char *fromHere = (char *) fromHereIn;

	// empty out the current data
	featureNames.clear ();	
	myFeatures.clear ();	

	// read in the number of entries
	int *temp = (int *) fromHere;
	fromHere += sizeof (int);

	// now, read in each entry
	for (int i = 0; i < *temp; i++) {

		// read in the string
		string myString (fromHere);
		featureNames.push_back (myString);
	
		// find the end of the string
		for (; *fromHere != 0; fromHere++);
		fromHere++;
		
		// and read in the feature
		size_t numBytesRead;
		bool success;
		myFeatures.push_back (machine.deSerialize (fromHere, numBytesRead, success));
		fromHere += numBytesRead;			

		if (!success) {
			cerr << "Error when trying to deserialize features.\n";
			exit (1);
		}
	}	
	
	// and get outta here
	size = fromHere - (char *) temp;
	return fromHere;
}

#endif
