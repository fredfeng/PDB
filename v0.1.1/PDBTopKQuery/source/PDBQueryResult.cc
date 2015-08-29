
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

PDBQueryResult :: PDBQueryResult (const PDBQueryResult &fromMe) {
	myData = fromMe.myData;
	myDistance = fromMe.myDistance;
}

PDBQueryResult &PDBQueryResult :: operator = (const PDBQueryResult &fromMe) {
	myData = fromMe.myData;
	myDistance = fromMe.myDistance;
	return *this;
}

#endif 

	

	
