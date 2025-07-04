#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void udf::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  // EMPTY
}

void udf::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void udf::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    //auto
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("wrong type in unary logical expression");
  }
}

void udf::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (node->left()->type() != node->right()->type()) {
    throw std::string("same type expected on both sides of logical operator");
  }
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (node->left()->type() != node->right()->type()) {
    throw std::string("same type expected on both sides of logical operator");
  }
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void udf::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------

void udf::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
 }

void udf::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void udf::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

//---------------------------------------------------------------------------

void udf::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of unary expression");

  // in udf, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void udf::type_checker::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void udf::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  // if left is pointer and right is not int, throw error
  if (node->left()->is_typed(cdk::TYPE_POINTER) && !node->right()->is_typed(cdk::TYPE_INT))
    throw std::string("invalid pointer addition: right operand must be int");
  // if right is pointer and left is not int, throw error
  if (!node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER))
    throw std::string("invalid pointer addition: left operand must be int");
  // if both are double assign double type
  if (node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  }
  // if left is pointer and right is int assign pointer type
  else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    auto left_ptr = cdk::reference_type::cast(node->left()->type());
    node->type(cdk::reference_type::create(4, left_ptr->referenced()));  // pointer arithmetic
  }
  // if left is int and right is pointer assign pointer type
  else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    auto right_ptr = cdk::reference_type::cast(node->right()->type());
    node->type(cdk::reference_type::create(4, right_ptr->referenced())); // pointer arithmetic
  }
  else {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  }
}

void udf::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if(node->left()->is_typed(cdk::TYPE_POINTER) && !node->right()->is_typed(cdk::TYPE_POINTER) && !node->right()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of binary expression");
  if(!node->left()->is_typed(cdk::TYPE_POINTER) && !node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in argument of binary expression");

  if (!node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_POINTER) ) throw std::string("wrong type in argument of binary expression");
  if (!node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in arugment of binary expression");

  if(node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else if( (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) || (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)))
    node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
  else
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of binary expression");
  if (!node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of binary expression");

  if(node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of binary expression");
  if (!node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->right()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of binary expression");

  if(node->left()->is_typed(cdk::TYPE_DOUBLE) || node->right()->is_typed(cdk::TYPE_DOUBLE))
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  else node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT) ) throw std::string("wrong type in argument of binary expression");
  if (!node->right()->is_typed(cdk::TYPE_INT) ) throw std::string("wrong type in argument of binary expression");
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void udf::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  if(!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  if(!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  if(!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  if(!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in argument of binary expression");
  if(!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in argument of binary expression");
  if(node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in argument of binary expression");

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  if(!node->left()->is_typed(cdk::TYPE_INT) && !node->left()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in argument of binary expression");
  if(!node->right()->is_typed(cdk::TYPE_INT) && !node->right()->is_typed(cdk::TYPE_DOUBLE) && !node->left()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in argument of binary expression");
  if(node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_DOUBLE)) throw std::string("wrong type in argument of binary expression");
  if(node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_POINTER)) throw std::string("wrong type in argument of binary expression");

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void udf::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC;
  const std::string &id = node->name();
  std::shared_ptr<udf::symbol> symbol = _symtab.find(id);
  if (symbol != nullptr) {
    node->type(symbol->type());
    std::cout << node->lineno() << ": "<< id <<": do_variable_node::symbol\n" ;
  } else {
    std::cout << node->lineno() << ": "<< id <<": do_variable_node::symbol undef\n" ;
    throw id;
  }
}

void udf::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC;
  try {
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void udf::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);

  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else {
      throw std::string("wrong assignment to integer");
    }
  } else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {

    //TODO: check pointer level
    if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      node->type(node->rvalue()->type());
    } else if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      //TODO: check that the integer is a literal and that it is zero
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
    } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
    } else {
      throw std::string("wrong assignment to pointer");
    }

  } else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {

    if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
      node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else {
      throw std::string("wrong assignment to real");
    }

  } else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {

    if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    } else {
      throw std::string("wrong assignment to string");
    }

  } else if (node->lvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {

    if (node->rvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
      node->type(node->lvalue()->type());
      //DAVID TODO XXX check types
    } else {
      throw std::string("wrong assignment to function pointer");
    }

  } else {
    throw std::string("wrong types in assignment");
  }
}

//---------------------------------------------------------------------------

void udf::type_checker::do_program_node(udf::program_node *const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void udf::type_checker::do_evaluation_node(udf::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

void udf::type_checker::do_if_node(udf::if_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

void udf::type_checker::do_if_else_node(udf::if_else_node *const node, int lvl) {
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void udf::type_checker::do_block_node(udf::block_node *const node, int lvl) {
}

void udf::type_checker::do_function_node(udf::function_node *const node, int lvl) {
}

void udf::type_checker::do_function_call_node(udf::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;
  
  // Check if the function exists in the symbol table
  std::shared_ptr<udf::symbol> symbol = _symtab.find(node->identifier());
  if (!symbol) {
    throw std::string("undeclared function '" + node->identifier() + "'");
  }

  // Check arguments
  if (node->arguments()) {
    for (size_t i = 0; i < node->arguments()->size(); ++i) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));
      if (arg) arg->accept(this, lvl + 2);
    }
  }

  // Set the type of the function call node to the function's return type
  auto ftype = cdk::functional_type::cast(symbol->type());
  if (!ftype) {
    throw std::string("symbol '" + node->identifier() + "' is not a function");
  }
  node->type(ftype->output(0));
}

void udf::type_checker::do_return_node(udf::return_node *const node, int lvl) {
  if (node->retvalue() != nullptr) node->retvalue()->accept(this, lvl);
}

void udf::type_checker::do_var_declaration_node(udf::var_declaration_node *const node, int lvl) {
  if (node->initializer() != nullptr) {
      node->initializer()->accept(this, lvl + 2);
      //TODO WHY WOULD WE NEED THIS?
      //if (node->type() == nullptr)
      //  node->type(node->initializer()->type());

      if (node->is_typed(cdk::TYPE_INT)) {
        if (!node->initializer()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type for initializer (integer expected).");
      } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
        if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
          throw std::string("wrong type for initializer (integer or double expected).");
        }
      } else if (node->is_typed(cdk::TYPE_STRING)) {
        if (!node->initializer()->is_typed(cdk::TYPE_STRING)) {
          throw std::string("wrong type for initializer (string expected).");
        }
      } else if (node->is_typed(cdk::TYPE_POINTER)) {
        if (!node->initializer()->is_typed(cdk::TYPE_POINTER)) {
          auto in = (cdk::literal_node<int>*)node->initializer();
          if (in == nullptr || in->value() != 0) throw std::string("wrong type for initializer (pointer expected).");
        }
      } else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
        if (!node->initializer()->is_typed(cdk::TYPE_FUNCTIONAL)) {
          throw std::string("wrong type for initializer (function expected).");
        }
      }
      else if (node->is_typed(cdk::TYPE_UNSPEC)) {
        printf("\033[1;31m AUTO \n\033[0m");
        if (node->initializer()->is_typed(cdk::TYPE_INT)) {
          node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        } else if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
          node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        } else if (node->initializer()->is_typed(cdk::TYPE_STRING)) {
          node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
        } else if (node->initializer()->is_typed(cdk::TYPE_POINTER)) {
          node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
        } else {
          throw std::string("unknown type for initializer.");
        }
      }
      else {
        throw std::string("unknown type for initializer.");
      }
  }
}

//---------------------------------------------------------------------------

void udf::type_checker::do_nullptr_node(udf::nullptr_node *const node, int lvl) {
  ASSERT_UNSPEC;
  // nullptr is a pointer to nothing, so it is a pointer type
  // and pointers represent object addresses and occupy 4 bytes
  node->type(cdk::reference_type::create(4, nullptr));
}

void udf::type_checker::do_address_of_node(udf::address_of_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

void udf::type_checker::do_index_node(udf::index_node * const node, int lvl) {
  ASSERT_UNSPEC;
  std::shared_ptr < cdk::reference_type > btype;

  if (node->base()) {
    node->base()->accept(this, lvl + 2);
    if (!node->base()->is_typed(cdk::TYPE_POINTER)) if (!node->base()->is_typed(cdk::TYPE_POINTER)) throw std::string("pointer expression expected in index left-value");

    btype = cdk::reference_type::cast(node->base()->type());
    if (!btype) throw std::string("invalid base pointer type in index expression");
  }
  //TODO This doesnt look right at all
  // else {
  //  btype = cdk::reference_type::cast(_function->type());
  //  if (!_function->is_typed(cdk::TYPE_POINTER)) throw std::string("return pointer expression expected in index left-value");
  //}
  else {
    // we enter this block if the base is not defined, which means that we are dealing with ?
    printf("\033[1;31m index base not defined \n\033[0m");
  }

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT)) throw std::string("integer expression expected in left-value index");

  node->type(btype->referenced());
}

