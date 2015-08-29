
#ifndef PDBTOPKQUERY_C_
#define PDBTOPKQUERY_C_

#include "Lexer.h"
#include "MetricApplication.h"
#include "PDBTopKQuery.h"
#include "Parser.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;


PDBTopKQuery :: PDBTopKQuery () {
	gotAnError = false;
	errorMessage = "no error";
	totCount = 0;
	numValid = 0;
}

bool PDBTopKQuery :: wasAnError (string &error) {
	if (gotAnError) {
		error = errorMessage;
		return true;
	}
	return false;
}

PDBQueryExecutor &PDBTopKQuery :: operator += (PDBQueryExecutorPtr rhsIn) {

	PDBTopKQuery *rhs = dynamic_cast<PDBTopKQuery *> (rhsIn.get ());
	
	// aggregate all of his information
	totCount += rhs->totCount;
	numValid += rhs->numValid;
	if (rhs->gotAnError) {
		gotAnError = true;
		errorMessage = rhs->errorMessage;
	}

	// get all of this guy's contents
	priority_queue <PDBQueryResult, vector <PDBQueryResult>, PDBDataComparison> tempQ;
	while (rhs->myQ.size () > 0) {
		PDBQueryResult temp = rhs->myQ.top ();
		tempQ.push (temp);
		myQ.push (temp);
		rhs->myQ.pop ();
	}

	// copy him over
	rhs->myQ = tempQ;

	// limit the size to k
        while (myQ.size () > getK ()) {
        	myQ.pop ();
	}

	return *this;
}

PDBQueryExecutorPtr PDBTopKQuery :: getEmpty () {
	PDBQueryExecutorPtr result {make_shared<PDBTopKQuery> ()};
	PDBTopKQuery *res = dynamic_cast<PDBTopKQuery *> (result.get ());
	res->input = input;

	// since the metrics are stateful, we actually have to produce a deep copy
	for (PDBMetricPtr p : metricsToUse) {
		res->metricsToUse.push_back (p->getCopy ());
	}

	res->queryFeatures = queryFeatures;
	res->totCount = 0;
	res->numValid = 0;
	res->gotAnError = gotAnError;
	res->errorMessage = errorMessage;

	res->myQuery = myQuery;
	return result;
}

void PDBTopKQuery :: aggregate (PDBData &p) {

	if (gotAnError)
		return;

	// compute the distance
	bool wasValid;
	PDBQueryResult temp (p, apply (p.getFeatures (), wasValid, gotAnError, errorMessage));
	
	// if there was an error, we are done
	if (gotAnError) {
		return;
	}

	totCount++;
	if (wasValid) {
		numValid++;
		myQ.push (temp);
	}

	// limit the size to k
	if (myQ.size () > getK ()) {
		myQ.pop ();
	}
}

PDBQueryResultSet PDBTopKQuery :: getOutput () {
	priority_queue <PDBQueryResult, vector <PDBQueryResult>, PDBDataComparison> tempQ;
	PDBQueryResultSet output;
	while (myQ.size () > 0) {
		PDBQueryResult temp = myQ.top ();
		output.addNewQueryResult (temp);
		myQ.pop ();
		tempQ.push (temp);
	}
	myQ = tempQ;
	output.setCounts (totCount, totCount - numValid);
	return output;
}

string PDBTopKQuery :: display () {
	string result ("Scanned ");
	result += totCount;
	result += "objects; ";
	result += numValid;
	result += "were valid";
	return result;
}

void *PDBTopKQuery :: serialize (void *, size_t &) {
	return nullptr;
	// to do!
	return nullptr;
}

void *PDBTopKQuery :: deSerialize (void *, size_t &) {
	return nullptr;
	// to do!
	return nullptr;
}

unsigned PDBTopKQuery :: getK () {
	if (gotAnError)
		return 0;
	return myQuery->k;
}

PDBTopKQuery :: PDBTopKQuery (const string &queryString, const PDBFeatureList &queryIn, PDBCatalogPtr catalog) {

	// set the counts to zero
	totCount = 0;
	numValid = 0;
	
	// get a standard char string version of the string
	// and double null-terminate it (required by lex)
	input = queryString;
	input.push_back('\0');

	// and remember the query feature list
	queryFeatures = queryIn;
	
	// compile the query
	gotAnError = false;
	errorMessage = compile (gotAnError, catalog);
}

