/***********************************************************************
 * SECTION 1
 ***********************************************************************/
/* The code in %{ %} is included as it is in lex.yy.c file
 * it has C global variables, prototypes, and comments 
 */


%{

#include <string.h> // For strdup()
#include <stdlib.h> // For malloc()
#include "MetricApplication.h"
#include "y.tab.h"

int lineno = 1;
char yyerrorstring [500];
void yyerror (char *s);

%}


/******************************************************************************
 * SECTION 2
 ******************************************************************************/
/* This is the DEFINITION section which contains substitutions, code, and
 * start stats; will be copied into lex.yy.c
 */

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

[0-9]+			{yylval.intAtt = atoi (yytext);
			 return Integer;
			}

[+-]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)? {yylval.doubleAtt = atof (yytext);
  			 return Weight;
			}

[A-Za-z][A-Za-z0-9_-]* {yylval.charAtt = strdup (yytext);
  			return Name;
			}     

"."    	        	return('.');

\n                	lineno++;

[ \t]             	;

.                 	yyerror("LEX_ERROR: invalid character");

%%

void yyerror(char *s) {
	sprintf (yyerrorstring, "error at line %d: %s at %s\n", lineno, s, yytext);
}

int yywrap(void){
  return 1;
}


