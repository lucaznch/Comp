#pragma once

#include <cdk/ast/expression_node.h>

namespace udf {
  /**
   * Class for describing rank_node nodes.
   */
    class rank_node : public cdk::expression_node {
        std::string _identifier;
    public:
        rank_node(int lineno, const std::string &identifier) :
            cdk::expression_node(lineno), _identifier(identifier) {}

        void accept(basic_ast_visitor *sp, int level) { sp->do_rank_node(this, level); } 
    };
}
