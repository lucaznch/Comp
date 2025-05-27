#pragma once

#include <cdk/ast/typed_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/types/basic_type.h>
#include <cdk/types/functional_type.h>
#include "block_node.h"

#include <string>

namespace udf {

    /**
     * Class for describing function nodes.
     *
     * UDF language defines a function, like:
     *      function = [public|forward] (type|auto) identifier ([variables]) [block]
     * 
     *  a qualifier can be:
     *      - public: it is accessible from outside the module
     *          - e.g., public real n = 22;
     *
     *      - forward: allows the declaration of entities in a module that are defined in other modules
     *          - e.g., forward real n;
     *       
     *      - if no qualifier is specified, the default is private where:
     *          - it is only accessible from the module where it is defined
     *          - e.g., real n = 22;
     * 
     *  a type can be:
     *      - int
     *      - real
     *      - string
     *      - ptr
     *      - tensor
     *      - auto: the type is inferred from the initializer
     * 
     *  an identifier is the name of the function
     *      - if the function name is "udf" and has no arguments, it is the main function that initiates the program
     *          - e.g., public int udf() { return 0; }
     * 
     */
    class function_node: public cdk::typed_node {
        int _qualifier;    // 0 = private, 1 = public, 2 = forward
        std::string _identifier;
        bool _is_auto;
        cdk::sequence_node * _args;
        udf::block_node *_block;


    public:

        /**
         * Constructor for function nodes.
         *
         * @param lineno the source code line number that originated the node
         * @param qualifier the qualifier of the function
         * @param identifier the name of the function
         * @param is_auto true if it is an auto function, false otherwise
         * @param args the arguments of the function
         * @param return_type the return type of the function
         * @param block the block of code in the function
         */
        function_node(int lineno,
                int qualifier,
                const std::string &identifier,
                bool is_auto,
                cdk::sequence_node *args,
                std::shared_ptr<cdk::basic_type> return_type,
                udf::block_node *block) :
            cdk::typed_node(lineno),
            _qualifier(qualifier),
            _identifier(identifier),
            _is_auto(is_auto),
            _args(args),
            _block(block) {
                if(!args)
                    this->type(cdk::functional_type::create(return_type));
                else{
                    std::vector<std::shared_ptr<cdk::basic_type>> arg_types;
                    for (size_t i = 0; i < args->size(); i++) {
                        auto *arg = dynamic_cast<cdk::typed_node*>(args->node(i));
                        arg_types.push_back(arg->type());
                    }
                    this->type(cdk::functional_type::create(arg_types, return_type));
                }
        }

        // Getters
        int qualifier() { return _qualifier; }

        const std::string &identifier() { return _identifier; }

        bool is_main() const { return _identifier =="udf"; }

        bool is_auto() { return _is_auto; }

        cdk::sequence_node *args() { return _args; }

        udf::block_node *block() { return _block; }


        // Visitor pattern
        void accept(basic_ast_visitor *sp, int level) { sp->do_function_node(this, level); }
    };

} // udf

