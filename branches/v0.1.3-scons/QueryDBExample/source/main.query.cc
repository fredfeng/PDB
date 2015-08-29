
#include "BagOfWordsBuilder.h"
#include "FileNameBuilder.h"
#include "PDBClient.h"
#include "PDBFeatureListBuilder.h"
#include "PDBQueryExecutor.h"
#include "PDBQueryResultSet.h"
#include "PDBTopKQuery.h"
#include "TextFileData.h"
#include "WordFrequencyAggregator.h"
#include <dirent.h>
#include <iostream>

using namespace std;

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
		cout << "Error connecting to the DB: " << errMsg << '\n';
		exit (1);
	}

	// now we will count all of the frequencies of all of the words... 
	// this is done so that we can build the query features
	cout << "Using the WordFrequencyAggregator class to count.\n";
	PDBQueryExecutorPtr myAggregator {make_shared<WordFrequencyAggregator> ()};

	myAggregator = myDB.runQuery (myAggregator, wasError, errMsg);
	if (wasError) {
		cout << "Error running query: " << errMsg << '\n';
		exit (1);
	}
	cout << "Done running aggregation; result is " << myAggregator->display () << '\n';

	// now we ask a query... to build up the query, we first load up 
	// a newsgroup post on Motif
	cout << "Creating a top-k query.\n";
	PDBStoredDataTypePtr newsgroupPost {make_shared<TextFileData> ("20News/sci.med/59002")};
	PDBData query (newsgroupPost);

	// use a PDBFeatureListBuilder object to add two features to the
	// query object
        PDBFeatureListBuilder featureMachine;
        PDBFeatureBuilderPtr featureBuilderOne {make_shared<BagOfWordsBuilder> ("BOW", myAggregator, 10000)};
        PDBFeatureBuilderPtr featureBuilderTwo {make_shared<FileNameBuilder> ("FName")};
        featureMachine.add (featureBuilderOne);
        featureMachine.add (featureBuilderTwo);
        if (query.buildFeatures (featureMachine, errMsg)) {
		cout << "Problem building featues: " << errMsg << '\n';
		exit (1);
	}

	// create the query
	PDBQueryExecutorPtr myTopK {make_shared<PDBTopKQuery> (
		"K = 12, Query =                                                   \n\
				 ((0.5, EuclideanDistance (target.BagOfWords,      \n\
                                                           query.BOW)),            \n\
			          (0.5, CheckSameDirectory (target.FileName,       \n\
						            query.FName)))",
		query.getFeatures (), myDB.getCatalog () )};

	// execute it
	myTopK = myDB.runQuery (myTopK, wasError, errMsg);
	if (wasError) {
		cout << "Error running top k: " << errMsg << '\n';
		myAggregator = myDB.runQuery (myAggregator, wasError, errMsg);
		if (wasError) {
			cout << "Error running top k: " << errMsg << '\n';
		}
		exit (1);
	}
	PDBTopKQuery *result = dynamic_cast<PDBTopKQuery *> (myTopK.get ());

	// make sure everything was OK
	string errorMessage;
	if (myTopK->wasAnError (errorMessage)) {
		cout << "Found an error " << errorMessage << '\n';
	}
	PDBQueryResultSet queryAnswer = result->getOutput ();

 	// and print the results
	if (!queryAnswer.wasSuccessful ()) {
		cout << "Error answering query: " << queryAnswer.getErrorMessage () << '\n';
	}
	cout << "Checked " << queryAnswer.getNumberDatabaseItemsChecked () << " database objects.\n";
	cout << queryAnswer.getNumberDatabaseItemsIgnored () << " of them were ignored.\n";
	vector <PDBQueryResult> output = queryAnswer.getResults ();
	cout << "Got " << output.size () << " results:\n";
	cout << result->display () << "\n";
	for (PDBQueryResult r : output) {
		cout << '\t' << r.getData ().display () << '\n';
	}
	cout << '\n';

	// just to illustrate what happens when you have an error
	PDBQueryExecutorPtr myTopKAgain {make_shared<PDBTopKQuery> (
		"K = 12, Query = this is so cool I love it", 
		query.getFeatures (), myDB.getCatalog () )};

	// make sure everything was bad
	if (myTopKAgain->wasAnError (errorMessage)) {
		cout << "Found an error " << errorMessage << '\n';
	}

	// and make sure running it returns an error
	myTopKAgain = myDB.runQuery (myTopKAgain, wasError, errMsg);
	if (wasError) {
		cout << "Error running top k: " << errMsg << '\n';
	}
}

