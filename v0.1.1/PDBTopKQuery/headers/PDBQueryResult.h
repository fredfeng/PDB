
#ifndef PDBQUERYRES_H_
#define PDBQUERYRES_H_

#include <string>
#include <vector>
#include <memory>
#include "PDBData.h"
using namespace std;

// this class stores a PDBData, distance pair
class PDBQueryResult {

public:
	
	// returns a reference to the data in here
	PDBData &getData ();

	// returns the "goodness" of the answer
	double getDistanceToQuery () const;

	// standard constructor
	PDBQueryResult (PDBData storeMe, double useThisDistance);

	// copy from another object
	PDBQueryResult (const PDBQueryResult &fromMe);

	// assign from another object
	PDBQueryResult &operator = (const PDBQueryResult &fromMe);	

private:
	
	PDBData myData;
	double myDistance;

};

#endif /* PDBTOPKQUERY_H_ */

	

	
