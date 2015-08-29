
#ifndef PDB_H_
#define PDB_H_

#include "PDBCatalog.h"
#include "PDBData.h"
#include "PDBFeatureListBuilder.h"
#include "PDBQueryExecutor.h"
#include <vector>
using namespace std;

class PDB {

public:

	// registers a new PDBStoredDataType... libraryFile is the path of
	// the library file which contains the implementation of the PDBStoredDataType
	// that is to be registered... returns true on success, false on failure
	virtual bool registerPDBStoredDataType (const string &libraryFile) = 0;

	// similar methods for PDBFeatureType, PDBMetric
	virtual bool registerPDBFeatureType (const string &libraryFile) = 0;
	virtual bool registerPDBMetricType (const string &libraryFile) = 0;

	// close the database... this is called automatically at object destruction
	virtual void closeDB () = 0;

	// gets access to the catalog
	virtual PDBCatalogPtr getCatalog () = 0;

	/*****************************************/
	// These four methods constitute the API for manipulating the database
	// data, as well as querying it

	// stores the specified PDBData object; returns true if there was an error
	virtual bool store (PDBData &storeMe, string &errMsg) = 0;
	virtual bool store (vector <PDBData> &dataToAdd, string &errMsg) = 0;

	// runs a query over the database and returns the result. Any errors will
	// be noted in the returned PDBQueryExecutor object (PDBQueryExecutor.wasAnError ()
	// will return true).  Note that the PDBQueryExecutor object returned will always
	// be the same type as the PDBQueryExecutor object that is sent in, and will 
	// contain the query answer
	virtual PDBQueryExecutorPtr runQuery (PDBQueryExecutorPtr useMe, bool &wasError, string &errorMsg) = 0;

	// adds new features to all of the items in the database, using the specification
	// embedded in the object useMe.  Returns true if there was an error
	virtual bool buildFeatures (PDBFeatureListBuilder &useMe, string &errorMsg) = 0;

	// destructor
	virtual ~PDB () = default;
};

#endif /* PDB_H_ */

