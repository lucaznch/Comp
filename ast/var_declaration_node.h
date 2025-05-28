#pragma once

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/types/basic_type.h>

#include <string>

namespace udf {

    /**
     * Class for describing variable declaration nodes.
     *
     * UDF language defines a variable like:
     *  - [ public | forward ] type identifier [ = expression ]
     *  - [ public ] auto identifier = expression
     */
    class var_declaration_node : public cdk::typed_node {
        int _qualifier;  // 0 = private, 1 = public, 2 = forward
        std::string _identifier;
        cdk::expression_node *_initializer;

        
    public:
        var_declaration_node(int lineno,
                            int qualifier,
                            const std::string &identifier,
                            std::shared_ptr<cdk::basic_type> type,
                            cdk::expression_node *initializer = nullptr) :
            cdk::typed_node(lineno),
            _qualifier(qualifier),
            _identifier(identifier),
            _initializer(initializer) {
                this->type(type);
        }

        // Getters
        int qualifier() const { return _qualifier; }

        const std::string &identifier() const { return _identifier; }

        bool is_auto()  { return this->type()->name() == cdk::TYPE_UNSPEC; }

        cdk::expression_node *initializer() const { return _initializer; }


        void accept(basic_ast_visitor *sp, int level) {
            sp->do_var_declaration_node(this, level);
        }
    };

}

