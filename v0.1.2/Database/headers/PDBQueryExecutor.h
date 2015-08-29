
#ifndef PDB_QUERY_H
#define PDB_QUERY_H

#include "PDBData.h"
#include <memory>
using namespace std;

// create a smart pointer for PDBQueryExecutor objects
class PDBQueryExecutor;
typedef shared_ptr <PDBQueryExecutor> PDBQueryExecutorPtr;

// this is a virtual base class that lets us run queries over a database... the
// idea is that you write a class that is derived from PDBQueryExecutor, and then
// ask the database to execute it... PDBQueryExecutor.aggregate () will be called
// over every object in the database
class PDBQueryExecutor {

public:

	// this takes the contents of another PDBQueryExecutor and adds it to this
	// particular object.  For example, imagine that I am summing over a
	// set of integers.  My PDBQueryExecutor might wrap up a single integer.
	// when += is called, it should take the integer inside of rhs and add
	// it to this one.  Note that the PDB instance will manage PDBQueryExecutor
	// objects in such a way that it will NEVER try to add together two
	// PDBQueryExecutor objects that are not of the same concrete type
	virtual PDBQueryExecutor &operator += (PDBQueryExecutorPtr rhs) = 0;

	// gets a PDBQueryExecutor that is empty or has the equivalent of a "zero"
	// value; adding the result this call to another PDBQueryExecutor object
	// should not change the value of the contents of the object
	virtual PDBQueryExecutorPtr getEmpty () = 0;

	// accepts an input database object and (possibly) adds it to the query result
	virtual void aggregate (PDBData &queryMe) = 0;

	// display the contents (or at least a subset of them)
	virtual string display () = 0;

	// check if there was an error (if there was, the error string is set)
	virtual bool wasAnError (string &error) = 0;

	// serializes this particular object to the location toHere...
	// the number of bytes that are required to serialize the object is returned via
	// the parameter size, and a pointer to the next byte after the serialized object
	// is returned by the function (that is, toHere + size = returnVal)
	virtual void *serialize(void *toHere, size_t &size) = 0;

	// this deserializes this particular object from the location fromHere.  The number
	// of bytes deserialized is written to the parameter size, and a pointer to the byte
	// after the last one deserialized is returned by the function (so fromHere + size =
	// returnVal).  
	virtual void *deSerialize(void *fromHere, size_t &size) = 0;

	// destructor
	virtual ~PDBQueryExecutor () = default;
};

#endif
