
#ifndef SPARSE_VEC_C
#define SPARSE_VEC_C

#include <sstream>
#include <string>
#include "SparseVectorFeatureType.h"
using namespace std;

double SparseVectorFeatureType :: getValue (int slot) {

	if (myMap.count (slot) == 0) {
		// did not find it
		return 0.0;
	} else {
		return myMap[slot];
	}
}

vector <int> SparseVectorFeatureType :: getNonZeroSlots () {
	vector <int> nonZeroSlots;
	for (std :: map <int, double> :: iterator it = myMap.begin(); it != myMap.end (); ++it) {
		nonZeroSlots.push_back (it->first);
	}
	return nonZeroSlots;
}

string SparseVectorFeatureType :: getTypeName () {
	return "SparseVectorFeatureType";
}

void SparseVectorFeatureType :: normalize () {

	// count the size
	double count = 0;
	for (std :: map <int, double> :: iterator it = myMap.begin(); it != myMap.end (); ++it) {
		count += it->second;
	}
	for (std :: map <int, double> :: iterator it = myMap.begin(); it != myMap.end (); ++it) {
		it->second = it->second / count;
	}
}

int SparseVectorFeatureType :: getCode () {
	return 5923458;
}

void *SparseVectorFeatureType :: serialize (void *toHereIn, size_t &size) {

	// first write the size
	char *toHere = (char *) toHereIn;
	*((int *) toHere) = myMap.size ();
	toHere += sizeof (int);

	// now write the slots
	for (std :: map <int, double> :: iterator it = myMap.begin(); it != myMap.end (); ++it) {
		*((int *) toHere) = it->first;
		toHere += sizeof (int);
		*((double *) toHere) = it->second;
		toHere += sizeof (double);
	}

	// done!
	size = toHere - (char *) toHereIn;
	return toHere;
	
}

string SparseVectorFeatureType :: display () {

	ostringstream output;
	int i = 0;
	output << "<";
	for (std :: map <int, double> :: iterator it = myMap.begin(); it != myMap.end (); ++it) {
		if (i != 0)
			output << ", ";
		output << it->first << ":" << it->second;
		if (i == 10)
			break;
		i++;
	}
	output << ">";
	return output.str();
}

void *SparseVectorFeatureType :: deSerialize (void *fromHereIn, size_t &size) {

	// read in the size
	char *fromHere = (char *) fromHereIn;
	int readsize = *((int *) fromHere);
	fromHere += sizeof (int);

	// now read the slots
	myMap.clear ();
	for (int i = 0; i < readsize; i++) {
		int first = *((int *) fromHere);
		fromHere += sizeof (int);
		double second = *((double *) fromHere);
		fromHere += sizeof (double);
		myMap[first] = second;
	}

	// done!
	size = fromHere - (char *) fromHereIn;
	return fromHere;
}

PDBFeatureTypePtr SparseVectorFeatureType :: getNewFeatureTypeObject () {
	PDBFeatureTypePtr returnVal (new SparseVectorFeatureType);
	return returnVal;
}

SparseVectorFeatureType :: SparseVectorFeatureType () {}
SparseVectorFeatureType :: ~SparseVectorFeatureType () {}
	
void SparseVectorFeatureType :: add (int whichSlot, double value) {
	myMap[whichSlot] = value;
}

#endif


