
#ifndef CHRIS_PDB_C
#define CHRIS_PDB_C

#include "LockGuard.h"
#include "PDBFeatureBuilder.h"
#include "PDBQueryExecutor.h"
#include "SimplePDB.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;


SimplePDB :: ~SimplePDB () {

	// free all of the mutexes and condition variables
	for (auto &mutex : dataMutexes)
		pthread_mutex_destroy (&mutex);
	for (auto &cond : dataVars)
		pthread_cond_destroy (&cond);
	pthread_mutex_destroy (&catalogMutex);
	pthread_mutex_destroy (&lastStoredMutex);
	pthread_mutex_destroy (&mapMutex);
	pthread_mutex_destroy (&storageMutex);
}

void SimplePDB :: initialize () {

	// now the ones protecting the data runs
	dataMutexes.resize (NUM_RUNS);
	for (auto &mutex : dataMutexes)
		pthread_mutex_init (&mutex, nullptr);
	dataVars.resize (NUM_RUNS);
	for (auto &cond : dataVars)
		pthread_cond_init (&cond, nullptr);
	
	// and add the counters in
	numAllDataReaders.resize (NUM_RUNS, 0);

	// now the mutex protecting the catalog
	pthread_mutex_init (&catalogMutex, nullptr);

	// and the one protectin the output file
	pthread_mutex_init (&storageMutex, nullptr);

	// and the one protecting the map of file names to numbers
	pthread_mutex_init (&mapMutex, nullptr);

	// and the one protecting the variable that remembers the last run written to
	pthread_mutex_init (&lastStoredMutex, nullptr);

	// last stored is zero
	lastStored = 0;

	// now, set up the data runs
	allData.resize (NUM_RUNS);

	outputFD = -1;
}

void SimplePDB :: closeDB () {
	
	// go through and delete all of the objects buffered in RAM
	for (int i = 0; i < NUM_RUNS; i++) {
		getRunWriteLock (i);
		allData[i].clear ();
		doneWritingRun (i);
	}

	// no file is current
	curFileMapped = -1;

	// and we have no mapped files
	clearMappedFiles ();

	if (outputFD != -1)
		close (outputFD);
	outputFD = -1;
}

bool SimplePDB :: openDB (const string &catalogLocation, string &errorMsg) {

	// close up first 
	closeDB ();

	// open up the catalog
	PDBCatalogPtr temp {make_shared<ChrisCatalog> ()};
	catalog = temp;
	catalog->open (catalogLocation);
	errorMsg = "";
	return false;
}
	
bool SimplePDB :: addFeatures (int whichRun, PDBFeatureListBuilder &builders, string &errMsg) {

	// add the features
	getRunWriteLock (whichRun);
	for (auto &elem : allData[whichRun]) {
		
		// loop through each of the indiviaul feature builders
		for (PDBFeatureBuilderPtr p : builders.getFeatureBuilders ()) {

			// if this guy runs on this data object, then add the new feature
			if (p->runsOnThisPDBData (elem.getData ())) {
				bool wasError;
				PDBFeatureTypePtr newFeature = p->buildFeature (elem.getData (), wasError, errMsg);
				if (wasError)
					return true;
				elem.getData ().getFeatures ().addFeature (newFeature, p->getFeatureName ());
			}
		}
	}
	doneWritingRun (whichRun);
	return false;
}

bool SimplePDB :: storeData (vector <PDBData> &dataToAdd, string &errorMsg) {

	// figure out which run to write to, and estimate the number of runs to write
	int curRun;
	{
		const LockGuard guard {lastStoredMutex};
		curRun = lastStored;
		int numRunsToWrite = dataToAdd.size () / WRITE_RUN_LEN;
		if (numRunsToWrite == 0)
			numRunsToWrite++;
		lastStored += numRunsToWrite;
		lastStored %= NUM_RUNS;
	}

	// get a buffer
	PDBBufferPagePtr myBuf {myBuffers->get ()};
	char *curPos {myBuf->getRaw ()};

	// now do the writing to the runs
	bool wasAnError = false;
	getRunWriteLock (curRun);	
	for (size_t i = 0; i < dataToAdd.size (); i++) {
		
		// make sure we have a write lock on the run
		if (i % WRITE_RUN_LEN == 0 && i != 0) {
			doneWritingRun (curRun);
			curRun = (curRun + 1) % NUM_RUNS;
			getRunWriteLock (curRun);
		}	

		// write to the run
		allData[curRun].emplace_back (dataToAdd[i], curFileMapped);

		// and write to the buffer
		if (curPos + MAX_DATA_SIZE > myBuf->getRaw () + WRITE_BUF_SIZE) {
			{
				const LockGuard guard {storageMutex};
				lseek (outputFD, 0, SEEK_END);
				if (write (outputFD, myBuf->getRaw (), curPos - myBuf->getRaw ()) < curPos - myBuf->getRaw ()) {
					errorMsg = "write to output file filed";
					wasAnError = true;
				}
			}
			curPos = myBuf->getRaw ();
		}

		size_t numBytesWritten;
		curPos = static_cast<char *> (dataToAdd[i].serialize (curPos, numBytesWritten));
	}
	doneWritingRun (curRun);

	// write back the buffer and free the memory
	const LockGuard guard {storageMutex};
	lseek (outputFD, 0, SEEK_END);
	if (write (outputFD, myBuf->getRaw (), curPos - myBuf->getRaw ()) < curPos - myBuf->getRaw ()) {
		errorMsg = "write to output file filed";
		wasAnError = true;
	}

	return wasAnError;
}

