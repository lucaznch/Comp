#pragma once

#include <cdk/ast/typed_node.h>

namespace udf {
    /**
    * Class for describing declaration nodes.
    */

    class declaration_node : public cdk::basic_node {
        int _access; // private forward public
        std::string _name;
        cdk::typed_node * _declared; //etiher a varaible or a function, both typed

    public:
        declaration_node(int lineno, int access, std::string name, cdk::typed_node *declared) noexcept:
        cdk::basic_node(lineno), _access(access), _name(name), _declared(declared){}

        const std::string &name() const {return _name;}
        int access() {return _access;}
        cdk::typed_node *declared() { return _declared;}

        void accept(basic_ast_visitor *sp, int level) override { sp->do_declaration_node(this, level); }

    };
}
