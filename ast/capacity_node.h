#pragma once

#include <cdk/ast/unary_operation_node.h>

namespace udf {
  /**
   * Class for describing capacity_node nodes.
   */
    class capacity_node : public cdk::unary_operation_node {
    public:
        capacity_node(int lineno, cdk::expression_node *arg):
            cdk::unary_operation_node(lineno, arg) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_capacity_node(this, level); } 
    };
}
