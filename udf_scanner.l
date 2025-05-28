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

bool hex_to_int(int &val, const std::string &s) {
  try {
    val = std::stoi(s.c_str(), nullptr, 16);
    return true;
  } catch (std::invalid_argument &e) {
    return false;
  } catch (std::out_of_range &e) {
    return false;
  }
}

%}
%x X_COMMENT X_STRING
%%
  yydebug=1; set_debug(1);

"//".*                 ;                          /* ignore comments */

"/*"                   yy_push_state(X_COMMENT);  /* enter comment */
<X_COMMENT>"*/"        yy_pop_state();            /* exit comment */
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);  /* nested comment */
<X_COMMENT>.|\n        ;                          /* ignore comment content */

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
"&&"                   return tAND;
"||"                   return tOR;


[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

\"                      yy_push_state(X_STRING); yylval.s = new std::string(""); // enter string
<X_STRING>\"            yy_pop_state(); return tSTRING; // exit string
<X_STRING>\\\"          *yylval.s += '\"';              // escaped quote
<X_STRING>\\.           *yylval.s += yytext[1];         // handle common escapes
<X_STRING>\n            yyerror("newline in string");   // disallow newline in string
<X_STRING>.             *yylval.s += *yytext;           // regular character

0x[0-9a-fA-F]+ {
  if (!hex_to_int(yylval.i, yytext)) {
    yyerror("Invalid hex number");
  }
  return tINTEGER;
}

[0-9]+                 yylval.i = strtol(yytext, nullptr, 10); return tINTEGER;

[-()<>=+*/%;{}.~]       return *yytext;

[ \t\n]+               ; /* ignore whitespace */

.                      fprintf(stderr, "'%s'\t", yytext); yyerror("Unknown character"); 

%%
