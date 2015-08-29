
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

	// create a PDB object... assuming that the database has alread
	// been created (so the catalog file "catalog" already exists)
	// the existind database will be opend
	SimplePDB myDB ("catalog", 1024 * 1024, 1024 * 256);

	// now we will count all of the frequencies of all of the words... 
	// this is done so that we can build the query features
	cout << "Done loading articles.  Now, using the WordFrequencyAggregator class to count.\n";
	PDBQueryExecutorPtr myAggregator (new WordFrequencyAggregator);
	myAggregator = myDB.runQuery (myAggregator);
	cout << "Done running aggregation; result is " << myAggregator->display () << "\n";

	// now we ask a query... to build up the query, we first load up 
	// a newsgroup post on Motif
	cout << "Creating a top-k query.\n";
	PDBStoredDataTypePtr newsgroupPost (new TextFileData ("20News/comp.windows.x/67090"));
	PDBData query (newsgroupPost);

	// use a PDBFeatureListBuilder object to add two features to the
	// query object
        PDBFeatureListBuilder featureMachine;
        PDBFeatureBuilderPtr featureBuilderOne (new BagOfWordsBuilder ("BOW", myAggregator, 10000));
        PDBFeatureBuilderPtr featureBuilderTwo (new FileNameBuilder ("FName"));
        featureMachine.add (featureBuilderOne);
        featureMachine.add (featureBuilderTwo);
        query.buildFeatures (featureMachine);

	// create the query
	PDBQueryExecutorPtr myTopK (new PDBTopKQuery (
		"K = 12, Query =                                                   \n\
				 ((0.5, EuclideanDistance (target.BagOfWords,      \n\
                                                           query.BOW)),            \n\
			          (0.5, CheckSameDirectory (target.FileName,       \n\
						            query.FName)))",
		query.getFeatures (), myDB.getCatalog () ));

	// execute it
	myTopK = myDB.runQuery (myTopK);
	PDBTopKQuery *result = (PDBTopKQuery *) myTopK.get ();

	// make sure everything was OK
	string errorMessage;
	if (myTopK->wasAnError (errorMessage)) {
		cout << "Found an error " << errorMessage << "\n";
	}
	PDBQueryResultSet queryAnswer = result->getOutput ();

 	// and print the results
	if (!queryAnswer.wasSuccessful ()) {
		cout << "Error answering query: " << queryAnswer.getErrorMessage () << "\n";
	}
	cout << "Checked " << queryAnswer.getNumberDatabaseItemsChecked () << " database objects.\n";
	cout << queryAnswer.getNumberDatabaseItemsIgnored () << " of them were ignored.\n";
	vector <PDBQueryResult> output = queryAnswer.getResults ();
	cout << "Got " << output.size () << " results:\n";
	for (PDBQueryResult r : output) {
		cout << "\t" << r.getData ().display () << "\n";
	}
	cout << "\n";

	// just to illustrate what happens when you have an error
	PDBQueryExecutorPtr myTopKAgain (new PDBTopKQuery (
		"K = 12, Query = this is so cool I love it", 
		query.getFeatures (), myDB.getCatalog () ));
	myTopKAgain = myDB.runQuery (myTopKAgain);

	// make sure everything was bad
	if (myTopKAgain->wasAnError (errorMessage)) {
		cout << "Found an error " << errorMessage << "\n";
	}
}

