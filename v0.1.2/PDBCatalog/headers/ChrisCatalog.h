
#ifndef CHRIS_CATALOG_H
#define CHRIS_CATALOG_H

#include "CheckSameDirectory.h"
#include "EuclideanDistance.h"
#include "PDBCatalog.h"
#include "PDBFeatureTypeDeserializationMachine.h"
#include "PDBMetric.h"
#include "PDBStoredDataTypeDeserializationMachine.h"
#include "SparseVectorFeatureType.h"
#include "TextFeatureType.h"
#include "TextFileData.h"
#include <fstream>
#include <iostream>
using namespace std;

class ChrisCatalog : public PDBCatalog {

public:

	~ChrisCatalog () override {
		ofstream myFile (whereLocated);
		for (const string &s : files) {
			myFile << s << '\n';
		}
	}

	void open (const string &whereLocatedIn) override {

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
		} else {
			cout << "Warning: unable to find file '" << whereLocated << "', so I am creating it.\n";
		} 
	}

	bool isMetricRegistered (const string &checkMe) override {
		return checkMe == "EuclideanDistance" || checkMe == "CheckSameDirectory";
	}

	bool registerMetric (const string &) override {
		cerr << "Why did you call this?";
		return true;
	}

	bool isFeatureTypeRegistered (const string &checkMe) override {
		return checkMe == "SparseVectorFeatureType" || checkMe == "TextFeatureType";
	}

	bool registerFeatureType (const string &) override {
		cerr << "Why did you call this?";
		return true;
	}

	bool isStoredDataTypeRegistered (const string &checkMe) override {
		return checkMe == "TextFileData";
	}

	bool registerStoredDataType (const string &) override {
		cerr << "Why did you call this?";
		return true;
	}

	PDBMetricPtr getMetric (const string &retreiveMe) override {
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

	PDBFeatureTypeDeserializationMachine &getFeatureTypeDeserializer () override {
		return machineOne;
	}

	PDBStoredDataTypeDeserializationMachine &getStoredDataTypeDeserializer () override {
		return machineTwo;
	}

	vector <string> getStorageLocations () override {
		return files;
	}

	void addNewStorageLocation (const string &addMe) override {
	
		// if it is not already there, then add it
		bool gotIt = false;
		for (const string &s : files) {
			if (s == addMe)
				gotIt = true;
		}

		if (!gotIt)
			files.push_back (addMe);
	}

	vector <string> getRegisteredPDBStoredDataTypes () override {
		vector <string> temp;
		return temp;
	}
		
	vector <string> getRegisteredPDBFeatureTypes () override {
		return getRegisteredPDBStoredDataTypes ();
	}

	vector <string> getRegisteredPDBMetrics () override {
		return getRegisteredPDBStoredDataTypes ();
	}

	void unregisterPDBStoredDataType (const string &) override {}
	void unregisterPDBFeatureType (const string &) override {}
	void unregsiterPDBMetric (const string &) override {}
	
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

