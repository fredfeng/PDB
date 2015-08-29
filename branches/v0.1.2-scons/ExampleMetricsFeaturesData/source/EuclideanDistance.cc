
#ifndef EUCLIDEAN_C
#define EUCLIDEAN_C

#include "EuclideanDistance.h"
#include "SparseVectorFeatureType.h"
#include <iostream>
#include <string>
using namespace std;

EuclideanDistance :: EuclideanDistance () {
	gotError = false;
}

const string &EuclideanDistance :: getName () {
	static const string result {"EuclideanDistance"};
	return result;
}

int EuclideanDistance :: getNumParams () {
	return 2;
}

void EuclideanDistance :: takeParam (int whichParam, PDBFeatureTypePtr param) {

	if (param->getCode () != 5923458) {
		gotError = true;
		errorMsg = "You gave EuclideanDistance a param of type " + param->getTypeName ();
		return;
	}

	if (whichParam == 0) {
		vecOne = param;
		return;
	}
	if (whichParam == 1) {
		vecTwo = param;
		return;
	}

	gotError = true;
	errorMsg = "Bad parameter number to EuclideanDistance!";
}

const string &EuclideanDistance :: getParamType (int) {
	static const string result {"SparseVectorFeatureType"};
	return result;
}

void EuclideanDistance :: reset () {
	PDBFeatureTypePtr temp;
	vecOne = temp;
	vecTwo = temp;
	gotError = false;
}

double EuclideanDistance :: apply (bool &wasError, string &errorMessage) {

	if (gotError) {
		wasError = true;
		errorMessage = errorMsg;
		return 9e99;
	}

	if (!vecOne || !vecTwo) {
		wasError = true;
		errorMessage = "One of the parameters to EuclideanDistance was null";
		return 9e99;
	}
	
	SparseVectorFeatureType *vecOneCast = dynamic_cast<SparseVectorFeatureType *> (vecOne.get ());
	SparseVectorFeatureType *vecTwoCast = dynamic_cast<SparseVectorFeatureType *> (vecTwo.get ());

	double tot = 0.0;
	wasError = false;
	for (int i : vecOneCast->getNonZeroSlots ()) {
		double first = vecOneCast->getValue (i);
		double second = vecTwoCast->getValue (i);
		tot += (first - second) * (first - second);
	}
	return tot;
}

PDBMetricPtr EuclideanDistance :: getCopy () {
	return make_shared<EuclideanDistance>();
}

#endif
