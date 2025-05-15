#pragma once

#include <cdk/ast/expression_node.h>

namespace udf {
  /**
   * Class for describing index_node nodes.
   */
    class tensor_indexation_node : public cdk::expression_node {
    cdk::expression_node *_tensor; // (t?)[0]@(1,2); ([1,2],[3,4]**[[1,2],[3,4]])@(1,2))
    cdk::sequence_node *_indexes;

    public:
        tensor_indexation_node(int lineno, cdk::expression_node *tensor, cdk::sequence_node *indexes):
            cdk::expression_node(lineno),_tensor(tensor),_indexes(indexes) {}

        cdk::expression_node* tensor() const { return _tensor; }
        cdk::sequence_node* indexes() const { return _indexes; }

        void accept(basic_ast_visitor *sp, int level) { sp->do_tensor_indexation_node(this, level); } 
    };
}
