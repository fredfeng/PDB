
#ifndef PDBQUERYRES_H_
#define PDBQUERYRES_H_

#include "PDBData.h"
#include <memory>
#include <string>
#include <vector>
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

private:
	
	PDBData myData;
	double myDistance;

};

#endif /* PDBTOPKQUERY_H_ */
