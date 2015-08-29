
#ifndef CHRIS_CATALOG_H
#define CHRIS_CATALOG_H

#include <iostream>
#include <fstream>
#include "PDBCatalog.h"
#include "PDBMetric.h"
#include "CheckSameDirectory.h"
#include "EuclideanDistance.h"
#include "SparseVectorFeatureType.h"
#include "TextFeatureType.h"
#include "TextFileData.h"
#include "PDBFeatureTypeDeserializationMachine.h"
#include "PDBStoredDataTypeDeserializationMachine.h"
#include <iostream>
using namespace std;

class ChrisCatalog : public PDBCatalog {

public:

	~ChrisCatalog () {
		ofstream myFile (whereLocated);
		for (string s : files) {
			myFile << s << "\n";
		}
		myFile.close ();
	}

	void open (string whereLocatedIn) {

		whereLocated = whereLocatedIn;

		// if the catalog files is empty, get outta here
		if (whereLocated == "")
			return;

		// read in the all of the file names 
		string line;
		ifstream myfile (whereLocated);
		if (myfile.is_open()) {
			while (getline (myfile, line)) {
				files.push_back (line);
			}
			myfile.close();
		} else {
			cout << "Warning: unable to find file '" << whereLocated << "', so I am creating it.\n";
		} 
	}

	bool isMetricRegistered (string checkMe) {
		return checkMe == "EuclideanDistance" || checkMe == "CheckSameDirectory";
	}

	bool registerMetric (string sharedLibraryFile) {
		cerr << "Why did you call this?";
	}

	bool isFeatureTypeRegistered (string checkMe) {
		return checkMe == "SparseVectorFeatureType" || checkMe == "TextFeatureType";
	}

	bool registerFeatureType (string sharedLibraryFile) {
		cerr << "Why did you call this?";
	}

	bool isStoredDataTypeRegistered (string checkMe) {
		return checkMe == "TextFileData";
	}

	bool registerStoredDataType (string sharedLibraryFile) {
		cerr << "Why did you call this?";
	}

	PDBMetricPtr getMetric (string retreiveMe) {
		if (retreiveMe == "EuclideanDistance") {
			PDBMetricPtr temp (new EuclideanDistance);
			return temp;
		} else if (retreiveMe == "CheckSameDirectory") {
			PDBMetricPtr temp (new CheckSameDirectory);
			return temp;
		} else {
			PDBMetricPtr temp;
			return temp;
		}
	}

	PDBFeatureTypeDeserializationMachine &getFeatureTypeDeserializer () {
		return machineOne;
	}

	PDBStoredDataTypeDeserializationMachine &getStoredDataTypeDeserializer () {
		return machineTwo;
	}

	vector <string> getStorageLocations () {
		return files;
	}

	void addNewStorageLocation (string addMe) {
		files.push_back (addMe);
	}

	vector <string> getRegisteredPDBStoredDataTypes () {
		vector <string> temp;
		return temp;
	}
		
	vector <string> getRegisteredPDBFeatureTypes () {
		return getRegisteredPDBStoredDataTypes ();
	}

	vector <string> getRegisteredPDBMetrics () {
		return getRegisteredPDBStoredDataTypes ();
	}

	void unregisterPDBStoredDataType (string unregisterMe) {}
	void unregisterPDBFeatureType (string unregisterMe) {}
	void unregsiterPDBMetric (string unregisterMe) {}
	
	ChrisCatalog () {
		PDBStoredDataTypePtr tempData (new TextFileData);
		machineTwo.registerNewStoredDataType (tempData);
		PDBFeatureTypePtr tempData2 (new SparseVectorFeatureType);
		machineOne.registerNewFeatureType (tempData2);
		PDBFeatureTypePtr tempData3 (new TextFeatureType);
		machineOne.registerNewFeatureType (tempData3);
	}		

private:

	PDBFeatureTypeDeserializationMachine machineOne;
	PDBStoredDataTypeDeserializationMachine machineTwo;
	vector <string> files;
	string whereLocated;
};

#endif

