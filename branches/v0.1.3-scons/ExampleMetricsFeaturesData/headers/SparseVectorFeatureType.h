
#ifndef SPARSE_VEC_H
#define SPARSE_VEC_H

#include "PDBFeatureType.h"
#include <map>
#include <vector>
using namespace std;

// stores a vector in a sparse format
class SparseVectorFeatureType : public PDBFeatureType {

public:

	// add a new value
	void add (int whichSlot, double value);

	// gets the value for a particular slot
	double getValue (int slot);

	// gets a list of the non-zero slots
	vector <int> getNonZeroSlots ();

	// normalize the vector (so it sums to one)
	void normalize ();

	// from the interface
	const string &getTypeName () override;
	int getCode () override;
	void *serialize (void *toHere, size_t &size) override;
	void *deSerialize (void *fromHere, size_t &size) override;
	PDBFeatureTypePtr getNewFeatureTypeObject () override;
	string display () override;
	
private:
	
	// this gives a sparse representation of a vector
	map <int, double> myMap;	
};

#endif


