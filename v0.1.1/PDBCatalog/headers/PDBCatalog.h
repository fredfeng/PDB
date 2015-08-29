
#ifndef PDBCATALOG_H_
#define PDBCATALOG_H_

#include <string>
#include <vector>
#include <memory>
#include "PDBFeatureTypeDeserializationMachine.h"
#include "PDBStoredDataTypeDeserializationMachine.h"
#include "PDBMetric.h"

using namespace std;

// create a smart pointer type for PDBStoredDataType objects
class PDBCatalog;
typedef shared_ptr <PDBCatalog> PDBCatalogPtr;

class PDBCatalog {

public:

	// opens up a catalog file locate at the specified location... creates it if there
	// is not already one there
	virtual void open (string whereLocated) = 0;

	// checks whether the named PDBMetric is registered (checkMe is the name of the metric)
	virtual bool isMetricRegistered (string checkMe) = 0;

	// registers the metric whose .so file is located at the specified location.  This
	// basically amounts to going to that file, loading up the library, creating a metric
	// of the type whose code is stored in the file (in order to obtain info about it)
	// and then actually storing the shared library in the catalog for future use.  A
	// true is returned if this is successful, and a false if not.
	virtual bool registerMetric (string sharedLibraryFile) = 0;

	// checks whether the named PDBFeatureType is registered (checkMe is the name of the
	// feature tpye)
	virtual bool isFeatureTypeRegistered (string checkMe) = 0;

	// registers the given PDBFeatureType... similar to registerMetric
	virtual bool registerFeatureType (string sharedLibraryFile) = 0;

	// checks to see whether the named PDBStoredDataType has been registered (as above,
	// checkMe is the name of the PDBStoredDataType to check for)
	virtual bool isStoredDataTypeRegistered (string checkMe) = 0;

	// registers the given PDBStoredDataType... similar to registerMetic/registerFeatureType
	virtual bool registerStoredDataType (string sharedLibraryFile) = 0;

	// returns an instance of the specified metric
	virtual PDBMetricPtr getMetric (string retreiveMe) = 0;

	// returns a deserialization machine for the various PDBFeatureTypes that have been registered
	virtual PDBFeatureTypeDeserializationMachine &getFeatureTypeDeserializer () = 0;
	
	// returns a deserialization machine for the PDBStoredDataTypes that have been registered
	virtual PDBStoredDataTypeDeserializationMachine &getStoredDataTypeDeserializer () = 0;

	// returns the locations of all of the files/network locations where data are stored
	virtual vector <string> getStorageLocations () = 0;

	// adds a new storage location into the catalog
	virtual void addNewStorageLocation (string addMe) = 0;

	// returns a list of all of the names of the PDBStoredDataTypes that are in the catalog
	virtual vector <string> getRegisteredPDBStoredDataTypes () = 0;

	// similar function for the PDBFeatureTypes and PDBMetrics
	virtual vector <string> getRegisteredPDBFeatureTypes () = 0;
	virtual vector <string> getRegisteredPDBMetrics () = 0;
	
	// unregister PDBStoredDataTypes, PDBFeatureTypes, PDBMetrics
	virtual void unregisterPDBStoredDataType (string unregisterMe) = 0;
	virtual void unregisterPDBFeatureType (string unregisterMe) = 0;
	virtual void unregsiterPDBMetric (string unregisterMe) = 0;
};

#endif
