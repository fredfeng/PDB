
#ifndef SAME_DIR_H
#define SAME_DIR_H

#include "PDBMetric.h"

// checks whether twofile sames like /dir1/dir2/file and /dir1/dir2/file are in the same directory;
// returns a 1 if they are, and a 0 otherwise
class CheckSameDirectory : public PDBMetric {

private:

	PDBFeatureTypePtr stringOne;
	PDBFeatureTypePtr stringTwo;
	string errMsg;
	bool gotErr;

public:

	// constructor
	CheckSameDirectory ();

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

