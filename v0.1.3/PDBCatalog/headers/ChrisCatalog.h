
#ifndef CHRIS_CATALOG_H
#define CHRIS_CATALOG_H

#include "BagOfWordsBuilder.h"
#include "CheckSameDirectory.h"
#include "EuclideanDistance.h"
#include "FileNameBuilder.h"
#include "PDBCatalog.h"
#include "PDBFeatureTypeDeserializationMachine.h"
#include "PDBMetric.h"
#include "PDBStoredDataTypeDeserializationMachine.h"
#include "PDBTopKQuery.h"
#include "SparseVectorFeatureType.h"
#include "TextFeatureType.h"
#include "TextFileData.h"
#include "WordFrequencyAggregator.h"
#include <fstream>
#include <iostream>
using namespace std;

class ChrisCatalog : public PDBCatalog {

public:

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

	PDBQueryExecutorDeserializationMachine &getQueryExecutorDeserializer () override {
		return machineThree;
	}

	PDBFeatureBuilderDeserializationMachine &getFeatureBuilderDeserializationMachine () override {
		return machineFour;
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

		if (!gotIt) {
			files.push_back (addMe);
			ofstream myFile (whereLocated, ofstream :: out | ofstream :: app);
			myFile << addMe << '\n';
		}
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

		PDBQueryExecutorPtr tempQuery1 (new WordFrequencyAggregator);
		PDBQueryExecutorPtr tempQuery2 (new PDBTopKQuery (*this));
		machineThree.registerNewQueryExecutor (tempQuery1);
		machineThree.registerNewQueryExecutor (tempQuery2);

		PDBFeatureBuilderPtr builder1 (new FileNameBuilder);
		PDBFeatureBuilderPtr builder2 (new BagOfWordsBuilder);
		machineFour.registerNewFeatureBuilders (builder1);
		machineFour.registerNewFeatureBuilders (builder2);
	}		

private:

	PDBFeatureTypeDeserializationMachine machineOne;
	PDBStoredDataTypeDeserializationMachine machineTwo;
	PDBQueryExecutorDeserializationMachine machineThree;
	PDBFeatureBuilderDeserializationMachine machineFour;
	vector <string> files;
	string whereLocated;
};

#endif

