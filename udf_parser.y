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
  udf::block_node      *block;
};

%token <i> tINTEGER
%token <s> tIDENTIFIER tSTRING
%token tIF
%token tTYPE_INT tTYPE_REAL tTYPE_STRING tTYPE_VOID
%token tPUBLIC tFORWARD tAUTO
%token tFOR tBREAK tCONTINUE tRETURN
%token tINPUT
%token tWRITE tWRITELN

%nonassoc tIFX
%nonassoc tELIF tELSE

%right '='
%left tGE tLE tEQ tNE '>' '<' tAND tOR
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY tNOT

%type <node> program arg declr instr
%type <sequence> exprs args declrs instrs
%type <expression> expr
%type <lvalue> lval
%type <block> blck
%type <type> type
%type <s> string 

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

program : declrs { compiler->ast(new udf::program_node(LINE, $1)); }
        ;

expr : tINTEGER              { $$ = new cdk::integer_node(LINE, $1); }
     | string                { $$ = new cdk::string_node(LINE, $1); delete $1; }
     | '-' expr %prec tUNARY { $$ = new cdk::unary_minus_node(LINE, $2); }
     | '+' expr %prec tUNARY { $$ = new cdk::unary_plus_node(LINE, $2); }
     | tNOT expr             { $$ = new cdk::not_node(LINE, $2); }
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

exprs : exprs ',' expr            { $$ = new cdk::sequence_node(LINE, $3, $1); }
      | expr                  { $$ = new cdk::sequence_node(LINE, $1); }
      ;

lval : tIDENTIFIER             { $$ = new cdk::variable_node(LINE, $1); }
     ; 

string : string tSTRING { $$ = $1; $$->append(*$2); delete $2; }
       | tSTRING        { $$ = $1; }
       ;

type : tTYPE_INT    { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
     | tTYPE_REAL   { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
     | tTYPE_STRING { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
     | tTYPE_VOID   { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
     ;

arg : type tIDENTIFIER { $$ = new udf::var_declaration_node(LINE, 0, *$2, false, $1, nullptr); }
    ;

args : args ',' arg { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | arg          { $$ = new cdk::sequence_node(LINE, $1); }
     ;

blck : '{' declrs instrs '}' { $$ = new udf::block_node(LINE, $2, $3); }
     | '{' declrs '}' { $$ = new udf::block_node(LINE, $2, nullptr); }
     | '{' instrs '}' { $$ = new udf::block_node(LINE, nullptr, $2); }
     ;

declrs : declrs declr    { $$ = new cdk::sequence_node(LINE, $2, $1); }
       | declr           { $$ = new cdk::sequence_node(LINE, $1); }
       ; 

declr : type tIDENTIFIER                    ';'
               { $$ = new udf::var_declaration_node(LINE, 0, *$2, false, $1, nullptr); delete $2; }
       | type tIDENTIFIER '=' expr           ';'
               { $$ = new udf::var_declaration_node(LINE, 0, *$2, false, $1, $4); delete $2; }
       | tPUBLIC type tIDENTIFIER            ';'
               { $$ = new udf::var_declaration_node(LINE, 1, *$3, false, $2, nullptr); delete $3; }
       | tPUBLIC type tIDENTIFIER '=' expr   ';'
               { $$ = new udf::var_declaration_node(LINE, 1, *$3, false, $2, $5); delete $3; }
       | tFORWARD type tIDENTIFIER           ';'
               { $$ = new udf::var_declaration_node(LINE, 2, *$3, false, $2, nullptr); delete $3; }
       | tFORWARD type tIDENTIFIER '=' expr  ';'
               { $$ = new udf::var_declaration_node(LINE, 2, *$3, false, $2, $5); delete $3; }
       | tAUTO tIDENTIFIER '=' expr          ';'
               { $$ = new udf::var_declaration_node(LINE, 0, *$2, true, nullptr, $4); delete $2; }
       | tPUBLIC tAUTO tIDENTIFIER '=' expr  ';'
               { $$ = new udf::var_declaration_node(LINE, 1, *$3, true, nullptr, $5); delete $3; }
       | type tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 0, *$2, false, nullptr, $1, $5); delete $2; }
       | type tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 0, *$2, false, $4, $1, $6); delete $2; }
       | tPUBLIC type tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 1, *$3, false, nullptr, $2, $6); delete $3; }
       | tPUBLIC type tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 1, *$3, false, $5, $2, $7); delete $3; }
       | tFORWARD type tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 2, *$3, false, nullptr, $2, $6); delete $3; }
       | tFORWARD type tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 2, *$3, false, $5, $2, $7); delete $3; }
       | tAUTO tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 0, *$2, true, nullptr, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $5); delete $2; }
       | tAUTO tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 0, *$2, true, $4, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $6); delete $2; }
       | tPUBLIC tAUTO tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 1, *$3, true, nullptr, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $6); delete $3; }
       | tPUBLIC tAUTO tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 1, *$3, true, $5, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $7); delete $3; }
       | tFORWARD tAUTO tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 2, *$3, true, nullptr, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $6); delete $3; }
       | tFORWARD tAUTO tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 2, *$3, true, $5, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $7); delete $3; }
               // void can be omitted
       | tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 0, *$1, false, nullptr, cdk::primitive_type::create(0, cdk::TYPE_VOID), $4); delete $1; }
       | tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 0, *$1, false, $3, cdk::primitive_type::create(0, cdk::TYPE_VOID), $5); delete $1; }
       | tPUBLIC tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 1, *$2, false, nullptr, cdk::primitive_type::create(0, cdk::TYPE_VOID), $5); delete $2; }
       | tPUBLIC tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 1, *$2, false, $4, cdk::primitive_type::create(0, cdk::TYPE_VOID), $6); delete $2; }
       | tFORWARD tIDENTIFIER '(' ')' blck
               { $$ = new udf::function_node(LINE, 2, *$2, false, nullptr, cdk::primitive_type::create(0, cdk::TYPE_VOID), $5); delete $2; }
       | tFORWARD tIDENTIFIER '(' args ')' blck
               { $$ = new udf::function_node(LINE, 2, *$2, false, $4, cdk::primitive_type::create(0, cdk::TYPE_VOID), $6); delete $2; }
       ;

instrs : instrs instr    { $$ = new cdk::sequence_node(LINE, $2, $1); }
       | instr           { $$ = new cdk::sequence_node(LINE, $1); }
       ;

instr : expr ';'              { $$ = new udf::evaluation_node(LINE, $1); }
      | tWRITE exprs ';'      { $$ = new udf::write_node(LINE, $2, false); }
      | tWRITELN exprs ';'    { $$ = new udf::write_node(LINE, $2, true); }
      | tBREAK ';'            { $$ = new udf::break_node(LINE); }
      | tCONTINUE ';'         { $$ = new udf::continue_node(LINE); }
      | tRETURN expr ';'      { $$ = new udf::return_node(LINE, $2); }
      | tRETURN ';'           { $$ = new udf::return_node(LINE, nullptr); }
      ;

%%
