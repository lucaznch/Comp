#pragma once

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

#include <string>

namespace udf {

    /**
     * Class for describing function call nodes.
     */
    class function_call_node : public cdk::expression_node {
        std::string _identifier;
        cdk::sequence_node *_args;
    
    public:
        function_call_node(int lineno, const std::string &identifier, cdk::sequence_node *args) :
            cdk::expression_node(lineno), _identifier(identifier), _args(args) {
        }
    
        const std::string &identifier() const {
            return _identifier;
        }
    
        cdk::sequence_node *arguments() {
            return _args;
        }
    
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_call_node(this, level);
        }
    
    };

} // udf

