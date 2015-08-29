
#ifndef EUCLIDEAN_H
#define EUCLIDEAN_H

#include "PDBMetric.h"

// computes the Euclidean distance between two SparseVectorFeatureType objects
class EuclideanDistance : public PDBMetric {

private:

	PDBFeatureTypePtr vecOne;
	PDBFeatureTypePtr vecTwo;

public:

	// from the interface
	string getName ();
	int getNumParams ();
	void takeParam (int whichParam, PDBFeatureTypePtr param);
	string getParamType (int whichParam);
	void reset ();
	double apply (bool &wasError, string &errorMessage);
};

#endif

