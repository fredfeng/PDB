 
// This is the parser for a top-K query.  An example top-K query is:
//
//   "K = 12, Query =                                                    
//              ((0.4, metric1 (query.att2,         
//                              target.att4)), 
//               (0.6, metric2 (target.thatAtt,       
//                              query.thisAtt)))"
//
// What this means is that we are going to run a top-k query where 
// we are comparing the feature att2 in the query to the feature att4 
// in a database object using the metric metric1. Also, we are comparing 
// the feature thisAtt in the query to the feature thatAtt in the database 
// object using metric2. When computing the Euclidian distance (which 
// combines the result of these two comparisons) these two metrics are 
// weighted according to the two specified weights; that is, 0.4 and 0.6.

%{

	#include "Lexer.h"
	#include "MetricApplication.h" 
	#include "Parser.h" 
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

%}

// this stores all of the types returned by production rules
%union {
	struct FinalQuery *finalQuery;
 	struct MetricApplication *metricList;
	struct MetricParamList *paramList;
	struct MetricParam *param;
	char *charAtt;
	double doubleAtt;	
	int intAtt;
};

%pure-parser
%lex-param {void *scanner}
%parse-param {void *scanner}
%parse-param {struct FinalQuery **finalQuery}

%token <doubleAtt> Weight 
%token <charAtt> Query
%token <charAtt> Target
%token <charAtt> Name 
%token <intAtt> Integer

%type <metricList> DistanceMetricApplication
%type <finalQuery> MetricQuery
%type <param> MetricParam 
%type <paramList> MetricParams
%type <paramList> MetricParamList
%type <intAtt> K

%destructor { free_MetricApplication ($$); } DistanceMetricApplication
%destructor { free_MetricParamList ($$); } MetricParams MetricParamList
%destructor { free_MetricParam ($$); } MetricParam
%destructor { free ($$); } Name

%start MetricQuery


//******************************************************************************
// SECTION 3
//******************************************************************************
/* This is the PRODUCTION RULES section which defines how to "understand" the 
 * input language and what action to take for each "statment"
 */

%%

MetricQuery: K ',' Query '=' '(' DistanceMetricApplication ')'
{
	$$ = (struct FinalQuery *) malloc (sizeof (struct FinalQuery));
	*finalQuery = $$;
	$$->k = $1;
	$$->metricApplication = $6;
}
|
Query '=' '(' DistanceMetricApplication ')' ',' K 
{
	$$ = (struct FinalQuery *) malloc (sizeof (struct FinalQuery));
	*finalQuery = $$;
	$$->k = $7;
	$$->metricApplication = $4;
}
;
	
K: 'k' '=' Integer
{
	$$ = $3;
}

DistanceMetricApplication: '(' Weight ',' Name MetricParams ')'
{
	$$ = (struct MetricApplication *) malloc (sizeof (struct MetricApplication));
	$$->weight = $2;
	$$->paramList = $5;
	$$->metricName = $4;
	$$->next = NULL;
}

| '(' Weight ',' Name MetricParams ')' ',' DistanceMetricApplication
{
	$$ = (struct MetricApplication *) malloc (sizeof (struct MetricApplication));
	$$->weight = $2;
	$$->paramList = $5;
	$$->metricName = $4;
	$$->next = $8;
}
;

MetricParams: '(' MetricParamList ')'
{
	$$ = $2;
}
;

MetricParamList: MetricParam 
{
	$$ = (struct MetricParamList *) malloc (sizeof (struct MetricParamList));
	$$->param = $1;
	$$->next = NULL; 
}

| MetricParam ',' MetricParamList
{
	$$ = (struct MetricParamList *) malloc (sizeof (struct MetricParamList));
	$$->param = $1;
	$$->next = $3;
}
;

MetricParam: Query '.' Name 
{
	$$ = (struct MetricParam *) malloc (sizeof (struct MetricParam));
	$$->paramName = $3;
	$$->fromQuery = 1;
	$$->fromTarget = 0;
}

| Target '.' Name
{
	$$ = (struct MetricParam *) malloc (sizeof (struct MetricParam));
	$$->paramName = $3;
	$$->fromQuery = 0;
	$$->fromTarget = 1;
}

%%

