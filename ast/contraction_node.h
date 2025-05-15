#pragma once

#include <cdk/ast/binary_expression_node.h>

namespace udf {
  /**
   * Class for describing contraction_node nodes.
   */
    class contraction_node : public cdk::binary_expression_node {

    public:
    contraction_node(int lineno, cdk::expression_node *t1, cdk::expression_node *t2):
            cdk::binary_expression_node(lineno,t1,t2) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_contraction_node(this, level); } 
    };
}