bool SimplePDB :: writeBackData (int whichFile, string &errorMsg) {

	// and open the new file
	int dataFD = open (getMappedName (whichFile).c_str (), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (dataFD < 0) {
		errorMsg = "Open of " + getMappedName (whichFile) + " failed; error is " + strerror (errno);
		return true;
	}

	// get a buffer
	PDBBufferPagePtr myBuf {myBuffers->get ()};
	char *curPos {myBuf->getRaw ()};

	// move to the start of the file
	lseek (dataFD, 0, SEEK_SET);

	// loop through all of the data runs
	bool wasAnError = false;
	for (size_t i = 0; i < allData.size (); i++) {
		getRunReadLock (i);
		for (PDBDataHolder p : allData[i]) {

			// make sure that this is being written to the correct file
			if (p.getFile () != whichFile) 
				continue;			

			// write back the buffer if we need to
			if (curPos + MAX_DATA_SIZE > myBuf->getRaw () + WRITE_BUF_SIZE) {
				if (write (dataFD, myBuf->getRaw (), curPos - myBuf->getRaw ()) < curPos - myBuf->getRaw ()) {
					errorMsg = "write to output file filed";
					wasAnError = true;
				}
				curPos = myBuf->getRaw ();
			}

			size_t numBytesWritten;
			curPos = static_cast<char *> (p.getData ().serialize (curPos, numBytesWritten));
		}
		doneReadingRun (i);
	}

	// write back the buffer and close the file
	if (write (dataFD, myBuf->getRaw (), curPos - myBuf->getRaw ()) < curPos - myBuf->getRaw ()) {
		errorMsg = "write to output file filed";
		wasAnError = true;
	}
	close (dataFD);	
	return wasAnError;
}

bool SimplePDB :: loadUpFile (const string &fName, string &errorMsg) {

	// first we need the deserialization machines from the catalog
	PDBStoredDataTypeDeserializationMachine dataDeserializer;
	PDBFeatureTypeDeserializationMachine featureDeserializer;
	{
		const LockGuard guard {catalogMutex};
		dataDeserializer = catalog->getStoredDataTypeDeserializer ();
		featureDeserializer = catalog->getFeatureTypeDeserializer ();
	}

	// the file descriptor for the file
	int fd;

	// open up the file and see how long it is
	fd = open (fName.c_str (), O_RDONLY);
	if (fd < 0) {
		errorMsg = "Open of '" + fName + "' failed; error is " + strerror (errno);
		return true;
	}
	
	// get an integer representing the file
	int whichFile = mapFName (fName);

	struct stat s;
	size_t size;
	int status;
	status = fstat (fd, &s);
	size = s.st_size;
	if (status < 0) {
		errorMsg = "Stat of '" + fName + "' failed; error is " + strerror (errno);
		return true;
	}
	
	// now we memory map the file so that we can process it
	char *mapped;
	mapped = static_cast<char *> (mmap (nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));

	// make sure that it actually worked
	if (mapped == MAP_FAILED) {
		errorMsg = "Could not memory map the file '" + fName + "' error is '" + strerror (errno);
		return true;
        }

	// and now read in the entire file
	char *curPos = mapped;
	int numObjects = 0;
	int runNumber = -1;
	while (curPos < mapped + size) {

		// WRITE_RUN_LEN objects go in each one of the lists
		if (numObjects % WRITE_RUN_LEN == 0) {

			// note that we are done writing to this run
			if (runNumber != -1) 
				doneWritingRun (runNumber);	

			// and now move on... getting the write lock on the next run
			runNumber = (runNumber + 1) % NUM_RUNS;
			getRunWriteLock (runNumber);
		}
		
		size_t numBytes;
		PDBData temp;
		curPos = static_cast<char *> (temp.deSerialize (curPos, numBytes, dataDeserializer, featureDeserializer));
		allData[runNumber].emplace_back (temp, whichFile);
		numObjects++;
	}

	// and release the lock if applicable
	if (runNumber != -1)
		doneWritingRun (runNumber);

	// now close the file
	close (fd);
	errorMsg = "";
	return false;
}

bool SimplePDB :: runQuery (PDBQueryExecutorPtr useMe, int whichRun, string &errMsg) {
	getRunReadLock (whichRun);	
	for (PDBDataHolder p : allData[whichRun]) {
		useMe->aggregate (p.getData ());
	}
	doneReadingRun (whichRun);	

	if (useMe->wasAnError (errMsg)) {
		return true;
	}
	errMsg = "";
	return false;
}

void SimplePDB :: getRunReadLock (int whichRun) {
	const LockGuard guard {dataMutexes[whichRun]};

	// wait until the writer is done
	while (numAllDataReaders[whichRun] < 0) {
		pthread_cond_wait (&dataVars[whichRun], &dataMutexes[whichRun]);
	}

	// now we have read access
	numAllDataReaders[whichRun]++;
}

void SimplePDB :: doneReadingRun (int whichRun) {

	const LockGuard guard {dataMutexes[whichRun]};
	numAllDataReaders[whichRun]--;

	// if the number of readers is down to zero, then see if there is a waiting writer
	if (numAllDataReaders[whichRun] == 0) {
		pthread_cond_signal (&dataVars[whichRun]);
	}
}

void SimplePDB :: getRunWriteLock (int whichRun) {

	const LockGuard guard {dataMutexes[whichRun]};
	
	// wait until there is no one in there 
	while (numAllDataReaders[whichRun] != 0) {
		pthread_cond_wait (&dataVars[whichRun], &dataMutexes[whichRun]);
	}

	// now we have exclusive access; use a -1 to indicate there is a writer
	numAllDataReaders[whichRun]--;
}

void SimplePDB :: doneWritingRun (int whichRun) {

	const LockGuard guard {dataMutexes[whichRun]};
	numAllDataReaders[whichRun]++;

	// now we signal everyone who is waiting
	pthread_cond_broadcast (&dataVars[whichRun]);
}

int SimplePDB :: mapFName (const string &fName) {
	const LockGuard guard {mapMutex};
	int i = 0;
	for (const string &s : mappedFNames) {
		if (s == fName) {
			return i;
		}
		i++;
	}
	mappedFNames.push_back (fName);
	return mappedFNames.size () - 1;
}

string SimplePDB :: getMappedName (int whichNum) {

	const LockGuard guard {mapMutex};
	return mappedFNames[whichNum];
}

void SimplePDB :: clearMappedFiles () {
	const LockGuard guard {mapMutex};
	mappedFNames.clear ();
}

int SimplePDB :: getNumMappedFiles () {
	return mappedFNames.size ();
}

bool SimplePDB :: setStorage (const string &toWhere, string &errMsg) {

	const LockGuard guard {storageMutex};

	if (outputFD != -1)
		close (outputFD);

	// check to see if the DB file already exists in the catalog; if it does not, then truncate
	bool foundIt = false;
	for (const string &s : catalog->getStorageLocations ()) {
		if (toWhere == s) 
			foundIt = true;
	}

	// get a mapped version of the storage name, and add to the catalog
	curFileMapped = mapFName (toWhere);
	catalog->addNewStorageLocation (toWhere);

	// and open the file
	if (foundIt) {
		outputFD = open (getMappedName (curFileMapped).c_str (), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	} else {
		outputFD = open (getMappedName (curFileMapped).c_str (), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	}

	if (outputFD < 0) {
		errMsg = "Open of " + getMappedName (curFileMapped) + " failed; error is " + strerror (errno);
		return true;
	}

	// move to the end of the file
	lseek (outputFD, 0, SEEK_END);

	errMsg = "";
	return false;
}

bool SimplePDB :: registerPDBStoredDataType (const string &libraryFile) {
	return catalog->registerStoredDataType (libraryFile);
}

bool SimplePDB :: registerPDBFeatureType (const string &libraryFile) {
	return catalog->registerFeatureType (libraryFile);
}

bool SimplePDB :: registerPDBMetricType (const string &libraryFile) {
	return catalog->registerMetric (libraryFile);
}

SimplePDB :: SimplePDB (int numRuns, PDBBufferPoolPtr myBufferIn) {

	// set up all of the key variables
	NUM_RUNS = numRuns;
	WRITE_RUN_LEN = 100;
	WRITE_BUF_SIZE = myBufferIn->get ()->getSize ();
	MAX_DATA_SIZE = WRITE_BUF_SIZE * 2 / 3;

	// set up the buffer
	myBuffers = myBufferIn;

	// set everything up
	initialize ();
}

int SimplePDB :: numRuns () {
	return NUM_RUNS;
}

PDBCatalogPtr SimplePDB :: getCatalog() {
	return catalog;
}
#endif
