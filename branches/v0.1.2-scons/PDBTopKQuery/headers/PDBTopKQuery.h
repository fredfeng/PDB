
#ifndef PDBTOPKQUERY_H_
#define PDBTOPKQUERY_H_

#include "PDBCatalog.h"
#include "PDBData.h"
#include "PDBDataComparison.h"
#include "PDBFeatureList.h"
#include "PDBMetric.h"
#include "PDBQueryExecutor.h"
#include "PDBQueryResultSet.h"
#include <memory>
#include <queue>
#include <string>
#include <vector>

using namespace std;

// this class encapsulates the idea of a top-K query
class PDBTopKQuery : public PDBQueryExecutor {

public:

	// constructor that builds a PDBTopKQuery.  The string input into the constructor
	// is the specification for the query.  The syntax for a top-K query is:
	//
	//   "K = 12, Query =        
	//              ((0.4, metric1 (query.att2,                       
	//                              target.att4)),                     
	//               (0.6, metric2 (target.thatAtt,                    
	//                              query.thisAtt)))"
	//
	// What this means is that we are going to run a top-12 query where
	// we are comparing the feature att2 in the query to the feature att4
	// in a database object using the metric metric1. Also, we are comparing
	// the feature thisAtt in the query to the feature thatAtt in the database
	// object using metric2. When computing the distance (which combines the
	// result of running metric1 and metric2) these two metrics are
	// weighted according to the two specified weights; that is, 0.4 and 0.6.
	// That is, the value returned would be:
	//
	// 0.4 * metric1 (query.att2, target.att4) + 0.6 * metric2 (target.thatAtt, query.thisAtt)
	//
	// The parameter "query" is the set of query features that we are comparing
	// to, and the catalog is used to compile the query
	//
	PDBTopKQuery (const string &queryString, const PDBFeatureList &query, PDBCatalogPtr catalog);

	// constructor
	PDBTopKQuery ();

	// returns the answer to the top-K query
	PDBQueryResultSet getOutput ();

	// from PDBQueryExecutor
	PDBQueryExecutor &operator += (PDBQueryExecutorPtr rhs) override;
	PDBQueryExecutorPtr getEmpty () override;
	void aggregate (PDBData &queryMe) override;
	string display () override;
	bool wasAnError (string &message) override;
	void *serialize(void *toHere, size_t &size) override;
	void *deSerialize(void *fromHere, size_t &size) override;

private:

	// compiles the query string and then checks to see 
	// whether the specified query is valid... the result (valid or not)
	// is put into the parameter that is sent in... an appropriate message is 
	// returned by the call (the message is "valid" if the query is found to be
	// valid, and might be something like "ERROR: metric GraphEditDistance has
	// not been registered in the catalog" if an error is found.
	string compile (bool &foundError, PDBCatalogPtr useMe);

	// applies this particular query to a target data point to get the distance
	// note that wasValid is set to true if this comparison was valid; that is,
	// if all of the target attributes were found within the object target.  Also,
	// wasError is set to true if an error was encountered (for example, a metric
	// was not fully parameterized, or the wrong type was encountered for one of the
	// parameters to a metric).  In this case, an error message is put into the param
	// errorMessage
	double apply (PDBFeatureList &target, bool &wasValid, bool &wasError, 
		string &errorMessage);

	// gets the value for k
	unsigned getK ();

	// the query string in a c-like rep with *two* trailing NULs
	string input;

	// the vector of metrics that we are going to use
	vector <PDBMetricPtr> metricsToUse;

	// the parsed query
	shared_ptr <struct FinalQuery> myQuery;

	// whether there was an error
	bool gotAnError;
	string errorMessage;

	// counters
	int totCount, numValid;

	// this is the query feature list that we are comparing against
	PDBFeatureList queryFeatures;

        // used to evaluate the query
        priority_queue <PDBQueryResult, vector <PDBQueryResult>, PDBDataComparison> myQ;
};

#endif /* PDBTOPKQUERY_H_ */

