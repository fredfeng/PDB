
#ifndef PDBQUERYRES_SET_C_
#define PDBQUERYRES_SET_C_

#include "PDBQueryResultSet.h"
using namespace std;

bool PDBQueryResultSet :: wasSuccessful () {
	return success;
}

string PDBQueryResultSet :: getErrorMessage () {
	return message;
}

PDBQueryResultSet :: PDBQueryResultSet (const PDBQueryResultSet &fromMe) {
	success = fromMe.success;
	message = fromMe.message;
	numChecked = fromMe.numChecked;
	results = fromMe.results;
	numIgnored = fromMe.numIgnored;
}

PDBQueryResultSet :: PDBQueryResultSet () {
	success = true;
	message = "";
	numChecked = 0;
	numIgnored = 0;
}

PDBQueryResultSet &PDBQueryResultSet :: operator = (const PDBQueryResultSet &fromMe) {
	success = fromMe.success;
	message = fromMe.message;
	numChecked = fromMe.numChecked;
	results = fromMe.results;
	numIgnored = fromMe.numIgnored;
	return *this;
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

void PDBQueryResultSet :: wasAnError (string errorMessage) {
	success = false;
	message = errorMessage;
}

void PDBQueryResultSet :: setCounts (int numDBItemsChecked, int numDBItemsIgnored) {
	numChecked = numDBItemsChecked;
	numIgnored = numDBItemsIgnored;
}

#endif 

	

	
