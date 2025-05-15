#pragma once

#include <cdk/ast/unary_operation_node.h>

namespace udf {
  /**
   * Class for describing dims_node nodes.
   */
    class dims_node : public cdk::unary_operation_node {
    public:
        dims_node(int lineno, cdk::expression_node *arg):
            cdk::unary_operation_node(lineno, arg) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_dims_node(this, level); } 
    };
}
