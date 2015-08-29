
#ifndef CHRIS_PDB_C
#define CHRIS_PDB_C

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

#include "PDB.h"
#include "SimplePDB.h"
using namespace std;

PDBCatalogPtr SimplePDB :: getCatalog () {
	return catalog;
}

bool SimplePDB :: setStorage (string toWhere) {

	// remember the file to write to
	curFile = toWhere;

	// write back to the current file
	clearBuffer ();
	if (dataFD >= 0)
		close (dataFD);

	// and open the new file
	dataFD = open (toWhere.c_str (), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (dataFD < 0) {
		cerr << "Open of " << toWhere << " failed; error is " << strerror (errno) << "\n";
		return false;
	}

	// if this storage location is not known, then remember it
	bool gotIt = false;
	vector <string> storageLocations = catalog->getStorageLocations ();
	for (string s : storageLocations) {
		if (s == toWhere)
			gotIt = true;
	}

	if (!gotIt) 
		catalog->addNewStorageLocation (toWhere);

	// move to the end of the file
	lseek (dataFD, 0, SEEK_END);

	if (writeBufferStart == 0)
		writeBuffer = writeBufferStart = new char [writeBufferSize];

	return true;
}

bool SimplePDB :: store (PDBData &storeMe) {

	if (dataFD < 0) {
		cerr << "You cannot store an object without setting the storage path!";
		return false;
	}

	// if we don't have enough RAM, then write back the buffer
	if (writeBuffer + maxPBDDataSize > writeBufferStart + writeBufferSize) {
		
		// write back the current data
		clearBuffer ();
	}

	size_t numBytesWritten;
	writeBuffer = (char *) storeMe.serialize (writeBuffer, numBytesWritten);

	// and remember this guy for query processing
	storedObjects.push_back (storeMe);
}

SimplePDB :: ~SimplePDB () {

	closeDB ();

	// delete the write buffer
	if (writeBufferStart != 0) {
		delete [] writeBufferStart;
		writeBufferStart = 0;
	}
}

void SimplePDB :: clearBuffer () {

	if (dataFD >= 0) {
		if (writeBuffer != 0 && writeBuffer != writeBufferStart)
			write (dataFD, writeBufferStart, writeBuffer - writeBufferStart);
	}
	writeBuffer = writeBufferStart;
}

void SimplePDB :: closeDB () {

	// empty out the buffer
	clearBuffer ();

	if (dataFD >= 0)
		close (dataFD);
	dataFD = -1;

	// forget the stored objects
	storedObjects.clear ();

	// we are no longer writing
	curFile = "";
}

bool SimplePDB :: regsiterPDBStoredDataType (string libraryFile) {
	return catalog->registerStoredDataType (libraryFile);
}

bool SimplePDB :: regsiterPDBFeatureType (string libraryFile) {
	return catalog->registerFeatureType (libraryFile);
}

bool SimplePDB :: regsiterPDBMetricType (string libraryFile) {
	return catalog->registerMetric (libraryFile);
}

PDBQueryExecutorPtr SimplePDB :: runQuery (PDBQueryExecutorPtr useMe) {
	for (PDBData p : storedObjects) {
		useMe->aggregate (p);
	}
	return useMe;
}

bool SimplePDB :: buildFeatures (PDBFeatureListBuilder &useMe) {

	// make sure we have a write buffer
	if (writeBufferStart == 0)
		writeBuffer = writeBufferStart = new char [writeBufferSize];

	// remember the file we are writing to
	string lastFileName = curFile;

	// empty the buffer and close the file we are writing
	clearBuffer ();
	if (dataFD >= 0)
		close (dataFD);

	// get the vector of feature builders
	vector <PDBFeatureBuilderPtr> builders = useMe.getFeatureBuilders ();

	// go through all of the files and add the new fetures to each 
	vector <string> myFiles = catalog->getStorageLocations ();
	
	// loop through all of the data files
	for (string fName : myFiles) {

		// load the contents
		storedObjects.clear ();
		loadUpFile (fName);

		// add the features
		for (int i = 0; i < storedObjects.size (); i++) {
			
			// loop through each of the indiviaul feature builders
			for (PDBFeatureBuilderPtr p : builders) {

				// if this guy runs on this data object, then add the new feature
				if (p->runsOnThisPDBData (storedObjects[i])) {
					PDBFeatureTypePtr newFeature = p->buildFeature (storedObjects[i]);
					storedObjects[i].getFeatures ().addFeature (newFeature, p->getFeatureName ());
				}
			}
		}

		// and open the new file
		dataFD = open (fName.c_str (), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if (dataFD < 0) {
			cerr << "Open of " << fName << " failed; error is " << strerror (errno) << "\n";
			return false;
		}

		// move to the start of the file
		lseek (dataFD, 0, SEEK_SET);

		// loop through and serialize all of the objects
		for (int i = 0; i < storedObjects.size (); i++) {
		
			// if we don't have enough RAM, then write back the buffer
			if (writeBuffer + maxPBDDataSize > writeBufferStart + writeBufferSize) {
		
				// write back the current data
				clearBuffer ();
			}

			size_t numBytesWritten;
			writeBuffer = (char *) storedObjects[i].serialize (writeBuffer, numBytesWritten);
		}
		clearBuffer ();
		close (dataFD);	
		dataFD = -1;

		storedObjects.clear ();
	}
	
	// loop through and re-load the data ojects 
	for (string fName : myFiles) {
		loadUpFile (fName);
	}

	cout << "Loaded " << storedObjects.size () << " data objects from " << myFiles.size () << " different data files.\n";

	if (lastFileName != "") {
		setStorage (lastFileName);
	}
	
	return true;
}

SimplePDB :: SimplePDB (string catalogLocation, size_t writeBufferSizeIn, size_t maxPBDDataSizeIn) {
	writeBuffer = writeBufferStart = 0;	
	writeBufferSize = writeBufferSizeIn;
	maxPBDDataSize = maxPBDDataSizeIn;
	openDB (catalogLocation);
}

void SimplePDB :: loadUpFile (string fName) {

	// first we need the deserialization machines from the catalog
	PDBStoredDataTypeDeserializationMachine dataDeserializer = catalog->getStoredDataTypeDeserializer ();
	PDBFeatureTypeDeserializationMachine featureDeserializer = catalog->getFeatureTypeDeserializer ();

	// the file descriptor for the first file
	int fd;

	// open up the file and see how long it is
	fd = open (fName.c_str (), O_RDONLY);
	if (fd < 0) {
		cerr << "Open of '" << fName << "' failed; error is " << strerror (errno) << "\n";
	}
	
	struct stat s;
	size_t size;
	int status;
	status = fstat (fd, &s);
	size = s.st_size;
	if (status < 0) {
		cerr << "Stat of '" << fName << "' failed; error is " << strerror (errno) << "\n";
	}
	
	// now we memory map the file so that we can process it
	char *mapped;
	mapped = (char *) mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);

	// make sure that it actually worked
	if (mapped == MAP_FAILED) {
		cerr << "Could not memory map the file '" << fName << "' error is '" << strerror (errno) << "'\n";
        }

	// and now read in the entire file
	char *curPos = mapped;
	while (curPos < mapped + size) {
		size_t numBytes;
		PDBData temp;
		curPos = (char *) temp.deSerialize (curPos, numBytes, dataDeserializer, featureDeserializer);
		storedObjects.push_back (temp);
	}

	// now close the file
	close (fd);
}

void SimplePDB :: openDB (string catalogLocation) {

	// close up first 
	closeDB ();

	// open up the catalog
	PDBCatalogPtr temp (new ChrisCatalog);
	catalog = temp;
	catalog->open (catalogLocation);

	// go through all of the files and load everything up
	vector <string> myFiles = catalog->getStorageLocations ();
	
	// loop through all of the data files
	for (string fName : myFiles) {
		loadUpFile (fName);
	}

	cout << "Loaded " << storedObjects.size () << " data objects from " << myFiles.size () << " different data files.\n";
}

#endif
