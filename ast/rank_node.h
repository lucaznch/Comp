#pragma once

#include <cdk/ast/unary_operation_node.h>

namespace udf {
  /**
   * Class for describing rank_node nodes.
   */
    class rank_node : public cdk::unary_operation_node {
    public:
        rank_node(int lineno, cdk::expression_node *arg):
            cdk::unary_operation_node(lineno, arg) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_rank_node(this, level); } 
    };
}
