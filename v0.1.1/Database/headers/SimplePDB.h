
#ifndef CHRIS_PDB_H
#define CHRIS_PDB_H

#include "PDB.h"
#include "ChrisCatalog.h"

class SimplePDB : public PDB {

public:

	// this creates an instance of the SimplePDB, loading up the catalog at the 
	// given location (or, creating it if it does not already exist).
	// writeBufferSize is (obviously) the size of the write buffer that is used,
	// and maxPBDDataSize is the max expected size (in bytes) of any particular PDBData
	// object when it is serialized; writeBufferSize should be larger than 
	SimplePDB (string catalog, size_t writeBufferSize, size_t maxPBDDataSize);

	// similar to the last one; the is first closed, and then re-opened using
	// the specified catalog
	void openDB (string catalog);

	// destructor
	~SimplePDB ();

	// this sets the location that new data are going to be stored to; returns a 
	// true if we are able to write to that location, and a false otherwise.  
	// maxPBDDataSize
	bool setStorage (string toWhere);

	/**********************/
	/* INHERITED FROM PDB */
	bool store (PDBData &storeMe);
	bool regsiterPDBStoredDataType (string libraryFile);
	bool regsiterPDBFeatureType (string libraryFile);
	bool regsiterPDBMetricType (string libraryFile);
	void closeDB ();
	PDBCatalogPtr getCatalog ();
	bool buildFeatures (PDBFeatureListBuilder &useMe);
	PDBQueryExecutorPtr runQuery (PDBQueryExecutorPtr useMe);
	/**********************/

private:

	// loads the contents of the file into RAM
	void loadUpFile (string fName);

	// private method used to clear the buffer
	void clearBuffer ();

	// this silly implementation just has a big huge array of all of the stored
	// PDBData objects
	vector <PDBData> storedObjects;

	// here is the catalog
	PDBCatalogPtr catalog;

	// this is where we are currently writing data to
	int dataFD;

	// the name of the file we are currently writing to
	string curFile; 

	// and here is the buffer and buffer size infomation for writing
	size_t writeBufferSize;
	size_t maxPBDDataSize;
	char *writeBuffer;
	char *writeBufferStart;
}; 

#endif
