
#ifndef DATA_HOLDER_H
#define DATA_HOLDER_H

#include "PDBData.h"

// this little class is used within the database to store <PDBStoredData, file> pairs
class PDBDataHolder {

public:

	PDBData &getData () {
		return storeMe;
	}

	int getFile () {
		return whichFile;
	}

	PDBDataHolder (PDBData storeMeIn, int whichFileIn) {
		storeMe = storeMeIn;
		whichFile = whichFileIn;
	}

private:

	PDBData storeMe;
	int whichFile;
};

#endif
