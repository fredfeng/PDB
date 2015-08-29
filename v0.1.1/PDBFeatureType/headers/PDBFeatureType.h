
#ifndef PDBFEATURETYPE_H_
#define PDBFEATURETYPE_H_

#include <memory>
#include <string>
using namespace std;

// create a smart pointer type for PDBStoredDataType objects
class PDBFeatureType;
typedef shared_ptr <PDBFeatureType> PDBFeatureTypePtr;

class PDBFeatureType {
public:

    // returns the name of this feature type (graph, bag-of-words, etc.)
    virtual string getTypeName () = 0;

    // returns an int that provides a unique code for this feature type... if two
    // PDBFeatureTypes are registered to the catalog that have the same code, an error
    // will be returned, so these need to be unique
    virtual int getCode () = 0;

    // serializes an instance of this particular feature type to the location toHere... 
    // the number of bytes that are required to serialize the object is returned via
    // the parameter size, and a pointer to the next byte after the serialized object
    // is returned by the function (that is, toHere + size = returnVal)
    virtual void *serialize (void *toHere, size_t &size) = 0;

    // this deserializes this particular feature from the location fromHere.  The number
    // of bytes deserialized is written to the parameter size, and a pointer to the byte
    // after the last one deserialized is returned by the function (so fromHere + size =
    // returnVal)
    virtual void *deSerialize (void *fromHere, size_t &size) = 0;

    // returns an empty PDBFeatureType of the same type as this one... typically this
    // will be used when we are deserializing
    virtual PDBFeatureTypePtr getNewFeatureTypeObject () = 0;

    // returns a text summary of the feature
    virtual string display () = 0;
};

#endif /* PDBFEATURETYPE_H_ */
