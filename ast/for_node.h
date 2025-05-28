#pragma once

#include <cdk/ast/expression_node.h>

namespace udf {

class for_node : public cdk::basic_node {
    cdk::sequence_node* _inits;
    cdk::sequence_node* _conditions;
    cdk::sequence_node* _steps;
    basic_node* _instr;

public:
    for_node(int lineno, cdk::sequence_node* inits, cdk::sequence_node* conditions,
             cdk::sequence_node* steps, basic_node* instr)
        : cdk::basic_node(lineno), _inits(inits), _conditions(conditions), _steps(steps), _instr(instr) {}

    cdk::sequence_node* inits() {
        return _inits;
    }

    cdk::sequence_node* conditions() {
        return _conditions;
    }

    cdk::sequence_node* steps() {
        return _steps;
    }

    basic_node* instr() {
        return _instr;
    }

    void accept(basic_ast_visitor* sp, int level) {
        sp->do_for_node(this, level);
    }
};

}  // namespace udf
