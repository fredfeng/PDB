
#ifndef FLAT_DATA_H
#define FLAT_DATA_H

#include "PDBStoredDataType.h"
using namespace std;

class FlatStoredData;
typedef shared_ptr <FlatStoredData> FlatStoredDataPtr;

// this class is used as a temporary data object that exists only temporarily, when
// we do not want to actually unflatten a data object... you use this when you 
// want to create a transient object that just exists long enough to re-serialize it
class FlatStoredData : public PDBStoredDataType {

public:

	// constructor, destructor
	FlatStoredData ();
	~FlatStoredData ();

	// these are inherited from PDBStoredDataType
	const string &getName() override;
	int getCode () override;
	void *serialize (void *toHere, size_t &numBytes) override;
	void *deSerialize (void *fromHere, size_t &numBytes) override;
	PDBStoredDataTypePtr getNewStoredDataTypeObject () override;
	string display () override;

private:

	char *myBytes;
	size_t mySize;
	string myName;
};

#endif

