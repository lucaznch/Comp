#pragma once

#include <cdk/ast/basic_node.h>

namespace udf {

  /**
   * Class for describing program nodes.
   */
  class program_node : public cdk::basic_node {
    cdk::sequence_node *_declarations;

  public:
    program_node(int lineno, cdk::sequence_node *declarations) :
        cdk::basic_node(lineno), _declarations(declarations) {
    }

    cdk::sequence_node *declarations() { return _declarations; }

    void accept(basic_ast_visitor *sp, int level) { sp->do_program_node(this, level); }

  };

} // udf
