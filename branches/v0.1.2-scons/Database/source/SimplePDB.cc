
#ifndef CHRIS_PDB_C
#define CHRIS_PDB_C

#include "AddDataWork.h"
#include "AddFeaturesWork.h"
#include "LockGuard.h"
#include "OpenFileWork.h"
#include "PDB.h"
#include "PDBFeatureBuilder.h"
#include "PDBQueryExecutor.h"
#include "QueryWork.h"
#include "SepukuWork.h"
#include "SimplePDB.h"
#include "WriteBackDataWork.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;


void SimplePDB :: doWorkForever () {

	// just loop forever
	while (true) {

		// this is the work we are going to do
		PDBWorkPtr myWork;

		{
			// get the lock on the work queue
			const LockGuard guard {workToDoQueueMutex};

			// make sure that there is work to do 
			while (workToDoQueue.size () == 0) {
				pthread_cond_wait (&workToDoSignal, &workToDoQueueMutex);	
			}
	
			// get some work
			myWork = workToDoQueue.front ();
			workToDoQueue.pop ();	

			// release the mutex
		}
		// do the work
		myWork->execute ();
			
		// signal to whoever is waiting that the work is done
		myWork->done ();
	}
}

// this is the entry point for all of the worker threads
void *callDoWorkForever (void *pdbInstance) {
	SimplePDB *temp = static_cast<SimplePDB *> (pdbInstance);
	temp->doWorkForever ();
	return nullptr;
}

SimplePDB :: ~SimplePDB () {

	// ask all of the worker threads to kill themselves 
	vector <PDBWorkPtr> workToDo;
	workToDo.reserve (NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; i++) {
		workToDo.emplace_back (make_shared<SepukuWork> ());
		addWorkToQueue (workToDo.back ());
	}

	for (PDBWorkPtr p : workToDo) {
		p->waitUntilDone ();
	}
	
	// free all of the mutexes and condition variables
	pthread_mutex_destroy (&workToDoQueueMutex);
	pthread_cond_destroy (&workToDoSignal);
	for (auto &mutex : dataMutexes)
		pthread_mutex_destroy (&mutex);
	for (auto &cond : dataVars)
		pthread_cond_destroy (&cond);
	pthread_mutex_destroy (&catalogMutex);
	pthread_mutex_destroy (&lastStoredMutex);
	pthread_mutex_destroy (&mapMutex);
	pthread_mutex_destroy (&storageMutex);

	// and free the threads		
	for (auto &thread : threads) {
		pthread_join(thread, nullptr);
	}
}

void SimplePDB :: initialize () {

	// set up all of the mutexes and condition variables

	// first is the work queue
	pthread_mutex_init (&workToDoQueueMutex, nullptr);
	pthread_cond_init (&workToDoSignal, nullptr);

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

	// finally, set up all of the threads
	threads.resize(NUM_THREADS);
	for (auto &thread : threads) {
		int return_code = pthread_create (&thread, nullptr, callDoWorkForever, this);
		if (return_code) {
         		cout << "ERROR; return code from pthread_create () is " << return_code << '\n';
         		exit (-1);
      		}
	}

	outputFD = -1;
}

void SimplePDB :: addWorkToQueue (PDBWorkPtr addMe) {

	// add the work to the work queue, and then let the workers know we are done
	const LockGuard guard {workToDoQueueMutex};
	workToDoQueue.push (addMe);
	pthread_cond_signal (&workToDoSignal);
}

bool SimplePDB :: openFiles (vector <string> fNames, string &errorMsg) {

	// create a list of work
	vector <PDBWorkPtr> workToDo;
	workToDo.reserve (fNames.size ());
	for (const string &s : fNames) {
		workToDo.emplace_back (make_shared<OpenFileWork> (this, s));
		addWorkToQueue (workToDo.back ());
	}
	
	bool wasError = false;
	for (PDBWorkPtr p : workToDo) {

		// wait until it is done
		p->waitUntilDone ();

		if (p->wasError ()) {
			errorMsg = p->getError ();
			wasError = true;
		}
	}

	return wasError;
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

	// and read in all of the files
	return (openFiles (catalog->getStorageLocations (), errorMsg));
}
	
PDBQueryExecutorPtr SimplePDB :: runQuery (PDBQueryExecutorPtr useMe, bool &wasError, string &errorMsg) {

	// create a list of work
	vector <PDBWorkPtr> workToDo;
	workToDo.reserve (allData.size ());

	// and do the work
	int whichList = 0;
	for (size_t i = 0; i < allData.size (); ++i) {
		workToDo.emplace_back (make_shared<QueryWork> (this, whichList++, useMe->getEmpty ()));
		addWorkToQueue (workToDo.back ());
	}

	// get an answer
	PDBQueryExecutorPtr result = useMe->getEmpty ();

	// now, add every answer into the result
	wasError = false;
	for (PDBWorkPtr p : workToDo) {

		// wait until it is done
		p->waitUntilDone ();
		
		if (p->wasError ()) {
			errorMsg = p->getError ();
			wasError = true;
		}

		// and get the result out
		QueryWork *completedWork = dynamic_cast<QueryWork *> (p.get ());
		(*result) += completedWork->getResult ();
	}

	// and get outta here!
	return result;
}

