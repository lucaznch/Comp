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
  double                d;          /* double value */
  std::string          *s;          /* symbol name or string literal */
  cdk::basic_node      *node;       /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;
  udf::block_node      *block;
  udf::for_node      *loop;
};

%token <i> tINTEGER
%token <d> tREAL
%token <s> tIDENTIFIER tSTRING
%token tIF
%token tTYPE_INT tTYPE_REAL tTYPE_STRING tTYPE_VOID tTYPE_POINTER tTYPE_TENSOR
%token tPUBLIC tFORWARD tAUTO
%token tFOR tBREAK tCONTINUE tRETURN
%token tINPUT
%token tSIZEOF
%token tWRITE tWRITELN
%token tAND tOR
%token tCONTRACTION tDIM tDIMS tRESHAPE tRANK tCAPACITY
%token tOBJECTS tNULLPTR
%nonassoc tIFX
%nonassoc tELIF tELSE

%right '='
%left tOR
%left tAND
%left tGE tLE tEQ tNE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%left '[' ']' 
%left '?' 
%left tCONTRACTION tDIM tDIMS tRESHAPE tRANK tCAPACITY 
%right '@'
%nonassoc tUNARY 

%type <node> program arg declr instr elif fn_declr var_declr
%type <sequence> exprs args declrs instrs opt_exprs opt_loop_declrs var_declrs dims  list tensor
%type <expression> expr dim 
%type <lvalue> lval
%type <block> blck
%type <loop> for
%type <type> type
%type <s> string 

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

program : declrs { compiler->ast(new udf::program_node(LINE, $1)); }
        ;

