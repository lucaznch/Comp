#pragma once

#include <cdk/ast/unary_operation_node.h>

namespace udf {

  /**
   * Class for describing malloc nodes.
   */
  class malloc_node : public cdk::unary_operation_node {

  public:
    malloc_node(int lineno, cdk::expression_node *arg) noexcept:
        cdk::unary_operation_node(lineno, arg) {}

    void accept(basic_ast_visitor *sp, int level) { sp->do_malloc_node(this, level); }

  };

} // udf