bool SimplePDB :: buildFeatures (PDBFeatureListBuilder &useMe, string &errorMsg) {

	// create a lit of work
	vector <PDBWorkPtr> featureBuildingWorkToDo;
	featureBuildingWorkToDo.reserve (allData.size ());

	// do the work of adding the features to every item in the database
	int whichList = 0;
	for (size_t i = 0; i < allData.size (); ++i) {
		featureBuildingWorkToDo.emplace_back (make_shared<AddFeaturesWork> (this, whichList++, useMe));
		addWorkToQueue (featureBuildingWorkToDo.back ());
	}

	// now wait until all of the feature-adding work is done
	bool wasError = false;
	for (PDBWorkPtr p : featureBuildingWorkToDo) {

		// wait until it is done
		p->waitUntilDone ();

		if (p->wasError ()) {
			errorMsg = p->getError ();
			wasError = true;
		}
	}

	// now go through and write back all of the new features
	vector <PDBWorkPtr> writingBackWorkToDo;
	writingBackWorkToDo.reserve (getNumMappedFiles ());
	for (int i = 0; i < getNumMappedFiles (); i++) {
		writingBackWorkToDo.emplace_back (make_shared<WriteBackDataWork> (this, i));
		addWorkToQueue (writingBackWorkToDo.back ());
	}
		
	// now wait until all of the writing back is done
	for (PDBWorkPtr p : writingBackWorkToDo) {

		// wait until it is done
		p->waitUntilDone ();

		if (p->wasError ()) {
			errorMsg = p->getError ();
			wasError = true;
		}
	}
	
	// and now we are done 
	return wasError;
}

bool SimplePDB :: store (PDBData &storeMe, string &errorMsg) {

	vector <PDBData> dataToAdd {storeMe};
	return store (dataToAdd, errorMsg);
}

bool SimplePDB :: store (vector <PDBData> &dataToAdd, string &errorMsg) {

	PDBWorkPtr pieceOfWork {make_shared<AddDataWork> (this, dataToAdd)};
	addWorkToQueue (pieceOfWork);
	pieceOfWork->waitUntilDone ();
	if (pieceOfWork->wasError ()) {
		errorMsg = pieceOfWork->getError ();
		return true;
	} else {
		return false;
	}
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
	vector <char> myBuf (WRITE_BUF_SIZE);
	char *curPos {myBuf.data ()};

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
		if (curPos + MAX_DATA_SIZE > myBuf.data () + WRITE_BUF_SIZE) {
			{
				const LockGuard guard {storageMutex};
				lseek (outputFD, 0, SEEK_END);
				if (write (outputFD, myBuf.data (), curPos - myBuf.data ()) < curPos - myBuf.data ()) {
					errorMsg = "write to output file filed";
					wasAnError = true;
				}
			}
			curPos = myBuf.data ();
		}

		size_t numBytesWritten;
		curPos = static_cast<char *> (dataToAdd[i].serialize (curPos, numBytesWritten));
	}
	doneWritingRun (curRun);

	// write back the buffer and free the memory
	const LockGuard guard {storageMutex};
	lseek (outputFD, 0, SEEK_END);
	if (write (outputFD, myBuf.data (), curPos - myBuf.data ()) < curPos - myBuf.data ()) {
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
	vector <char> myBuf (WRITE_BUF_SIZE);
	char *curPos {myBuf.data ()};

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
			if (curPos + MAX_DATA_SIZE > myBuf.data () + WRITE_BUF_SIZE) {
				if (write (dataFD, myBuf.data (), curPos - myBuf.data ()) < curPos - myBuf.data ()) {
					errorMsg = "write to output file filed";
					wasAnError = true;
				}
				curPos = myBuf.data ();
			}

			size_t numBytesWritten;
			curPos = static_cast<char *> (p.getData ().serialize (curPos, numBytesWritten));
		}
		doneReadingRun (i);
	}

	// write back the buffer and close the file
	if (write (dataFD, myBuf.data (), curPos - myBuf.data ()) < curPos - myBuf.data ()) {
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

SimplePDB :: SimplePDB (int numThreads, size_t writeBufferSize, size_t maxPBDDataSize) {

	// set up all of the key variables
	NUM_THREADS = numThreads;
	NUM_RUNS = numThreads;
	WRITE_RUN_LEN = 100;
	WRITE_BUF_SIZE = writeBufferSize;
	MAX_DATA_SIZE = maxPBDDataSize;

	// set everything up
	initialize ();
}

PDBCatalogPtr SimplePDB :: getCatalog() {
	return catalog;
}
#endif
