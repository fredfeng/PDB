
#ifndef EUCLIDEAN_C
#define EUCLIDEAN_C

#include <string>
#include <iostream>
#include "EuclideanDistance.h"
#include "SparseVectorFeatureType.h"
using namespace std;

string EuclideanDistance :: getName () {
	return "EuclideanDistance";
}

int EuclideanDistance :: getNumParams () {
	return 2;
}

void EuclideanDistance :: takeParam (int whichParam, PDBFeatureTypePtr param) {

	if (param->getCode () != 5923458) {
		cerr << "You gave EuclideanDistance a param of type " << param->getTypeName () << "\n";
		exit (1);
	}

	if (whichParam == 0) {
		vecOne = param;
		return;
	}
	if (whichParam == 1) {
		vecTwo = param;
		return;
	}
	cerr << "Bad parameter number to EuclideanDistance!";
	exit (1);
}

string EuclideanDistance :: getParamType (int whichParam) {
	return "SparseVectorFeatureType";
}

void EuclideanDistance :: reset () {
	PDBFeatureTypePtr temp;
	vecOne = temp;
	vecTwo = temp;
}

double EuclideanDistance :: apply (bool &wasError, string &errorMessage) {
	if (!vecOne || !vecTwo) {
		wasError = true;
		errorMessage = "One of the parameters to EuclideanDistance was null";
		return 9e99;
	}
	
	SparseVectorFeatureType *vecOneCast = (SparseVectorFeatureType *) vecOne.get ();
	SparseVectorFeatureType *vecTwoCast = (SparseVectorFeatureType *) vecTwo.get ();

	double tot = 0.0;
	wasError = false;
	for (int i : vecOneCast->getNonZeroSlots ()) {
		double first = vecOneCast->getValue (i);
		double second = vecTwoCast->getValue (i);
		tot += (first - second) * (first - second);
	}
	return tot;
}

#endif
