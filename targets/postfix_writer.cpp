#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include "targets/frame_size_calculator.h"
#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated

//---------------------------------------------------------------------------

void udf::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}

void udf::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

void udf::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_context == Context::Body) {
    _pf.DOUBLE(node->value()); // load number to the stack
  } else {
    _pf.SDOUBLE(node->value());    // double is on the DATA segment
  }
}

void udf::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
   node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  _pf.EQ();
}

void udf::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  //the labels are for not evaluating the second node if the first is false
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.AND();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

void udf::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  //the labels are for not evaluating the second node if the first is false
  int lbl = ++_lbl;
  node->left()->accept(this, lvl + 2);
  _pf.DUP32();
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this, lvl + 2);
  _pf.OR();
  _pf.ALIGN();
  _pf.LABEL(mklbl(lbl));
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  if (_context == Context::Body) {
    _pf.INT(node->value()); // integer literal is on the stack: push an integer
  } else {
    _pf.SINT(node->value()); // integer literal is on the DATA segment
  }
}

void udf::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;
  /* generate the string literal */
  _pf.RODATA(); // strings are readonly DATA
  _pf.ALIGN(); // make sure the address is aligned
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
  _pf.SSTRING(node->value()); // output string characters
  if (_context == Context::Body) {
    // local variable initializer
    if (_segments.size() == 0) _pf.TEXT();
    else _pf.TEXT(_segments.top());
    _pf.ADDR(mklbl(lbl1));
  } else {
    // global variable initializer
    _pf.DATA();
    _pf.SADDR(mklbl(lbl1));
  }
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_unary_minus_node(cdk::unary_minus_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

void udf::postfix_writer::do_unary_plus_node(cdk::unary_plus_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.ADD();
}

void udf::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.SUB();
}

void udf::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MUL();
}

void udf::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.DIV();
}

void udf::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

void udf::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LT();
}

void udf::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
}

void udf::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
}

void udf::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GT();
}

void udf::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
}

void udf::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.EQ();
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // simplified generation: all variables are global
  _pf.ADDR(node->name());
}

void udf::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  _pf.LDINT(); // depends on type size
}

