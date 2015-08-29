
#ifndef PDBQUERYRES_SET_H_
#define PDBQUERYRES_SET_H_

#include "PDBQueryResult.h"
#include <memory>
#include <string>
#include <vector>
using namespace std;

// this simple class encapsulates the idea of a query result
class PDBQueryResultSet {

public:

	// returns true if the query was successfully executed
	bool wasSuccessful ();

	// if the query was not successfully executed, this returns an 
	// appropriate error message
	const string &getErrorMessage ();

	// an empty set
	PDBQueryResultSet ();

	// returns a vector containing the query results
	vector <PDBQueryResult> getResults ();

	// tells how many data items were checked to obtain the query result
	int getNumberDatabaseItemsChecked ();

	// tells how many data items were ignored because they did not match
	// the query specification (one more more required attributes could not
	// be found in the data item)
	int getNumberDatabaseItemsIgnored ();

	//*********************************************************//
	// The rest of these functions should not be used outside 
	// of the PDB implementation!!!
	//*********************************************************//
	//
	// add a new query result... 
	void addNewQueryResult (PDBQueryResult &addMe);

	// note that there was an error
	void wasAnError (const string &errorMessage);

	// sets the number of items checked and ignored
	void setCounts (int numDBItemsChecked, int numDBItemsIgnored);

private:

	bool success;
	string message;
	int numChecked;
	int numIgnored;
	vector <PDBQueryResult> results;
};

#endif /* PDBTOPKQUERY_H_ */

	

	
