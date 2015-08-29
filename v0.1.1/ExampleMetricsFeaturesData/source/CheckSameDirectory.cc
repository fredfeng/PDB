
#ifndef CHECK_SAME_DIR_C
#define CHECK_SAME_DIR_C

#include <string>
#include <iostream>
#include "CheckSameDirectory.h"
#include "TextFeatureType.h"
using namespace std;

string CheckSameDirectory :: getName () {
	return "CheckSameDirectory";
}

int CheckSameDirectory :: getNumParams () {
	return 2;
}

void CheckSameDirectory :: takeParam (int whichParam, PDBFeatureTypePtr param) {

	if (param->getCode () != 34958343) {
		cerr << "You gave CheckSameDirectory a param of type " << param->getTypeName () << "\n";
		exit (1);
	}

	if (whichParam == 0) {
		stringOne = param;
		return;
	}
	if (whichParam == 1) {
		stringTwo = param;
		return;
	}
	cerr << "Bad parameter number to CheckSameDirectory!";
	exit (1);
}

string CheckSameDirectory :: getParamType (int whichParam) {
	return "TextFeatureType";
}

void CheckSameDirectory :: reset () {
	PDBFeatureTypePtr temp;
	stringOne = temp;
	stringTwo = temp;
}

double CheckSameDirectory :: apply (bool &wasError, string &errorMessage) {
	if (!stringOne || !stringTwo) {
		wasError = true;
		errorMessage = "One of the parameters to EuclideanDistance was null";
		return 9e99;
	}

        TextFeatureType *stringOneCast = (TextFeatureType *) stringOne.get ();
        TextFeatureType *stringTwoCast = (TextFeatureType *) stringTwo.get ();

	int one = stringOneCast->getVal ().rfind ('/');
	int two = stringTwoCast->getVal ().rfind ('/');

	// if we found no slashes, check whether the names are identical	
	if (one == string :: npos || two == string :: npos) {
		if (one == two) 
			return 0.0;
		return 1.0;
	}

	// we found a slash in each, so check the directories
	string stringOne = stringOneCast->getVal ().substr (0, one);
	string stringTwo = stringTwoCast->getVal ().substr (0, two);

	if (stringOne == stringTwo)
		return 0.0;
	return 1.0;
}

#endif
