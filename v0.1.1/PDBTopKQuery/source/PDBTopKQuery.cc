
#ifndef PDBTOPKQUERY_C_
#define PDBTOPKQUERY_C_

#include "PDBTopKQuery.h"
#include "MetricApplication.h"
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
using namespace std;

// for lex/yacc
extern "C" {
        struct yy_buffer_state;
        typedef yy_buffer_state *YY_BUFFER_STATE;
        YY_BUFFER_STATE yy_scan_string (char * string);
        int yyparse (void);   // defined in y.tab.c
	char yyerrorstring[500];
}

extern struct FinalQuery *final;

PDBTopKQuery :: PDBTopKQuery () {
	input = 0;
	gotAnError = false;
	errorMessage = "no error";
	totCount = 0;
	numValid = 0;
}

PDBTopKQuery :: ~PDBTopKQuery () {
	// to do!!!  free memory for compiled query
}

bool PDBTopKQuery :: wasAnError (string &error) {
	if (gotAnError) {
		error = errorMessage;
		return true;
	}
	return false;
}

PDBQueryExecutor &PDBTopKQuery :: operator += (PDBQueryExecutorPtr rhsIn) {

	PDBTopKQuery *rhs = (PDBTopKQuery *) rhsIn.get ();
	
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
        if (myQ.size () > getK ()) {
        	myQ.pop ();
	}

	return *this;
}

PDBQueryExecutorPtr PDBTopKQuery :: getEmpty () {
	PDBQueryExecutorPtr result (new PDBTopKQuery);
	PDBTopKQuery *res = (PDBTopKQuery *) result.get ();
	res->input = input;
	res->metricsToUse = metricsToUse;
	res->queryFeatures = queryFeatures;

	// fix this later!!  Not safe from memory leak!!
	res->myQuery = myQuery;
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

void *PDBTopKQuery :: serialize (void *toHere, size_t &size) {
	// to do!
}

void *PDBTopKQuery :: deSerialize (void *fromHere, size_t &size) {
	// to do!
}

int PDBTopKQuery :: getK () {
	return myQuery->k;
}

PDBTopKQuery :: PDBTopKQuery (string queryString, const PDBFeatureList &queryIn, PDBCatalogPtr catalog) {

	// set the counts to zero
	totCount = 0;
	numValid = 0;
	
	// get a standard char string version of the string
	input = new char[queryString.length () + 2];
	strcpy (input, queryString.c_str ());	
	
	// double null-terminate it (required by lex)
	input [queryString.length () + 1] = 0;

	// and remember the query feature list
	queryFeatures = queryIn;
	
	// compile the query
	gotAnError = false;
	errorMessage = compile (gotAnError, catalog);
}

string PDBTopKQuery :: compile (bool &foundError, PDBCatalogPtr useMe) {

	foundError = false;

	// make sure we have a query
	if (input == 0) {
		foundError = true;
		return "No query to process.";
	}

	// first thing is to use lex/yacc to parse the query
	YY_BUFFER_STATE buffer = yy_scan_string (input);
	if (yyparse ()) {
		foundError = true;
		string temp (yyerrorstring);
		return temp;
	}

	// now we make sure that the query is OK
	if (final == 0) {
		cerr << "Got back a bad pointer from the parser!!";
		exit (1);
	}

	// remember the query
	myQuery = final;
	final = 0;

	// make sure we have no metrics
	metricsToUse.clear ();
	
	// and check to make sure that all of the metrics are OK
	// loop through all of the different distance metrics that are used
	for (struct MetricApplication *myPtr = myQuery->metricApplication; myPtr != 0; myPtr = myPtr->next) {
	
		// get the metric from the catalog
		string metricName (myPtr->metricName);
		if (!useMe->isMetricRegistered (metricName)) {
			foundError = true;
			string temp = "Error: did not find metric ";
			temp += metricName;
			return temp;
		}

		// if we made it here, the metric was there, so retreive it
		PDBMetricPtr myMetric = useMe->getMetric (metricName);

		// and check to make sure that the number of parameters is correct, and that all of the query
		// attributes were actually found in the query
		int counter = 0;
		for (struct MetricParamList *listPtr = myPtr->paramList; listPtr != 0; listPtr = listPtr->next) {
			counter++;

			// if this is from the query, make sure it is there
			if (listPtr->param->fromQuery == 1) {
				string temp (listPtr->param->paramName);
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
}

double PDBTopKQuery :: apply (PDBFeatureList &target, bool &wasValid, bool &wasError, string &errorMessageOut) {

	// everything is OK to start
	wasError = false;
	wasValid = true;

	// loop through all of the different metrics that are used
	int i = 0;
	double dist = 0;
	for (struct MetricApplication *myPtr = myQuery->metricApplication; myPtr != 0; myPtr = myPtr->next, i++) {

		// parameterize the metric		
		metricsToUse[i]->reset ();
		
		// loop through all of the parameters
		int j = 0;
		for (struct MetricParamList *listPtr = myPtr->paramList; listPtr != 0; listPtr = listPtr->next, j++) {
			
			string temp (listPtr->param->paramName);
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
				exit (1);
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
