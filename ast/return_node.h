#pragma once

#include <cdk/ast/expression_node.h>

namespace udf {

    class return_node : public cdk::basic_node {
        cdk::expression_node *_retvalue;

    public:
        return_node(int lineno, cdk::expression_node *retvalue) :
            cdk::basic_node(lineno), _retvalue(retvalue) {
        }

        cdk::expression_node *retvalue() {
            return _retvalue;
        }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_return_node(this, level);
        }
    };

} // udf

