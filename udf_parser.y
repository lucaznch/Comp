%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;          /* integer value */
  std::string          *s;          /* symbol name or string literal */
  cdk::basic_node      *node;       /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  udf::block_node      *block_node;
};

%token <i> tINTEGER
%token <s> tIDENTIFIER tSTRING
%token tWHILE tIF tPRINT tREAD tBEGIN tEND

%token tTYPE_INT tTYPE_REAL tTYPE_STRING tTYPE_VOID
%token tPUBLIC tFORWARD tAUTO
%token tFOR tBREAK tCONTINUE tRETURN
%token tINPUT
%token tWRITE tWRITELN

%nonassoc tIFX
%nonassoc tELIF tELSE

%right '='
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY

%type <expression> expr
%type <lvalue> lval
%type <sequence> decls stmts args exprs
%type <node> decl stmt arg program 
%type <block_node> block
%type <type> type
%type <i> qualifier
%type <s> string
%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

program : decls { compiler->ast(new udf::program_node(LINE, $1)); }
        ;

stmts : stmt      { $$ = new cdk::sequence_node(LINE, $1); }
     | stmts stmt { $$ = new cdk::sequence_node(LINE, $2, $1); }
     | /* EMPTY */ {$$ = new cdk::sequence_node(LINE);}
     ;

decls : decl      { $$ = new cdk::sequence_node(LINE, $1); }
     | decls decl { $$ = new cdk::sequence_node(LINE, $2, $1); }
     | /* EMPTY */ {$$ = new cdk::sequence_node(LINE);}
     ;

args : arg      { $$ = new cdk::sequence_node(LINE, $1); }
     | args',' arg { $$ = new cdk::sequence_node(LINE, $3, $1); }
     ;

exprs : expr       { $$ = new cdk::sequence_node(LINE, $1); }
     | exprs',' expr { $$ = new cdk::sequence_node(LINE, $3, $1); }
     ;

stmt : expr ';'                         { $$ = new udf::evaluation_node(LINE, $1); }
     | tIF '(' expr ')' stmt %prec tIFX { $$ = new udf::if_node(LINE, $3, $5); }
     | tIF '(' expr ')' stmt tELSE stmt { $$ = new udf::if_else_node(LINE, $3, $5, $7); }
     | tWRITE exprs ';' { $$ = new udf::write_node(LINE, $2, false);}
     | tWRITELN exprs ';' { $$ = new udf::write_node(LINE, $2, true);}
     | tRETURN expr ';' {$$ = new udf::return_node(LINE, $2);}
     ;

block: '{' decls stmts '}' { $$ = new udf::block_node(LINE, $2, $3);}

arg: type tIDENTIFIER    { $$ = new udf::var_declaration_node(LINE, 0, *$2, false, $1,nullptr); }

decl
  : qualifier type tIDENTIFIER '(' args ')' block
      { $$ = new udf::function_node(LINE, $1, *$3, false, $5, $2, $7); }
  | qualifier type tIDENTIFIER '(' args ')' 
      { $$ = new udf::function_node(LINE, $1, *$3, false, $5, $2, nullptr); }
  | qualifier type tIDENTIFIER '(' ')' block
      { $$ = new udf::function_node(LINE, $1, *$3, false, nullptr, $2, $6); }
  | qualifier type tIDENTIFIER '(' ')' 
      { $$ = new udf::function_node(LINE, $1, *$3, false, nullptr, $2, nullptr); }
  | qualifier type tIDENTIFIER ';'
      { $$ = new udf::var_declaration_node(LINE, $1, *$3, false, $2); delete $3; }
  | qualifier type tIDENTIFIER '=' expr ';'
      { $$ = new udf::var_declaration_node(LINE, $1, *$3, false, $2, $5); delete $3; }
  ;
     
qualifier : tPUBLIC {$$ = 1;}
            |tFORWARD {$$ = 2;}
            |/* EMPTY */ {$$ =0;}
            ;

type : tTYPE_INT           { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
     | tTYPE_STRING        { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
     | tTYPE_VOID          {$$ = cdk::primitive_type::create(0, cdk::TYPE_VOID);}
     ;

expr : tINTEGER              { $$ = new cdk::integer_node(LINE, $1); }
     | string               { $$ = new cdk::string_node(LINE, $1); }
     | '-' expr %prec tUNARY { $$ = new cdk::unary_minus_node(LINE, $2); }
     | '+' expr %prec tUNARY { $$ = new cdk::unary_plus_node(LINE, $2); }
     | expr '+' expr         { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr         { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr         { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr         { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr         { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr         { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr         { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr         { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr         { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr         { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr         { $$ = new cdk::eq_node(LINE, $1, $3); }
     | '(' expr ')'          { $$ = $2; }
     | lval                  { $$ = new cdk::rvalue_node(LINE, $1); }
     | lval '=' expr         { $$ = new cdk::assignment_node(LINE, $1, $3); }
     ;

string
     : tSTRING      { $$ = $1; }
     | string tSTRING { $$ = new std::string(*$1 + *$2); delete $1; delete $2 ;}
     ;
lval : tIDENTIFIER             { $$ = new cdk::variable_node(LINE, $1); }
     ;

%%
