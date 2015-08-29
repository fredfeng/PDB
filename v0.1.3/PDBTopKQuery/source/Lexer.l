/***********************************************************************
 * SECTION 1
 ***********************************************************************/
/* The code in %{ %} is included as it is in lex.yy.c file
 * it has C global variables, prototypes, and comments 
 */


%{

#include "Lexer.h"
#include "MetricApplication.h"
#include "y.tab.h"
#include <stdio.h>  // For fileno()
#include <stdlib.h> // For malloc()
#include <string.h> // For strdup()

#ifdef __APPLE__
extern char *strdup(const char *s);
extern int snprintf(char * restrict str, size_t size, const char * restrict format, ...);
#endif

#define YY_USER_INIT (yylineno = 1)

%}


/******************************************************************************
 * SECTION 2
 ******************************************************************************/
/* This is the DEFINITION section which contains substitutions, code, and
 * start stats; will be copied into lex.yy.c
 */

%option noinput
%option nounput

%option bison-bridge
%option extra-type="struct LexerExtra *"
%option reentrant

/******************************************************************************
 * SECTION 3
 ******************************************************************************/
/* This is the RULES section which defines how to "scan" and what action
 * to take for each token
 */

%%

[kK]			return ('k');

"="			return ('=');

"("			return ('(');

")"    	        	return (')');

","    	        	return (',');

[tT][aA][rR][gG][eE][tT] return Target;

[qQ][uU][eE][rR][yY]	return Query; 

[0-9]+			{yylval->intAtt = atoi (yytext);
			 return Integer;
			}

[+-]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)? {yylval->doubleAtt = atof (yytext);
  			 return Weight;
			}

[A-Za-z][A-Za-z0-9_-]* {yylval->charAtt = strdup (yytext);
  			return Name;
			}     

"."    	        	return('.');

\n                	++yylineno;

[ \t]             	;

.                 	yyerror(yyscanner, 0, "LEX_ERROR: invalid character");

%%

void yyerror(yyscan_t scanner, struct FinalQuery **query, const char *s) {
	struct LexerExtra * const extra = yyget_extra (scanner);
	snprintf (extra->errorMessage, sizeof (extra->errorMessage), "error at line %d: %s at %s\n", yyget_lineno (scanner), s, yyget_text (scanner));
}

int yywrap(yyscan_t scanner){
  return 1;
}
