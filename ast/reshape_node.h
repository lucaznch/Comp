#pragma once

#include <cdk/ast/expression_node.h>

namespace udf {
  /**
   * Class for describing reshape_node nodes.
   */
    class reshape_node : public cdk::expression_node {
        cdk::expression_node *_tensor;
        cdk::sequence_node *_sizes;

    public:
        reshape_node(int lineno, cdk::expression_node *tensor, cdk::sequence_node *sizes):
            cdk::expression_node(lineno), _tensor(tensor), _sizes(sizes) {}

        cdk::expression_node* tensor() const { return _tensor; }
        cdk::sequence_node* sizes() const { return _sizes; }

        void accept(basic_ast_visitor *sp, int level) { sp->do_reshape_node(this, level); } 
    };
}
