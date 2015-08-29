
#ifndef SAME_DIR_H
#define SAME_DIR_H

#include "PDBMetric.h"

// checks whether twofile sames like /dir1/dir2/file and /dir1/dir2/file are in the same directory;
// returns a 1 if they are, and a 0 otherwise
class CheckSameDirectory : public PDBMetric {

private:

	PDBFeatureTypePtr stringOne;
	PDBFeatureTypePtr stringTwo;

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