expr : tINTEGER                    { $$ = new cdk::integer_node(LINE, $1); }
     | tREAL                       { $$ = new cdk::double_node(LINE, $1); }
     | string                      { $$ = new cdk::string_node(LINE, $1); delete $1; }
     | tNULLPTR                    { $$ = new udf::nullptr_node(LINE);}
     | '-' expr %prec tUNARY       { $$ = new cdk::unary_minus_node(LINE, $2); }
     | '+' expr %prec tUNARY       { $$ = new cdk::unary_plus_node(LINE, $2); }
     | '~' expr %prec tUNARY       { $$ = new cdk::not_node(LINE, $2); }
     | expr tDIM '(' expr ')'      { $$ = new udf::dim_node(LINE, $1,$4); }
     | expr tDIMS                  { $$ =  new udf::dims_node(LINE, $1); }
     | expr tRESHAPE '(' dims ')'  { $$ = new udf::reshape_node(LINE, $1,$4); }
     | expr tRANK                  { $$ =  new udf::rank_node(LINE, $1); }
     | expr tCAPACITY              { $$ =  new udf::capacity_node(LINE, $1); }
     | expr '?'                    { $$ = new udf::address_of_node(LINE, $1); }
     | tOBJECTS  '(' expr ')'      { $$ = new udf::malloc_node(LINE, $3);}
     | tINPUT                      { $$ = new udf::input_node(LINE); }
     | expr '+' expr               { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr               { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr               { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '/' expr               { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '%' expr               { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '<' expr               { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr               { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tAND expr              { $$ = new cdk::and_node(LINE, $1, $3); }
     | expr tOR expr               { $$ = new cdk::or_node(LINE, $1, $3); }
     | expr tGE expr               { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr               { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr               { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr               { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tCONTRACTION expr      { $$ = new udf::contraction_node(LINE, $1, $3); }
     | '(' expr ')'                { $$ = $2; }
     | tSIZEOF '(' expr ')'        { $$ = new udf::size_of_node(LINE, $3); }
     | tIDENTIFIER '(' ')'         { $$ = new udf::function_call_node(LINE, *$1, nullptr); delete $1; }
     | tIDENTIFIER '(' exprs ')'   { $$ = new udf::function_call_node(LINE, *$1, $3); delete $1; }      
     | tensor
        {
              std::vector<size_t> shape = udf::tensor_node::compute_shape($1);
              auto type = cdk::tensor_type::create(shape);
              $$ = new udf::tensor_node(LINE, *type, $1);
        }
     | lval                        { $$ = new cdk::rvalue_node(LINE, $1); }
     | lval '=' expr               { $$ = new cdk::assignment_node(LINE, $1, $3); }
     ;

exprs : exprs ',' expr             { $$ = new cdk::sequence_node(LINE, $3, $1); }
      | expr                       { $$ = new cdk::sequence_node(LINE, $1); }
      ;

lval : tIDENTIFIER             { $$ = new cdk::variable_node(LINE, $1); }
        | expr '[' expr ']'     {$$ = new udf::index_node(LINE,$1,$3);}
        | expr '@' '(' exprs ')' { $$ = new udf::tensor_indexation_node(LINE, $1, $4); }
     ; 

string : string tSTRING     { $$ = $1; $$->append(*$2); delete $2; }
       | tSTRING            { $$ = $1; }
       ;

type : tTYPE_INT                   { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
     | tTYPE_REAL                  { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
     | tTYPE_STRING                { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
     | tTYPE_VOID                  { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
     | tTYPE_POINTER '<' type '>'  { $$ = cdk::reference_type::create(4, $3); }
     | tTYPE_POINTER '<' tAUTO '>' { $$ = cdk::reference_type::create(4, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC)); }  
     | tTYPE_TENSOR '<' dims '>'
        {    
          std::vector<size_t> dimensions;
          auto seq = dynamic_cast<cdk::sequence_node*>($3);
          if (seq) {
              for (size_t i = 0; i < seq->size(); ++i) {
                  auto dim_node = dynamic_cast<cdk::integer_node*>(seq->node(i));
                  if (dim_node) { dimensions.push_back(dim_node->value()); }
                  else { printf("Error: Invalid dimension in tensor type\n"); }
              }
          }
          $$ = cdk::tensor_type::create(dimensions);
        }
     ;

list : list ',' expr                    { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | expr                             { $$ = new cdk::sequence_node(LINE, $1); }
     ;

tensor : '[' list ']'                   { $$ = $2; }

dim : tINTEGER                          { $$ = new cdk::integer_node(LINE, $1); }
    ;

dims : dims ',' dim                     { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | dim                              { $$ = new cdk::sequence_node(LINE, $1); }
     ;

arg : type tIDENTIFIER                  { $$ = new udf::var_declaration_node(LINE, 0, *$2, $1, nullptr); }
    ;

args : args ',' arg                     { $$ = new cdk::sequence_node(LINE, $3, $1); }
     | arg                              { $$ = new cdk::sequence_node(LINE, $1); }
     ;

blck : '{' var_declrs ';' instrs '}'    { $$ = new udf::block_node(LINE, $2, $4); }
     | '{' var_declrs ';' '}'           { $$ = new udf::block_node(LINE, $2, nullptr); }
     | '{' instrs '}'                   { $$ = new udf::block_node(LINE, nullptr, $2); }
     | '{' '}'                          { $$ = new udf::block_node(LINE, nullptr, nullptr); }
     ;


var_declrs : var_declr                  { $$ = new cdk::sequence_node(LINE, $1); }
        |var_declrs ';' var_declr       { $$ = new cdk::sequence_node(LINE, $3, $1); }  
       ; 

declrs : declrs declr                   { $$ = new cdk::sequence_node(LINE, $2, $1); }
       | declr                          { $$ = new cdk::sequence_node(LINE, $1); }
       ; 

declr : var_declr ';'                   { $$=$1; }
      | fn_declr                        { $$=$1; }
      ;

var_declr : type tIDENTIFIER                    
                { $$ = new udf::var_declaration_node(LINE, 0, *$2, $1, nullptr); delete $2; }

          | type tIDENTIFIER '=' expr           
                { $$ = new udf::var_declaration_node(LINE, 0, *$2, $1, $4); delete $2; }

          | tPUBLIC type tIDENTIFIER            
                { $$ = new udf::var_declaration_node(LINE, 1, *$3, $2, nullptr); delete $3; }

          | tPUBLIC type tIDENTIFIER '=' expr   
                { $$ = new udf::var_declaration_node(LINE, 1, *$3, $2, $5); delete $3; }

          | tFORWARD type tIDENTIFIER           
                { $$ = new udf::var_declaration_node(LINE, 2, *$3, $2, nullptr); delete $3; }

          | tFORWARD type tIDENTIFIER '=' expr  
                { $$ = new udf::var_declaration_node(LINE, 2, *$3, $2, $5); delete $3; }

          | tAUTO tIDENTIFIER '=' expr          
                { $$ = new udf::var_declaration_node(LINE, 0, *$2, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $4); delete $2; }

          | tPUBLIC tAUTO tIDENTIFIER '=' expr  
                { $$ = new udf::var_declaration_node(LINE, 1, *$3, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $5); delete $3; }
          ;

fn_declr : type tIDENTIFIER '(' ')' blck
                { $$ = new udf::function_node(LINE, 0, *$2, nullptr, $1, $5); delete $2; }

         | type tIDENTIFIER '(' args ')' blck
                { $$ = new udf::function_node(LINE, 0, *$2, $4, $1, $6); delete $2; }

         | tPUBLIC type tIDENTIFIER '(' ')' blck
                { $$ = new udf::function_node(LINE, 1, *$3, nullptr, $2, $6); delete $3; }

         | tPUBLIC type tIDENTIFIER '(' args ')' blck
                { $$ = new udf::function_node(LINE, 1, *$3, $5, $2, $7); delete $3; }

         | tAUTO tIDENTIFIER '(' ')' blck
                { $$ = new udf::function_node(LINE, 0, *$2, nullptr, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $5); delete $2; }

         | tAUTO tIDENTIFIER '(' args ')' blck
                { $$ = new udf::function_node(LINE, 0, *$2, $4, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $6); delete $2; }

         | tPUBLIC tAUTO tIDENTIFIER '(' ')' blck
                { $$ = new udf::function_node(LINE, 1, *$3, nullptr, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $6); delete $3; }

         | tPUBLIC tAUTO tIDENTIFIER '(' args ')' blck
                { $$ = new udf::function_node(LINE, 1, *$3, $5, cdk::primitive_type::create(0, cdk::TYPE_UNSPEC), $7); delete $3; }

         | tFORWARD type tIDENTIFIER '(' args ')'
                { $$ = new udf::function_node(LINE, 2, *$3, $5, $2, nullptr); delete $3; }

         | tFORWARD type tIDENTIFIER '(' ')'
                { $$ = new udf::function_node(LINE, 2, *$3, nullptr, $2, nullptr); delete $3; }

         ;

instrs : instrs instr    { $$ = new cdk::sequence_node(LINE, $2, $1); }
       | instr           { $$ = new cdk::sequence_node(LINE, $1); }
       ;

instr : expr ';'                          { $$ = new udf::evaluation_node(LINE, $1); }
      | tWRITE exprs ';'                  { $$ = new udf::write_node(LINE, $2, false); }
      | tWRITELN exprs ';'                { $$ = new udf::write_node(LINE, $2, true); }
      | tBREAK ';'                        { $$ = new udf::break_node(LINE); }
      | tCONTINUE ';'                     { $$ = new udf::continue_node(LINE); }
      | tRETURN expr ';'                  { $$ = new udf::return_node(LINE, $2); }
      | tRETURN ';'                       { $$ = new udf::return_node(LINE, nullptr); }
      | tIF '(' expr ')' instr %prec tIFX { $$ = new udf::if_node(LINE, $3, $5); }
      | tIF '(' expr ')' instr elif       { $$ = new udf::if_else_node(LINE, $3, $5, $6); }
      | blck                              { $$ = $1; }
      | for                               { $$ = $1; }
      | tCONTINUE                         { $$ = new udf::continue_node(LINE); }
      | tBREAK                            { $$ = new udf::break_node(LINE); }
      ;

elif : tELSE instr                          { $$ = $2; }
     | tELIF '(' expr ')' instr %prec tIFX  { $$ = new udf::if_node(LINE, $3, $5); }
     | tELIF '(' expr ')' instr elif        { $$ = new udf::if_else_node(LINE, $3, $5, $6); }
     ;

for : tFOR '(' opt_loop_declrs ';' opt_exprs ';' opt_exprs')' instr
        { $$ = new udf::for_node(LINE,$3,$5,$7,$9); }

    | tFOR '(' exprs ';' opt_exprs ';' opt_exprs')' instr
        { $$ = new udf::for_node(LINE,$3,$5,$7,$9); }

    | tFOR '('  ';' opt_exprs ';' opt_exprs')' instr
        { $$ = new udf::for_node(LINE,nullptr,$4,$6,$8); }
    ;

opt_loop_declrs : opt_loop_declrs',' var_declr  { $$ = new cdk::sequence_node(LINE, $3, $1); }
                | var_declr                     { $$ = new cdk::sequence_node(LINE, $1); }
                ; 

opt_exprs : exprs       { $$ = $1; }
          |             { $$ = nullptr; }
          ;
%%
