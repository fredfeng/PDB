#include "BagOfWordsBuilder.h"
#include "FileNameBuilder.h"
#include "PDBClient.h"
#include "PDBFeatureListBuilder.h"
#include "PDBQueryExecutor.h"
#include "TextFileData.h"
#include "WordFrequencyAggregator.h"

#include <dirent.h>
#include <iostream>

using namespace std;

// simple helper for readdir() that opens/closes the directory during
// construction/destruction, thereby preventing memory leaks
namespace {
	class OpenDir {
	public:
		explicit OpenDir(const string &path)
			: handle(opendir(path.c_str())) {
		}

		~OpenDir() {
			if (handle)
				closedir(handle);
		}

		OpenDir(const OpenDir &) = delete;
		OpenDir &operator=(const OpenDir &) = delete;

		bool valid() const {
			return handle;
		}

		dirent *readdir() const {
			return ::readdir(handle);
		}

	private:
		DIR * const handle;
	};
}


int main (int numArgs, const char *args []) {

	if (numArgs != 4) {
		cout << "usage: ./buildDB internetAddressOfServer port logFile\n";
		exit (0);
	}

	string internetAddressOfServer (args[1]);
	int port = atoi (args[2]);
	string logFile (args[3]);

	// create a PDB object... assuming that the catalog file does not exist,
	// it will be created here
	PDBClient myDB;
	bool wasError;
	string errMsg;
	myDB.connectToServer (internetAddressOfServer, port, wasError, errMsg, 12 * 1024 * 1204, logFile);
	if (wasError) {
		cout << "Was an error connecting to server: " << errMsg << "\n";
		exit (-1);
	}

	// now, we go through all of the files in the 20 newsgroups directory...
	// for each one, we are going to create a PDBData object which we store
	// inside of the database
	struct dirent *ent;
	string directory ("20News");
	const OpenDir dir { directory };
	int counter = 0;
	if (dir.valid ()) {

		// read in all of the subdirectories
  		while ((ent = dir.readdir ()) != nullptr) {

			// descend into the subdirectory
			string dirName (ent->d_name);

			// make sure it is not . or ..
			if (dirName == "." || dirName == "..")
				continue;

			// and make sure it is not a weird Mac hidden file
			if (dirName.find ("._") != string::npos)
	 			continue;	

			cout << "Descending into " << dirName << '\n';
			string temp = directory + '/' + dirName;
			
			// set the storage
			string storage = "DBFiles/" + dirName + ".db";
			bool res;
			string errMsg;
			myDB.changeStorageLocation (storage, res, errMsg);
			if (res) {
				cout << "Error setting storage file: " << errMsg << '\n';
				exit (1);
			}
			const OpenDir subDir { temp };
			if (subDir.valid ()) {

				// now, loop through all of the files in the subdirectory
				vector <PDBData> allData;
  				while ((ent = subDir.readdir ()) != nullptr) {

					// make sure it is not . or ..
					string fName (ent->d_name);
					if (fName == "." || fName == "..")
						continue;

					// make sure it is not a weird hidden file on Mac
					if (fName.find ("._") != string::npos)
						continue;	

					// read in the file
					PDBStoredDataTypePtr newsgroupPost {make_shared<TextFileData> (directory + '/' + 
						dirName + '/' + fName)};

					// now store the object
					PDBData dataToStore (newsgroupPost);
					allData.push_back (dataToStore);
					counter++;
				}
				myDB.addData (allData, res, errMsg);
				if (res) {
					cout << "Error storing the data: " << errMsg << '\n';
					exit (1);
				}
			} else {
				cout << "Error descending!\n";
			}
  		}
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
	PDBQueryExecutorPtr myAggregator {make_shared<WordFrequencyAggregator> ()};
	
	myAggregator = myDB.runQuery (myAggregator, wasError, errMsg);
	if (wasError) {
		cout << "Error running the aggregate: " << errMsg << '\n';
		exit (1);
	}
	cout << "Done running aggregation; result is " << myAggregator->display () << '\n';


        // lastly we will add two features to all of the items in the database.
        // this is done by using the PDBFeatureListBuilder class.  This class
        // allows us to load up custom PDBFeatureBuilder objects, each of which
        // is tasked with adding a new feature to a PDBData object.  Here the
        // features that we create will be called "BagOfWords" and "FileName"
        cout << "Now adding features to everyone in the database.\n";
        PDBFeatureListBuilder featureMachine;
        featureMachine.add (make_shared<BagOfWordsBuilder> ("BagOfWords", myAggregator, 10000));
        featureMachine.add (make_shared<FileNameBuilder> ("FileName"));
	myDB.buildFeatures (featureMachine, wasError, errMsg);
        if (wasError) {
                cout << "Error building the features: " << errMsg << '\n';
                exit (1);
        }
        cout << "Done.\n";

}

