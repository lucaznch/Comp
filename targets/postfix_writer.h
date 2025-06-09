#pragma once

#include "targets/basic_ast_visitor.h"

#include <sstream>
#include <cdk/emitters/basic_postfix_emitter.h>
#include <stack>

namespace udf {

  //!
  //! Traverse syntax tree and generate the corresponding assembly code.
  //!
  class postfix_writer: public basic_ast_visitor {
    enum class Context { Args, Body, Global };
    cdk::symbol_table<udf::symbol> &_symtab;
    cdk::basic_postfix_emitter &_pf;
    int _lbl;
    int _offset;
    std::shared_ptr<udf::symbol> _function;
    std::vector<int> _forCond, _forStep, _forEnd; // for break/continue
    Context _context;
    bool _errors;
    std::stack<std::string> _segments; //consider declaring a string literal inside a function definition, and possibly more complicated cases (not sure), this way we dont need to track the segments types ourselves, the stack handles it
  public:
    postfix_writer(std::shared_ptr<cdk::compiler> compiler, cdk::symbol_table<udf::symbol> &symtab,
                   cdk::basic_postfix_emitter &pf) :
        basic_ast_visitor(compiler), _symtab(symtab), _pf(pf), _lbl(0),_offset(0),_context(Context::Global),_errors(false) {
    }

  public:
    ~postfix_writer() {
      os().flush();
    }

  private:
    /** Method used to generate sequential labels. */
    inline std::string mklbl(int lbl) {
      std::ostringstream oss;
      if (lbl < 0)
        oss << ".L" << -lbl;
      else
        oss << "_L" << lbl;
      return std::move(oss).str();
    }

  public:
  // do not edit these lines
#define __IN_VISITOR_HEADER__
#include ".auto/visitor_decls.h"       // automatically generated
#undef __IN_VISITOR_HEADER__
  // do not edit these lines: end

  };

} // udf
