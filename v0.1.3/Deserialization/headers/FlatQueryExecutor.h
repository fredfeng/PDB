
#ifndef FLAT_QUERY_H
#define FLAT_QUERY_H

#include "PDBQueryExecutor.h"
using namespace std;

class FlatQueryExecutor;
typedef shared_ptr <FlatQueryExecutor> FlatQueryExecutorPtr;

// this class is used as a temporary query executor that exists only temporarily, when
// we do not want to actually unflatten a query executor... you use this when you 
// want to create a transient object that just exists long enough to re-serializa it
class FlatQueryExecutor : public PDBQueryExecutor {

public:

	// constructor
	FlatQueryExecutor ();

	// destructor
	~FlatQueryExecutor ();

	// these are inherited from PDBQueryExecutor
	PDBQueryExecutor &operator += (PDBQueryExecutorPtr rhs) override;
	PDBQueryExecutorPtr getEmpty () override;
	void aggregate (PDBData &aggMe) override;
	int getCode () override;
	string display () override;
	void *serialize (void *toHere, size_t &numBytes) override;
	void *deSerialize (void *fromHere, size_t &numBytes) override;
	bool wasAnError (string &error) override;

private:

	char *myBytes;
	size_t mySize;
};

#endif

