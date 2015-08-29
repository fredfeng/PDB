#ifndef PDBTOPKQUERY_PARSER_H_
#define PDBTOPKQUERY_PARSER_H_

#include "Lexer.h"

struct FinalQuery;

#ifdef __cplusplus
extern "C"
#endif
int yyparse (yyscan_t, struct FinalQuery **);

#endif	/* !PDBTOPKQUERY_PARSER_H_ */