void udf::type_checker::do_malloc_node(udf::malloc_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in allocation expression");
  }
  //TODO pretty sure que double não causa problemas, caso contrario teriamos de verificar pelo 
  //lnode para saber o tipo o que é chato.
  auto mytype = cdk::reference_type::create(4, cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  node->type(mytype);
}

void udf::type_checker::do_size_of_node(udf::size_of_node * const node, int lvl) {
  ASSERT_UNSPEC;
  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void udf::type_checker::do_input_node(udf::input_node * const node, int lvl) {
}

//---------------------------------------------------------------------------

void udf::type_checker::do_for_node(udf::for_node * const node, int lvl) {
  if(node->inits())
    node->inits()->accept(this, lvl + 4);
  if(node->conditions())
    node->conditions()->accept(this, lvl + 4);
  if(node->steps())
    node->steps()->accept(this, lvl + 4);
  if(node->instr())
    node->instr()->accept(this, lvl+4);
}

void udf::type_checker::do_continue_node(udf::continue_node * const node, int lvl) {
}

void udf::type_checker::do_break_node(udf::break_node * const node, int lvl) {
}

//---------------------------------------------------------------------------

void udf::type_checker::do_capacity_node(udf::capacity_node * const node, int lvl) {
}

void udf::type_checker::do_rank_node(udf::rank_node * const node, int lvl) {
}

void udf::type_checker::do_dims_node(udf::dims_node * const node, int lvl) {
}

void udf::type_checker::do_dim_node(udf::dim_node * const node, int lvl) {
}

void udf::type_checker::do_tensor_indexation_node(udf::tensor_indexation_node * const node, int lvl) {
}

void udf::type_checker::do_reshape_node(udf::reshape_node * const node, int lvl){
}

void udf::type_checker::do_contraction_node(udf::contraction_node * const node, int lvl){
}

void udf::type_checker::do_tensor_node(udf::tensor_node * const node, int lvl){
}

//---------------------------------------------------------------------------

void udf::type_checker::do_write_node(udf::write_node * const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
}