#pragma once

#include <cdk/ast/literal_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/types/tensor_type.h>

namespace udf{
    class tensor_node : public cdk::literal_node<cdk::tensor_type>{
        cdk::sequence_node *_nested_sequences;

/*
    int compute_tensor_dimensions(cdk::sequence_node* nested_sequences, int dims_acc) {
        _dims[dims_acc] = nested_sequences->size();
        if(nested_sequences->node(0)->label() == nested_sequences->label())
            return compute_tensor_dimensions(nested_sequences->node(0), dims_acc+1);
        else
            return dims_acc;
    }
*/
    public:
        tensor_node(int lineno, const cdk::tensor_type &type, cdk::sequence_node *nested_sequences)
        : cdk::literal_node<cdk::tensor_type>(lineno, type),_nested_sequences(nested_sequences) {}



        cdk::sequence_node * nested_sequences() const {return _nested_sequences;}

        void accept(basic_ast_visitor *sp, int level) { sp->do_tensor_node(this, level); } 


    };


}