string PDBTopKQuery :: compile (bool &foundError, PDBCatalogPtr useMe) {

	foundError = false;

	// make sure we have a query
	if (input.empty()) {
		foundError = true;
		return "No query to process.";
	}

	// first thing is to use lex/yacc to parse the query
	yyscan_t scanner;
	LexerExtra extra { "" };
	yylex_init_extra (&extra, &scanner);
	const YY_BUFFER_STATE buffer { yy_scan_string (input.data(), scanner) };
	FinalQuery *final = nullptr;
	const int parseFailed { yyparse (scanner, &final) };
	yy_delete_buffer (buffer, scanner);
	yylex_destroy (scanner);

	if (parseFailed) {
		foundError = true;
		return extra.errorMessage;
	}

	// now we make sure that the query is OK
	if (final == nullptr) {
		cerr << "Got back a bad pointer from the parser!!";
		exit (1);
	}

	// remember the query
	myQuery.reset(final, free_FinalQuery);

	// make sure we have no metrics
	metricsToUse.clear ();
	
	// and check to make sure that all of the metrics are OK
	// loop through all of the different distance metrics that are used
	for (struct MetricApplication *myPtr = myQuery->metricApplication; myPtr != nullptr; myPtr = myPtr->next) {
	
		// get the metric from the catalog
		const string &metricName (myPtr->metricName);
		if (!useMe->isMetricRegistered (metricName)) {
			foundError = true;
			return "Error: did not find metric " + metricName;
		}

		// if we made it here, the metric was there, so retreive it
		PDBMetricPtr myMetric = useMe->getMetric (metricName);

		// and check to make sure that the number of parameters is correct, and that all of the query
		// attributes were actually found in the query
		int counter = 0;
		for (struct MetricParamList *listPtr = myPtr->paramList; listPtr != nullptr; listPtr = listPtr->next) {
			counter++;

			// if this is from the query, make sure it is there
			if (listPtr->param->fromQuery == 1) {
				const string &temp (listPtr->param->paramName);
				bool success;
				queryFeatures.getFeatureNamed (temp, success);
				if (!success) {
					ostringstream os;
					os << "Error: could not find a feature named " << temp << " in the query feature list";	
					foundError = true;
					return os.str ();
				}
			}
		}		

		if (myMetric->getNumParams () != counter) {
			ostringstream os;
			os << "Error: metric " << metricName << " required " << 
				myMetric->getNumParams () << " params; found " << counter;
			foundError = true;
			return os.str ();
		}

		// if we got here, the metric is OK
		metricsToUse.push_back (myMetric);
	}

	assert (!foundError);
	return string();
}

double PDBTopKQuery :: apply (PDBFeatureList &target, bool &wasValid, bool &wasError, string &errorMessageOut) {

	// everything is OK to start
	wasError = false;
	wasValid = true;

	// loop through all of the different metrics that are used
	int i = 0;
	double dist = 0;
	for (struct MetricApplication *myPtr = myQuery->metricApplication; myPtr != nullptr; myPtr = myPtr->next, i++) {

		// parameterize the metric		
		metricsToUse[i]->reset ();
		
		// loop through all of the parameters
		int j = 0;
		for (struct MetricParamList *listPtr = myPtr->paramList; listPtr != nullptr; listPtr = listPtr->next, j++) {
			
			const string &temp (listPtr->param->paramName);
			bool success;
			PDBFeatureTypePtr feature;

			// check if we are from the source or from the target	
			if (listPtr->param->fromTarget == 1) {
				feature = target.getFeatureNamed (temp, success);
			} else {
				feature = queryFeatures.getFeatureNamed (temp, success);				
			}

			// if we could not find this target feature
			if (!success) {
				wasValid = false;
				return 0.0;
			}

			// parameterize the metric
			metricsToUse[i]->takeParam (j, feature);
		}

		// apply the metric
		dist += myPtr->weight * metricsToUse[i]->apply (wasError, errorMessageOut);	

		// if there was an error (we were missing a paremter, or the parameter types were wrong) then return
		if (wasError) {
			return 0.0;
		}
	}
	return dist;
}

#endif
