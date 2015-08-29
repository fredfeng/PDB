
#ifndef PDBFEATURELIST_C_
#define PDBFEATURELIST_C_

#include "PDBFeatureList.h"
#include <iostream>

vector <PDBFeatureTypePtr> PDBFeatureList :: getFeatures () {
	return myFeatures;
}

void PDBFeatureList :: addFeature (PDBFeatureTypePtr addMe, const string &name) {

	// first, see if a feature with this name is already there
	for (size_t i = 0; i < featureNames.size (); i++) {
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

void PDBFeatureList :: clear () {
	myFeatures.clear ();
	featureNames.clear ();
}

PDBFeatureTypePtr PDBFeatureList :: getIthFeature (int i) {
	if (i >= getNumFeatures ()) {
		cerr << "You asked for feature " << i << " when the number of features was " 
			<< getNumFeatures () << ". Death!!";
		exit (EXIT_FAILURE);
	}
	return myFeatures[i];
}

PDBFeatureTypePtr PDBFeatureList :: getFeatureNamed (const string &findMe, bool &success) {

	// look for the feature
	for (size_t i = 0; i < featureNames.size (); i++) {
		if (featureNames[i] == findMe) {
			success = true;
			return myFeatures[i];
		}
	}		

	// if we don't find it, then return a bogus feature
	success = false;
	return {};
}

void *PDBFeatureList :: serialize (void *toHereIn, size_t &size) {

	// so we can do pointer arithmatic
	char *toHere = static_cast<char *> (toHereIn);

	// write out the number of entries
	int *temp = reinterpret_cast<int *> (toHere);
	*temp = featureNames.size ();
	toHere += sizeof (int);

	// now, write out each entry
	for (size_t i = 0; i < featureNames.size (); i++) {

		// write out the name of the feature
		for (const auto &elem : featureNames[i]) {
			*toHere = elem;
			toHere++;		
		}

		// null terminate it
		*toHere = 0;
		toHere++;

		// now, write out the code
		temp = reinterpret_cast<int *> (toHere);
		*temp = myFeatures[i]->getCode ();
		toHere += sizeof (int);

		// and write out the feature
		size_t tempSize;
		toHere = static_cast<char *> (myFeatures[i]->serialize (toHere, tempSize));
	}

	// and get outta here
	size = toHere - static_cast<char *> (toHereIn);
	return toHere;
}

void *PDBFeatureList :: deSerialize (void *fromHereIn, size_t &size, PDBFeatureTypeDeserializationMachine &machine, bool &success) {

	// so we can do arithmatic
	char *fromHere = static_cast<char *> (fromHereIn);

	// empty out the current data
	featureNames.clear ();	
	myFeatures.clear ();	

	// read in the number of entries
	int *temp = reinterpret_cast<int *> (fromHere);
	fromHere += sizeof (int);

	// now, read in each entry
	success = true;
	for (int i = 0; i < *temp; i++) {

		// read in the string
		featureNames.emplace_back (fromHere);
	
		// find the end of the string
		for (; *fromHere != 0; fromHere++);
		fromHere++;
		
		// and read in the feature
		size_t numBytesRead;
		myFeatures.push_back (machine.deSerialize (fromHere, numBytesRead, success));
		fromHere += numBytesRead;			

		if (!success) {
			return fromHereIn;
		}
	}	
	
	// and get outta here
	size = fromHere - reinterpret_cast<char *> (temp);
	return fromHere;
}

void *PDBFeatureList :: deSerialize (void *fromHereIn, size_t &size, PDBFeatureTypeDeserializationMachine &machine) {
	bool success;
	void *returnVal = deSerialize (fromHereIn, size, machine, success);
	if (success) {
		return returnVal;
	} else {
		cout << "Error deserializing in PDBFeatureList.\n";
		exit (-1);
	}
}

#endif
