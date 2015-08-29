
#ifndef SPARSE_VEC_C
#define SPARSE_VEC_C

#include "SparseVectorFeatureType.h"
#include <sstream>
#include <string>
using namespace std;

double SparseVectorFeatureType :: getValue (int slot) {

	const auto found (myMap.find (slot));
	return found == myMap.end () ? 0.0 : found->second;
}

vector <int> SparseVectorFeatureType :: getNonZeroSlots () {
	vector <int> nonZeroSlots;
	nonZeroSlots.reserve (myMap.size ());
	for (const auto &elem : myMap) {
		nonZeroSlots.push_back (elem.first);
	}
	return nonZeroSlots;
}

const string &SparseVectorFeatureType :: getTypeName () {
	static const string result {"SparseVectorFeatureType"};
	return result;
}

void SparseVectorFeatureType :: normalize () {

	// count the size
	double count = 0;
	for (const auto &elem : myMap) {
		count += elem.second;
	}
	for (auto &elem : myMap) {
		elem.second /= count;
	}
}

int SparseVectorFeatureType :: getCode () {
	return 5923458;
}

void *SparseVectorFeatureType :: serialize (void *toHereIn, size_t &size) {

	// first write the size
	char *toHere = static_cast<char *> (toHereIn);
	*(reinterpret_cast<int *> (toHere)) = myMap.size ();
	toHere += sizeof (int);

	// now write the slots
	for (const auto &elem : myMap) {
		*(reinterpret_cast<int *> (toHere)) = elem.first;
		toHere += sizeof (int);
		*(reinterpret_cast<double *> (toHere)) = elem.second;
		toHere += sizeof (double);
	}

	// done!
	size = toHere - static_cast<char *> (toHereIn);
	return toHere;
	
}

string SparseVectorFeatureType :: display () {

	ostringstream output;
	int i = 0;
	output << '<';
	for (const auto &elem : myMap) {
		if (i != 0)
			output << ", ";
		output << elem.first << ':' << elem.second;
		if (i == 10)
			break;
		i++;
	}
	output << '>';
	return output.str();
}

void *SparseVectorFeatureType :: deSerialize (void *fromHereIn, size_t &size) {

	// read in the size
	char *fromHere = static_cast<char *> (fromHereIn);
	int readsize = *(reinterpret_cast<int *> (fromHere));
	fromHere += sizeof (int);

	// now read the slots
	myMap.clear ();
	for (int i = 0; i < readsize; i++) {
		int first = *(reinterpret_cast<int *> (fromHere));
		fromHere += sizeof (int);
		double second = *(reinterpret_cast<double *> (fromHere));
		fromHere += sizeof (double);
		myMap[first] = second;
	}

	// done!
	size = fromHere - static_cast<char *> (fromHereIn);
	return fromHere;
}

PDBFeatureTypePtr SparseVectorFeatureType :: getNewFeatureTypeObject () {
	return make_shared<SparseVectorFeatureType> ();
}

void SparseVectorFeatureType :: add (int whichSlot, double value) {
	myMap[whichSlot] = value;
}

#endif


