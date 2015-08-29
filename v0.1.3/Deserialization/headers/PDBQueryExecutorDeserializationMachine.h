
#ifndef PDB_QUERY_DESER_H_
#define PDB_QUERY_DESER_H_

#include "PDBQueryExecutor.h"
#include <vector>
using namespace std;

// this is a factory that is used to deserialize queries
class PDBQueryExecutorDeserializationMachine {

public:

	// deserializes the next PDBQueryExecutor object... the number of bytes
	// processed is put into numBytes... note that in the case that the tpye
	// of the query encountered does not match any registered type, then
	// an object of type PDBUnknownQueryExecutorType is created and returned, and
	// exactly numBytes bytes from fromHere are used to create it
	PDBQueryExecutorPtr deSerialize (void *fromHere, size_t &numBytes, bool &success);

        // deserializes, but will not fail if a matching type code for the object
        // to be deserialized cannot be found... in this case, the object is deserialized
        // into a generic, flat object that cannot be used to do anything, but it can
        // be re-serialized.  This is used in the case that we are simply forwarding the
        // object on to someone else, and we actually don't care about doing anything more
        // than storing his bytes into an object before we re-send them over a wire
	PDBQueryExecutorPtr deSerialize (void *fromHere, size_t &numBytes);

	// constructor... accepts a list of all of the different query types that have
	// been registered in the catalog
	explicit PDBQueryExecutorDeserializationMachine (vector <PDBQueryExecutorPtr> &allQueryTypes);

	// register another query type
	void registerNewQueryExecutor (PDBQueryExecutorPtr regMe);

	PDBQueryExecutorDeserializationMachine () = default;

private:

	vector <PDBQueryExecutorPtr> allRegisteredTypes;
	
};

#endif
