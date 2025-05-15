#pragma once

#include <cdk/ast/expression_node.h>

namespace udf {

class for_node : public cdk::basic_node {
    cdk::expression_node* _init;
    cdk::expression_node* _condition;
    cdk::expression_node* _step;
    block_node* _block;

public:
    for_node(int lineno, cdk::expression_node* init, cdk::expression_node* condition,
             cdk::expression_node* step, block_node* block)
        : cdk::basic_node(lineno), _init(init), _condition(condition), _step(step), _block(block) {}

    cdk::expression_node* init() {
        return _init;
    }

    cdk::expression_node* condition() {
        return _condition;
    }

    cdk::expression_node* step() {
        return _step;
    }

    block_node* block() {
        return _block;
    }

    void accept(basic_ast_visitor* sp, int level) {
        sp->do_for_node(this, level);
    }
};

}  // namespace udf
