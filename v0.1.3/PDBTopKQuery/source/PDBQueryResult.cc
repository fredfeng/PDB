
#ifndef PDBQUERYRES_C_
#define PDBQUERYRES_C_

#include "PDBQueryResult.h"

PDBData &PDBQueryResult :: getData () {
	return myData;
}

double PDBQueryResult :: getDistanceToQuery () const {
	return myDistance;
}

PDBQueryResult :: PDBQueryResult (PDBData storeMe, double useThisDistance) {
	myData = storeMe;
	myDistance = useThisDistance;
}

#endif 

	

	
