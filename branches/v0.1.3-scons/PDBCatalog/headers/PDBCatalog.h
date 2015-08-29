
#ifndef PDBCATALOG_H_
#define PDBCATALOG_H_

#include "PDBFeatureTypeDeserializationMachine.h"
#include "PDBMetric.h"
#include "PDBStoredDataTypeDeserializationMachine.h"
#include "PDBQueryExecutorDeserializationMachine.h"
#include "PDBFeatureBuilderDeserializationMachine.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

// create a smart pointer type for PDBStoredDataType objects
class PDBCatalog;
typedef shared_ptr <PDBCatalog> PDBCatalogPtr;

class PDBCatalog {

public:

	// opens up a catalog file locate at the specified location... creates it if there
	// is not already one there
	virtual void open (const string &whereLocated) = 0;

	// checks whether the named PDBMetric is registered (checkMe is the name of the metric)
	virtual bool isMetricRegistered (const string &checkMe) = 0;

	// registers the metric whose .so file is located at the specified location.  This
	// basically amounts to going to that file, loading up the library, creating a metric
	// of the type whose code is stored in the file (in order to obtain info about it)
	// and then actually storing the shared library in the catalog for future use.  A
	// true is returned if this is successful, and a false if not.
	virtual bool registerMetric (const string &sharedLibraryFile) = 0;

	// checks whether the named PDBFeatureType is registered (checkMe is the name of the
	// feature tpye)
	virtual bool isFeatureTypeRegistered (const string &checkMe) = 0;

	// registers the given PDBFeatureType... similar to registerMetric
	virtual bool registerFeatureType (const string &sharedLibraryFile) = 0;

	// checks to see whether the named PDBStoredDataType has been registered (as above,
	// checkMe is the name of the PDBStoredDataType to check for)
	virtual bool isStoredDataTypeRegistered (const string &checkMe) = 0;

	// registers the given PDBStoredDataType... similar to registerMetic/registerFeatureType
	virtual bool registerStoredDataType (const string &sharedLibraryFile) = 0;

	// returns an instance of the specified metric
	virtual PDBMetricPtr getMetric (const string &retreiveMe) = 0;

	// returns a deserialization machine for the various PDBFeatureTypes that have been registered
	virtual PDBFeatureTypeDeserializationMachine &getFeatureTypeDeserializer () = 0;
	
	// returns a deserialization machine for the PDBStoredDataTypes that have been registered
	virtual PDBStoredDataTypeDeserializationMachine &getStoredDataTypeDeserializer () = 0;

	// returns a deserialization machine for queries
	virtual PDBQueryExecutorDeserializationMachine &getQueryExecutorDeserializer () = 0;

	// returns a deserialization machine for feature builders
	virtual PDBFeatureBuilderDeserializationMachine &getFeatureBuilderDeserializationMachine () = 0;

	// returns the locations of all of the files/network locations where data are stored
	virtual vector <string> getStorageLocations () = 0;

	// adds a new storage location into the catalog
	virtual void addNewStorageLocation (const string &addMe) = 0;

	// returns a list of all of the names of the PDBStoredDataTypes that are in the catalog
	virtual vector <string> getRegisteredPDBStoredDataTypes () = 0;

	// similar function for the PDBFeatureTypes and PDBMetrics
	virtual vector <string> getRegisteredPDBFeatureTypes () = 0;
	virtual vector <string> getRegisteredPDBMetrics () = 0;
	
	// unregister PDBStoredDataTypes, PDBFeatureTypes, PDBMetrics
	virtual void unregisterPDBStoredDataType (const string &unregisterMe) = 0;
	virtual void unregisterPDBFeatureType (const string &unregisterMe) = 0;
	virtual void unregsiterPDBMetric (const string &unregisterMe) = 0;

	// destructor
	virtual ~PDBCatalog () = default;
};

#endif
