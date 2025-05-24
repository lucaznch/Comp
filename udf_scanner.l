%option c++ prefix="udf_scanner_" outfile="udf_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "udf_parser.tab.h"

// don't change this
#define yyerror LexerError
%}
%x X_STRING
%%
  yydebug=1; set_debug(1);

"--".*                 ; /* ignore comments */

"int"                  return tTYPE_INT;
"real"                 return tTYPE_REAL;
"string"               return tTYPE_STRING;
"void"                 return tTYPE_VOID;

"public"               return tPUBLIC;
"forward"              return tFORWARD;
"auto"                 return tAUTO;

"for"                  return tFOR; 
"break"                return tBREAK;
"continue"             return tCONTINUE;
"return"               return tRETURN;

"input"                return tINPUT;

"write"                return tWRITE;
"writeln"              return tWRITELN;

"if"                   return tIF; 
"elif"                 return tELIF;
"else"                 return tELSE; 

">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 


[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

\'                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\'           yy_pop_state(); return tSTRING;
<X_STRING>\\\'         *yylval.s += yytext + 1;
<X_STRING>.            *yylval.s += yytext;
<X_STRING>\n           yyerror("newline in string");

[0-9]+                 yylval.i = strtol(yytext, nullptr, 10); return tINTEGER;

[-()<>=+*/%;{}.]       return *yytext;

[ \t\n]+               ; /* ignore whitespace */

.                      yyerror("Unknown character");

%%
