/*
 * PDBData.h
 *
 *  @authors C. Monroy, C. Jermaine
 *  @version 1.0 1/30/2015
 *  Rice University's Pliny Project.
 */

#ifndef PDBDATA_H_
#define PDBDATA_H_

#include "PDBFeatureList.h"
#include "PDBFeatureListBuilder.h"
#include "PDBFeatureType.h"
#include "PDBFeatureTypeDeserializationMachine.h"
#include "PDBStoredDataType.h"
#include "PDBStoredDataTypeDeserializationMachine.h"

// this is nothing more than a tuple that stores a piece of data (such as a program 
// element) and a list of features that have been extracted from that piece of data
class PDBFeatureListBuilder;
class PDBData {

public:

    // sets up an empty object
    PDBData () = default;

    // sets the list of features; the callee owns the object after the call
    void setKey (PDBFeatureList &useMe);
     
    // returns the list of features associated with this bit of data
    PDBFeatureList &getFeatures ();

    // sets the data stored in the object; the callee owns the object after the call
    void setData (PDBStoredDataTypePtr &useMe);

    // returns the actual data (the program element, stored, for example)
    PDBStoredDataTypePtr getData ();

    // serializes this particular PDBData object to the location toHere...
    // the number of bytes that are required to serialize the object is returned via
    // the parameter size, and a pointer to the next byte after the serialized object
    // is returned by the function (that is, toHere + size = returnVal)
    void *serialize(void *toHere, size_t &size);

    // this deserializes this particular PDBData object from the location fromHere.  The number
    // of bytes deserialized is written to the parameter size, and a pointer to the byte
    // after the last one deserialized is returned by the function (so fromHere + size =
    // returnVal).  The deserializaion is performed using the two machines provided.  If an
    // error is encountered (that is, some data or feature cannot be deserialized, then 
    // success is set to false and fromHere is returned
    void *deSerialize(void *fromHere, size_t &size, PDBStoredDataTypeDeserializationMachine &dataDeserializer,
	PDBFeatureTypeDeserializationMachine &featureDeserializer, bool &success);

    // alternative deserialization routine that will not fail in the case that an unknown 
    // data type is encountered.  In this case, exactly size bytes are loaded into a single
    // FlatStoredData type object
    void *deSerialize(void *fromHere, size_t &size, PDBStoredDataTypeDeserializationMachine &dataDeserializer,
	PDBFeatureTypeDeserializationMachine &featureDeserializer);
    
    // this constructor just bundles up a piece of data (a program element, for example)
    // and a list of features
    PDBData (PDBStoredDataTypePtr &dataToStore, PDBFeatureList &extractedFeatures);

    // this constructor produces a piece of data with an empty list of features
    explicit PDBData (PDBStoredDataTypePtr &dataToStore);

    // uses PDBFeatureListBuilder oject to build a list of features; any existing features are retained
    // returns a true if there was an error, and the error message is stored in errMsg
    bool buildFeatures (PDBFeatureListBuilder &useMe, string &errMsg);

    // prints out a representation to a string
    string display ();

private:

    PDBStoredDataTypePtr myStoredData;
    PDBFeatureList mySearchKey;

};

#endif /* PDBDATA_H_ */
