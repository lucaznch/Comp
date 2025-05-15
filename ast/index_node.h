#pragma once

#include <cdk/ast/binary_operation_node.h>

namespace udf {
  /**
   * Class for describing index_node nodes.
   */
    class index_node : public cdk::binary_operation_node {
    public:
        index_node(int lineno, cdk::expression_node *ptr, cdk::expression_node *index):
            cdk::binary_operation_node(lineno,ptr,index) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_index_node(this, level); } 
    };
}
