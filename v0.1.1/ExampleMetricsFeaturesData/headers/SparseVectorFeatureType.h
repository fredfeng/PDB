
#ifndef SPARSE_VEC_H
#define SPARSE_VEC_H

#include <map>
#include <vector>
#include "PDBFeatureType.h"
using namespace std;

// stores a vector in a sparse format
class SparseVectorFeatureType : public PDBFeatureType {

public:

	// constructor/destructor
	SparseVectorFeatureType ();
	~SparseVectorFeatureType ();

	// add a new value
	void add (int whichSlot, double value);

	// gets the value for a particular slot
	double getValue (int slot);

	// gets a list of the non-zero slots
	vector <int> getNonZeroSlots ();

	// normalize the vector (so it sums to one)
	void normalize ();

	// from the interface
	string getTypeName ();
	int getCode ();
	void *serialize (void *toHere, size_t &size);
	void *deSerialize (void *fromHere, size_t &size);
	PDBFeatureTypePtr getNewFeatureTypeObject ();
	string display ();
	
private:
	
	// this gives a sparse representation of a vector
	map <int, double> myMap;	
};

#endif


