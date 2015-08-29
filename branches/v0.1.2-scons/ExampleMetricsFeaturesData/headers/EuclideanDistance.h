
#ifndef EUCLIDEAN_H
#define EUCLIDEAN_H

#include "PDBMetric.h"

// computes the Euclidean distance between two SparseVectorFeatureType objects
class EuclideanDistance : public PDBMetric {

private:

	PDBFeatureTypePtr vecOne;
	PDBFeatureTypePtr vecTwo;
	bool gotError;
	string errorMsg;

public:

	// constructor
	EuclideanDistance ();

	// from the interface
	const string &getName () override;
	int getNumParams () override;
	void takeParam (int whichParam, PDBFeatureTypePtr param) override;
	const string &getParamType (int whichParam) override;
	void reset () override;
	double apply (bool &wasError, string &errorMessage) override;
	PDBMetricPtr getCopy () override;
};

#endif

