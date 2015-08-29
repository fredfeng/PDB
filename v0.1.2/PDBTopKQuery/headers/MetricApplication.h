
#ifndef METRIC_APP
#define METRIC_APP

#ifdef __cplusplus
extern "C" {
#endif


// this file contains the type definitions for the basic structures
// that encode a top-K query


// this is a parameter that is used to call a metric function
struct MetricParam {

	// the name of the parameter
	char *paramName;

	// 1 if this parameter is from the query, 0 if from the target
	int fromQuery;

	// 1 if this parameter is from the target, 0 if from the query
	int fromTarget;
};

void free_MetricParam (struct MetricParam *);

	
// this is a list of parameters to a metric function
struct MetricParamList {

	// the first parameter
	struct MetricParam *param;

	// the remainder of the parameters
	struct MetricParamList *next;

};

void free_MetricParamList (struct MetricParamList *);


// this encodes a PDB similarity query; it is basically a linked list
// of MetricApplication objects, each of which encodes the application
// of one metric to a set of attributes in the source and target PEs
struct MetricApplication {

	// this is the weight that we are assigning to this metric
	// during query evaluation
	double weight;

	// this is the name of the metric that is to be called
	char *metricName;

	// here is the ordered list of parameters for this metric
	struct MetricParamList *paramList;

	// this is the remainder of the metrics that will be used
	// to implement this query
	struct MetricApplication *next;

};

void free_MetricApplication (struct MetricApplication *);


// and here is the query
struct FinalQuery {

	// the number of results to return
	unsigned k;
	
	// and the actual query
	struct MetricApplication *metricApplication;
};

void free_FinalQuery (struct FinalQuery *);


#ifdef __cplusplus
}
#endif

#endif
