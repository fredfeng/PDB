
#ifndef CHRIS_PDB_H
#define CHRIS_PDB_H

#include "ChrisCatalog.h"
#include <memory>
using namespace std;

// create a smart pointer for SimplePDB objects
class SimplePDB;
typedef shared_ptr <SimplePDB> SimplePDBPtr;

#include "PDBBufferPool.h"
#include "PDBDataHolder.h"
#include "PDBWork.h"
#include <pthread.h>
#include <queue>
using namespace std;


class SimplePDB {

public:

	// this creates an instance of the SimplePDB.
	//
	// numThreads is the number of individual data runs to use, and myBuffer gives us the
	// buffer pages that we are to use
	SimplePDB (int numRuns, PDBBufferPoolPtr myBuffer);

	// opens the catalog at the given location, and then opens the database described
	// by the catalog.  If the catalog does not exist, it is created, and an empty database
	// is created.  Returns a true if there is some sort of error; in this case, errorMsg 
	// is set.
	bool openDB (const string &catalog, string &errorMsg);

	// destructor
	~SimplePDB ();

	// this sets the location that new data are going to be stored to; returns a 
	// true if there was some sort of error encountered... in this case, an error messge
	// is written to errMsg
	bool setStorage (const string &toWhere, string &errMsg);

	// these load up shared library files
	bool registerPDBStoredDataType (const string &libraryFile);
	bool registerPDBFeatureType (const string &libraryFile);
	bool registerPDBMetricType (const string &libraryFile);

	// closes the database
	void closeDB ();

	// allows access to the catalog
	PDBCatalogPtr getCatalog ();

	// causes the database to write back (to disk) all of the in-memory data associated
	// with the specified file... returns a true on error, as well as an error message
	bool writeBackData (int whichFile, string &errorMsg);

	// run the given query over the specified run of data... returns true on an error
	bool runQuery (PDBQueryExecutorPtr useMe, int whichRun, string &errMsg);

	// add the specified features to everyone in the run... returns true on an error
	bool addFeatures (int whichRun, PDBFeatureListBuilder &builders, string &errMsg);

	// stores the specified data in the database... returns true on an error
	bool storeData (vector <PDBData> &dataToAdd, string &errMsg);

	// causes the specified file to be loaded into the in-memory database... returns a
	// true on an error
	bool loadUpFile (const string &fName, string &errMsg);

	// gets the number of storage runs
	int numRuns ();

	// ask how many files are mapped
	int getNumMappedFiles ();

private:

	// sets up all of the internal data structures
	void initialize ();

	// open up the specified files, loading data from all of them into the in-memory database
	bool openFiles (vector <string> fNames, string &errorMsg);

	// this are "constants", or values that are set at startup and should not be messed with
	int NUM_RUNS;
	int WRITE_RUN_LEN;
	int WRITE_BUF_SIZE;
	int MAX_DATA_SIZE;

	// internal methods that allow us to lock the various runs to prevent race conditions
	void getRunReadLock (int whichRun);
	void getRunWriteLock (int whichRun);
	void doneWritingRun (int whichRun);
	void doneReadingRun (int whichRun);

	// maps a file name to a number... used to succinctly encode the file name
	int mapFName (const string &fName);

	// figure out the file name from the equivalent number
	string getMappedName (int whichNum);

	// clear all of the mappings
	void clearMappedFiles ();

	// the number of the currently mapped file
	int curFileMapped;

	// the source of buffer memory
	PDBBufferPoolPtr myBuffers;

	// protects concurrnet access to the mapping info
	pthread_mutex_t mapMutex;

	// stores the mapping
	vector <string> mappedFNames;
	
	// this is the list of vectors that stores all of the database objects in memory
	vector <vector <PDBDataHolder>> allData;

	// this is a list of counters that tell	us the number of readers in each of the 
	// lists in allData
	vector <int> numAllDataReaders;
	vector <pthread_mutex_t> dataMutexes;
	vector <pthread_cond_t> dataVars;

	// mutex to prevent access to the catalog
	pthread_mutex_t catalogMutex;

	// this variable tells us the last run that was written to; we also have a mutex protecting it
	int lastStored;
	pthread_mutex_t lastStoredMutex;

	// this is the FD for the current output file
	int outputFD;

	// and the mutex that makes sure the multiple writers are not writing to this file at the same time
	pthread_mutex_t storageMutex;

	// the catalog
	PDBCatalogPtr catalog;
}; 

#endif
