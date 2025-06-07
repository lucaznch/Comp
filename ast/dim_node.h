#pragma once

#include <cdk/ast/binary_operation_node.h>

namespace udf {
  /**
   * Class for describing dim_node nodes.
   */
    class dim_node : public cdk::binary_operation_node {
    public:
        dim_node(int lineno, cdk::expression_node *left, cdk::expression_node *right):
            cdk::binary_operation_node(lineno, left, right) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_dim_node(this, level); } 
    };
}