void udf::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl); // determine the new value
  _pf.DUP32();
  if (new_symbol() == nullptr) {
    node->lvalue()->accept(this, lvl); // where to store the value
  } else {
    _pf.DATA(); // variables are all global and live in DATA
    _pf.ALIGN(); // make sure we are aligned
    _pf.LABEL(new_symbol()->name()); // name variable location
    reset_new_symbol();
    _pf.SINT(0); // initialize it to 0 (zero)
    _pf.TEXT(); // return to the TEXT segment
    node->lvalue()->accept(this, lvl);
  }
  _pf.STINT(); // store the value at address
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_program_node(udf::program_node * const node, int lvl) {
  //TODO acho que nada disto é necessario no nosso
  //olhando para o til talvez o RTS exiga que a primeira função seja a main, pora gora vou assumir que não
  /*
  // Note that Simple doesn't have functions. Thus, it doesn't need
  // a function node. However, it must start in the main function.
  // The ProgramNode (representing the whole program) doubles as a
  // main function node.


  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  _pf.ALIGN();
  _pf.GLOBAL("_main", _pf.FUNC());
  _pf.LABEL("_main");
  _pf.ENTER(0);  

  node->declarations()->accept(this, lvl);

  // end the main function
  _pf.INT(0);
  _pf.STFVAL32();
  _pf.LEAVE();
  _pf.RET();
  */
  // these are just a few library function imports
  _pf.EXTERN("readi");
  _pf.EXTERN("printi");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");

  node->declarations()->accept(this, lvl);

}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_evaluation_node(udf::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

/*
void udf::postfix_writer::do_print_node(udf::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value to print
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.CALL("printi");
    _pf.TRASH(4); // delete the printed value
  } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.CALL("prints");
    _pf.TRASH(4); // delete the printed value's address
  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
  _pf.CALL("println"); // print a newline
}
*/

//---------------------------------------------------------------------------
/*
void udf::postfix_writer::do_read_node(udf::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.CALL("readi");
  _pf.LDFVAL32();
  node->argument()->accept(this, lvl);
  _pf.STINT();
}
*/
//---------------------------------------------------------------------------
/*
void udf::postfix_writer::do_while_node(udf::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  _pf.LABEL(mklbl(lbl1 = ++_lbl));
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl2 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl1));
  _pf.LABEL(mklbl(lbl2));
}
*/
//---------------------------------------------------------------------------

void udf::postfix_writer::do_if_node(udf::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_if_else_node(udf::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  _pf.LABEL(mklbl(lbl1));
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_block_node(udf::block_node * const node, int lvl) {
  //TODO all this scope stuff
  //if (node->is_scope()) _symtab.push(); // for block-local vars
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  //if (node->is_scope()) _symtab.pop();
}

void udf::postfix_writer::do_function_node(udf::function_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  if(node->block()==nullptr){
    if(node->qualifier()==tForward)
      _pf.EXTERN(node->identifier());
    else 
      std::cerr << "Non Forward function " << node->identifier() << " declaration must have body\n";
    return;
  }

  std::string funcName = (node->identifier() == "udf") ? "_main" : "_FUNC" + node->identifier();
  _segments.push(funcName);
   auto function = udf::make_symbol(false, node->qualifier(), node->type(), funcName, false, false);
  _function = function;
  _offset = 8; //4 fp + 4 return address

  if (node->args()) {
    _context = Context::Args; 
    for (size_t ix = 0; ix < node->args()->size(); ix++) {
      cdk::basic_node *arg = node->args()->node(ix);
      if (arg == nullptr) break; // this means an empty sequence of arguments
      arg->accept(this, 0); // the function symbol is at the top of the stack
    }
    _context = Context::Global;
  }

  _pf.TEXT(_segments.top());
  _pf.ALIGN();
  if (node->qualifier() == tPublic) _pf.GLOBAL(funcName,  _pf.FUNC()); //globais são as public, as private basta o label
  _pf.LABEL(funcName);
  
  // compute stack size to be reserved for local variables
  frame_size_calculator lsc(_compiler, _symtab, function);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize()); // total stack size reserved for local variables

  _context = Context::Body;
  os() << "        ;; before body " << std::endl;
  node->block()->accept(this, lvl);
  os() << "        ;; after body " << std::endl;

  _pf.LEAVE(); //TODO Isto não devia estar no return? faz diferença estar aqui para as não tenham return?
  _pf.RET();

  _context=Context::Global; //we always exit a function 
  //_symtab.pop(); //TODO later scope of arguments l
  _function = nullptr; //exited the function declaration
  _segments.pop();

  _pf.DATA();   //FIXME could be RODATA what
  _pf.SADDR(funcName);
  
}

void udf::postfix_writer::do_function_call_node(udf::function_call_node * const node, int lvl) {
}

void udf::postfix_writer::do_return_node(udf::return_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  auto function = _function ? _function : nullptr;
  if (function == nullptr)
    std::cerr << node->lineno() << ": should not happen: invalid return" << std::endl;
  
  auto return_type = cdk::functional_type::cast(function->type())->output(0); 

  // should not reach here without returning a value (if not void)
  if (return_type->name() != cdk::TYPE_VOID) {
    if (node->retvalue()) node->retvalue()->accept(this, lvl + 2);

    //std::cerr << "RETVAL2 " << _function->name() << ":" << _function->type()->size() << std::endl;

    if (return_type->name() == cdk::TYPE_INT || return_type->name() == cdk::TYPE_STRING ||
        return_type->name() == cdk::TYPE_POINTER || return_type->name() == cdk::TYPE_FUNCTIONAL) {
      _pf.STFVAL32();
    } else if (return_type->name() == cdk::TYPE_DOUBLE) {
      if (node->retvalue()->is_typed(cdk::TYPE_INT))
	      _pf.I2D();
      _pf.STFVAL64();
    } else {
      std::cerr << node->lineno() << ": should not happen: unknown return type" << std::endl;
    }
  }
  _pf.LEAVE();
  _pf.RET();
}

void udf::postfix_writer::do_var_declaration_node(udf::var_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  auto id = node->identifier();
  auto function = _function; //if function == nullptr -> var global
  std::cout << "INITIAL OFFSET: " << _offset << std::endl;
  int offset = 0, typesize = node->type()->size(); // in bytes

  if(_context == Context::Global){
    offset = 0;
  }
  else if(_context == Context::Args){
    offset = _offset;
    _offset += typesize;
  }
  else if(_context == Context::Body){
    _offset -= typesize;
    offset = _offset;
  }
  else{
    std::cout << "Invalid Context\n";
  }
  std::cout << "OFFSET: " << id << ", " << offset << std::endl;

  auto symbol = new_symbol();
  if(symbol){
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_context == Context::Args)
    return; //offsets are set, no further action needed

  if (_context == Context::Body) {
    if (node->initializer()==nullptr)
      return;
    
    node->initializer()->accept(this, lvl);
    if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_STRING) || node->is_typed(cdk::TYPE_POINTER)) {
      _pf.LOCAL(symbol->offset());
      _pf.STINT();
    } else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (node->initializer()->is_typed(cdk::TYPE_INT))
        _pf.I2D();
      _pf.LOCAL(symbol->offset());
      _pf.STDOUBLE();
    } else {
      std::cerr << "cannot initialize" << std::endl;
    }
    return;
  }

  if (_context != Context::Global)
    std::cout << "invalid context" << std::endl; 

  if (node->initializer() == nullptr) {
    std::cout << "AAAAAAAAAAAAAAAAAAAAAA\n\n\n\n\n";
    _pf.BSS();
    _pf.ALIGN();
    _pf.LABEL(id);
    _pf.SALLOC(typesize);
    return;
  }
  if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE) || node->is_typed(cdk::TYPE_POINTER)) {
    _pf.DATA();
    _pf.ALIGN();
    _pf.LABEL(id);

    if (!node->is_typed(cdk::TYPE_DOUBLE)){
      node->initializer()->accept(this, lvl);
      return;
    }


    if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
      node->initializer()->accept(this, lvl);
    }
    else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
      cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
      cdk::double_node ddi(dclini->lineno(), dclini->value());
      ddi.accept(this, lvl);
    }
    else {
      std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
      _errors = true;
    }

    return;
  }

  if (node->is_typed(cdk::TYPE_STRING)) {
      if (dynamic_cast<cdk::string_node *>(node)) {
        int litlbl;
        // HACK!!! string literal initializers must be emitted before the string identifier
        _pf.RODATA();
        _pf.ALIGN();
        _pf.LABEL(mklbl(litlbl = ++_lbl));
        _pf.SSTRING(dynamic_cast<cdk::string_node*>(node->initializer())->value());
        _pf.ALIGN();
        _pf.LABEL(id);
        _pf.SADDR(mklbl(litlbl));
        return;
      }
      else {
        _pf.DATA();
        _pf.ALIGN();
        _pf.LABEL(id);
        node->initializer()->accept(this, lvl);
        return;
      }
      std::cerr << node->lineno() << ": '" << id << "' has unexpected initializer\n";
      _errors = true;
      return;
  }
}

