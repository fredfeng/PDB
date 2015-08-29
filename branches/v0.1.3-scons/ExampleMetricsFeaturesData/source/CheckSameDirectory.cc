
#ifndef CHECK_SAME_DIR_C
#define CHECK_SAME_DIR_C

#include "CheckSameDirectory.h"
#include "TextFeatureType.h"
#include <iostream>
#include <string>
using namespace std;

CheckSameDirectory :: CheckSameDirectory () {
	gotErr = false;
}

const string &CheckSameDirectory :: getName () {
	static const string result {"CheckSameDirectory"};
	return result;
}

int CheckSameDirectory :: getNumParams () {
	return 2;
}

void CheckSameDirectory :: takeParam (int whichParam, PDBFeatureTypePtr param) {

	if (param->getCode () != 34958343) {
		errMsg = "You gave CheckSameDirectory a param of type " + param->getTypeName ();
		gotErr = true;
		return;
	}

	if (whichParam == 0) {
		stringOne = param;
		return;
	}
	if (whichParam == 1) {
		stringTwo = param;
		return;
	}

	errMsg = "Bad parameter number to CheckSameDirectory!";
	gotErr = true;
}

const string &CheckSameDirectory :: getParamType (int) {
	static const string result {"TextFeatureType"};
	return result;
}

void CheckSameDirectory :: reset () {
	PDBFeatureTypePtr temp;
	stringOne = temp;
	stringTwo = temp;
	gotErr = false;
}

double CheckSameDirectory :: apply (bool &wasError, string &errorMessage) {

	if (gotErr) {
		wasError = true;
		errorMessage = errMsg;
	}

	if (!stringOne || !stringTwo) {
		wasError = true;
		errorMessage = "One of the parameters to CheckSameDirectory was null";
		return 9e99;
	}

        TextFeatureType *stringOneCast = dynamic_cast<TextFeatureType *> (stringOne.get ());
        TextFeatureType *stringTwoCast = dynamic_cast<TextFeatureType *> (stringTwo.get ());

	size_t one = stringOneCast->getVal ().rfind ('/');
	size_t two = stringTwoCast->getVal ().rfind ('/');

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

PDBMetricPtr CheckSameDirectory :: getCopy () {
	return make_shared<CheckSameDirectory>();
}

#endif
