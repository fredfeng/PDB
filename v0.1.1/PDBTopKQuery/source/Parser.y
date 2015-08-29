 
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

	#include "MetricApplication.h" 
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <iostream>

	extern "C" int yylex();
	extern "C" int lineno;
	extern "C" int yyparse();
	extern "C" char * yytext;
	extern "C" void yyerror(char *s);
  
	// this is the final parse tree that is returned	
	struct FinalQuery *final;	

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
	lineno = 1;
	$$ = (struct FinalQuery *) malloc (sizeof (struct FinalQuery));
	final = $$;
	$$->k = $1;
	$$->metricApplication = $6;
}
|
Query '=' '(' DistanceMetricApplication ')' ',' K 
{
	lineno = 1;
	$$ = (struct FinalQuery *) malloc (sizeof (struct FinalQuery));
	final = $$;
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

