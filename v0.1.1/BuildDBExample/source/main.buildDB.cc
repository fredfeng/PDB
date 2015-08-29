
#include <iostream>
#include <dirent.h>
#include "SimplePDB.h"
#include "BagOfWordsBuilder.h"
#include "PDBTopKQuery.h"
#include "PDBQueryResultSet.h"
#include "TextFileData.h"
#include "PDBFeatureListBuilder.h"
#include "PDBQueryExecutor.h"
#include "FileNameBuilder.h"
#include "WordFrequencyAggregator.h"

#include "TextFileData.h"

using namespace std;

int main () {

	// create a PDB object... assuming that the catalog file does not exist,
	// it will be created here
	SimplePDB myDB ("catalog", 1024 * 1024, 1024 * 256);

	// now, we go through all of the files in the 20 newsgroups directory...
	// for each one, we are going to create a PDBData object which we store
	// inside of the database
	DIR *dir;
	struct dirent *ent;
	string directory ("20News");
	int counter = 0;
	if ((dir = opendir (directory.c_str ())) != 0) {

		// read in all of the subdirectories
  		while ((ent = readdir (dir)) != 0) {

			// descend into the subdirectory
			string dirName (ent->d_name);

			// make sure it is not . or ..
			if (dirName == "." || dirName == "..")
				continue;
			cout << "Descending into " << dirName << "\n";
			DIR *subDir;
			string temp = directory + "/" + dirName;
			
			// set the storage
			string storage = dirName + ".db";
			myDB.setStorage (storage);
			if ((subDir = opendir (temp.c_str ())) != 0) {

				// now, loop through all of the files in the subdirectory
  				while ((ent = readdir (subDir)) != 0) {

					// nmake sure it is not . or ..
					string fName (ent->d_name);
					if (fName == "." || fName == "..")
						continue;

					// read in the file
					PDBStoredDataTypePtr newsgroupPost (new TextFileData (directory + "/" + dirName + "/" + fName));

					// now store the object
					PDBData dataToStore (newsgroupPost);
					myDB.store (dataToStore);
					counter++;
				}
			} else {
				cout << "Error descending!\n";
			}
  		}
		closedir (dir);
	} else {
		cerr << "Could not open 20News!!\n";
		exit (1);
	}
	
	// at this point we have created the database.  In order to create
	// the features (in particular the "bag of words" feature) we are
	// going to have to count the occurence of each word in the corpus.
	// This can be done using the PDBQueryExecutor class, which allows
	// us to execute some arbitrary query code over all of the objects
	// in the database...
	//
	// now we will count all of the frequencies of all of the words
	cout << "Done loading " << counter << " articles.  Now, using " <<
		"the WordFrequencyAggregator class to count.\n";
	PDBQueryExecutorPtr myAggregator (new WordFrequencyAggregator);
	myAggregator = myDB.runQuery (myAggregator);
	cout << "Done running aggregation; result is " << myAggregator->display () << "\n";

	// lastly we will add two features to all of the items in the database.
	// this is done by using the PDBFeatureListBuilder class.  This class
	// allows us to load up custom PDBFeatureBuilder objects, each of which
	// is tasked with adding a new feature to a PDBData object.  Here the
	// features that we create will be called "BagOfWords" and "FileName"
	cout << "Now adding fetures to everyone in the database.\n";
	PDBFeatureListBuilder featureMachine;
	PDBFeatureBuilderPtr featureBuilderOne (new BagOfWordsBuilder ("BagOfWords", myAggregator, 10000));
	PDBFeatureBuilderPtr featureBuilderTwo (new FileNameBuilder ("FileName"));	
	featureMachine.add (featureBuilderOne);
	featureMachine.add (featureBuilderTwo);
	myDB.buildFeatures (featureMachine);
	cout << "Done.\n";
}

