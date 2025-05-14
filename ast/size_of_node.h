#pragma once

#include <cdk/ast/unary_operation_node.h>

namespace udf {
  /**
   * Class for describing size_of_node nodes.
   */
    class size_of_node : public cdk::unary_operation_node {
    public:
        size_of_node(int lineno, cdk::expression_node *arg) noexcept:
            cdk::unary_operation_node(lineno, arg) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_size_of_node(this, level); } 
    };
}
