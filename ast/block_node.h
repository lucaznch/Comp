#pragma once

#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>

namespace udf {

    /**
     * Class for describing block nodes.
     */
    class block_node : public cdk::basic_node {
        cdk::sequence_node *_declarations, *_instructions;

    public:

        /**
         * Constructor for block nodes.
         *
         * @param lineno the source code line number that originated the node
         * @param declarations the declarations in the block
         * @param instructions the instructions in the block
         */
        block_node(int lineno, cdk::sequence_node *declarations, cdk::sequence_node *instructions) :
            cdk::basic_node(lineno), _declarations(declarations), _instructions(instructions) {
        }

        // Getters
        cdk::sequence_node *declarations() { return _declarations; }
        cdk::sequence_node *instructions() { return _instructions; }

        // No need for Setters

        // Visitor pattern
        void accept(basic_ast_visitor *sp, int level) { sp->do_block_node(this, level); }

    };

} // udf
