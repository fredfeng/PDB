
#ifndef PDBSTOREDDATA_H_
#define PDBSTOREDDATA_H_

#include <memory>
#include <string>
using namespace std;

// create a smart pointer type for PDBStoredDataType objects
class PDBStoredDataType;
typedef shared_ptr <PDBStoredDataType> PDBStoredDataTypePtr;

// this encapsualtes a piece of data that is stored in PDB.  This might be something like
// a module, or a program element.  Note that this is a pure virtual base class.
class PDBStoredDataType {

public:

	// returns the name of this data type (module, program element version 1, etc.)
	virtual string getName() = 0;

	// returns an int that provides a unique code for this stored data type... if two
	// PDBStoredDatas are registered to the catalog that have the same code, an error
	// will be returned, so these need to be unique
	virtual int getCode () = 0;

	// serializes an instance of this particular data type to the location toHere...
	// the number of bytes that are required to serialize the object is returned via
	// the parameter size, and a pointer to the next byte after the serialized object
	// is returned by the function (that is, toHere + size = returnVal)
	virtual void *serialize (void *toHere, size_t &size) = 0;

	// this deserializes this particular data from the location fromHere.  The number
	// of bytes deserialized is written to the parameter size, and a pointer to the byte
	// after the last one deserialized is returned by the function (so fromHere + size =
	// returnVal)
	virtual void *deSerialize (void *fromHere, size_t &size) = 0;

	// returns an empty PDBStoredDataType of the same type as this one... typically this
	// will be used when we are deserializing
	virtual PDBStoredDataTypePtr getNewStoredDataTypeObject () = 0;

	// prints out a represetation of this guy to a string
	virtual string display () = 0;
};

#endif /* PDBSTOREDDATA_H_ */

