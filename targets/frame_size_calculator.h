#pragma once
#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <stack>

namespace udf {

  class frame_size_calculator: public basic_ast_visitor {
    cdk::symbol_table<udf::symbol> &_symtab;
    //O TIL permitia declarar funções umas dentro das outras, acho que para nós isto não é uma preocupaçãp
    //std::stack<std::shared_ptr<til::symbol>> _functions; // for keeping track of the current function and its arguments
    std::shared_ptr<udf::symbol> _function;

    size_t _localsize;

  public:
    frame_size_calculator(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<udf::symbol> &symtab, std::shared_ptr<udf::symbol> function) :
        basic_ast_visitor(compiler), _symtab(symtab), _function(function), _localsize(0) {
    }

  public:
    ~frame_size_calculator();

  public:
    size_t localsize() const {
      return _localsize;
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // udf

#
