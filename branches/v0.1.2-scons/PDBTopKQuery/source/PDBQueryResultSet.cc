
#ifndef PDBQUERYRES_SET_C_
#define PDBQUERYRES_SET_C_

#include "PDBQueryResultSet.h"
using namespace std;

bool PDBQueryResultSet :: wasSuccessful () {
	return success;
}

const string &PDBQueryResultSet :: getErrorMessage () {
	return message;
}

PDBQueryResultSet :: PDBQueryResultSet () {
	success = true;
	numChecked = 0;
	numIgnored = 0;
}

vector <PDBQueryResult> PDBQueryResultSet :: getResults () {
	return results;
}

int PDBQueryResultSet :: getNumberDatabaseItemsChecked () {
	return numChecked;
}

int PDBQueryResultSet :: getNumberDatabaseItemsIgnored () {
	return numIgnored;
}

void PDBQueryResultSet :: addNewQueryResult (PDBQueryResult &addMe) {
	results.push_back (addMe);
}

void PDBQueryResultSet :: wasAnError (const string &errorMessage) {
	success = false;
	message = errorMessage;
}

void PDBQueryResultSet :: setCounts (int numDBItemsChecked, int numDBItemsIgnored) {
	numChecked = numDBItemsChecked;
	numIgnored = numDBItemsIgnored;
}

#endif 

	

	