void udf::postfix_writer::do_nullptr_node(udf::nullptr_node * const node, int lvl) {
}

void udf::postfix_writer::do_address_of_node(udf::address_of_node * const node, int lvl) {
}

void udf::postfix_writer::do_index_node(udf::index_node * const node, int lvl) {
}

void udf::postfix_writer::do_input_node(udf::input_node * const node, int lvl) {
}

void udf::postfix_writer::do_malloc_node(udf::malloc_node * const node, int lvl) {
}

void udf::postfix_writer::do_size_of_node(udf::size_of_node * const node, int lvl) {
}

void udf::postfix_writer::do_for_node(udf::for_node * const node, int lvl) {
}

void udf::postfix_writer::do_continue_node(udf::continue_node * const node, int lvl) {
}

void udf::postfix_writer::do_break_node(udf::break_node * const node, int lvl) {
}

void udf::postfix_writer::do_capacity_node(udf::capacity_node * const node, int lvl) {
}

void udf::postfix_writer::do_rank_node(udf::rank_node * const node, int lvl) {
}

void udf::postfix_writer::do_dim_node(udf::dim_node * const node, int lvl) {
}

void udf::postfix_writer::do_dims_node(udf::dims_node * const node, int lvl) {
}

void udf::postfix_writer::do_tensor_indexation_node(udf::tensor_indexation_node * const node, int lvl) {
}

void udf::postfix_writer::do_reshape_node(udf::reshape_node * const node, int lvl){
}

void udf::postfix_writer::do_contraction_node(udf::contraction_node * const node, int lvl){
}

void udf::postfix_writer::do_tensor_node(udf::tensor_node * const node, int lvl){
}

void udf::postfix_writer::do_write_node(udf::write_node * const node, int lvl) {
    ASSERT_SAFE_EXPRESSIONS;
    for (size_t ix = 0; ix < node->arguments()->size(); ix++) {
      auto child = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ix));
      std::shared_ptr<cdk::basic_type> etype = child->type();

      child->accept(this, lvl); // expression to print
      if (etype->name() == cdk::TYPE_INT) {
        _pf.CALL("printi");
        _pf.TRASH(4); // trash int
      } else if (etype->name() == cdk::TYPE_DOUBLE) {
        _pf.CALL("printd");
        _pf.TRASH(8); // trash double
      } else if (etype->name() == cdk::TYPE_STRING) {
        _pf.CALL("prints");
        _pf.TRASH(4); // trash char pointer
      } else {
        std::cerr << "cannot print expression of unknown type" << std::endl;
        return;
      }
      if(node->newline())
        _pf.CALL("println");
    }
}