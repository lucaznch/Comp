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

//---------------------------------------------------------------------------

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
  //the labels are for not evaluating the second node if the first is true
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

void udf::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_context == Context::Body) {
    _pf.DOUBLE(node->value()); // load number to the stack
  } else {
    _pf.SDOUBLE(node->value());    // double is on the DATA segment
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
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->type())->referenced()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->type())->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) _pf.DADD();
  else _pf.ADD();
  
}

void udf::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->left()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->type())->referenced()->size());
    _pf.MUL();
  }

  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.I2D();
  } else if (node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    _pf.INT(cdk::reference_type::cast(node->type())->referenced()->size());
    _pf.MUL();
  }

  if (node->is_typed(cdk::TYPE_DOUBLE)) _pf.DSUB();
  else _pf.SUB();

  if(node->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT) ) {
    _pf.INT(cdk::reference_type::cast(node->type())->referenced()->size());
    _pf.DIV();
  }
  
}

void udf::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) 
    _pf.I2D();
  if (node->type()->name() == cdk::TYPE_DOUBLE) 
    _pf.DMUL();        
  else 
    _pf.MUL();    
}

void udf::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE) && node->left()->is_typed(cdk::TYPE_INT)) 
    _pf.I2D();
  if (node->type()->name() == cdk::TYPE_DOUBLE) 
    _pf.DDIV();        
  else 
    _pf.DIV(); 
}

void udf::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->right()->is_typed(cdk::TYPE_INT) && node->left()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();

  if(node->left()->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }

  _pf.LT();
}

void udf::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->right()->is_typed(cdk::TYPE_INT) && node->left()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();

  if(node->left()->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }
  
  _pf.LE();
}

void udf::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->right()->is_typed(cdk::TYPE_INT) && node->left()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();

  if(node->left()->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }
  
  _pf.GE();
}

void udf::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->right()->is_typed(cdk::TYPE_INT) && node->left()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();

  if(node->left()->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }
  
  _pf.GT();
}

void udf::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->right()->is_typed(cdk::TYPE_INT) && node->left()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();

  if(node->left()->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }
  
  _pf.NE();
}

void udf::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();
  node->right()->accept(this, lvl);
  if (node->right()->is_typed(cdk::TYPE_INT) && node->left()->is_typed(cdk::TYPE_DOUBLE)) 
    _pf.I2D();

  if(node->left()->is_typed(cdk::TYPE_DOUBLE)){
    _pf.DCMP();
    _pf.INT(0);
  }
  
  _pf.EQ();
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  const std::string &id = node->name();
  auto symbol = _symtab.find(id);
  if (symbol->global()) {
    _pf.ADDR(symbol->name());
  } else {
    _pf.LOCAL(symbol->offset());
    std::cerr << "LVAL " << symbol->name() << ":" << symbol->type()->size() << ":" << symbol->offset() << std::endl;
  }
  
  // simplified generation: all variables are global
  //_pf.ADDR(node->name());
}

void udf::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  if (node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.LDDOUBLE();
  } else {
    _pf.LDINT(); 
  }
}

void udf::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->rvalue()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    if (node->rvalue()->type()->name() == cdk::TYPE_INT) _pf.I2D();
    _pf.DUP64();
  } else {
    _pf.DUP32();
  }

  node->lvalue()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    _pf.STDOUBLE();
  } else {
    _pf.STINT();
  }
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_program_node(udf::program_node * const node, int lvl) {
  // TODO: is this necessary? maybe the RTS mandates that the first function is main
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
  _pf.EXTERN("printd");
  _pf.EXTERN("prints");
  _pf.EXTERN("println");
  _pf.EXTERN("mem_init");
  //_symtab.push();
  node->declarations()->accept(this, lvl);
  //_symtab.pop();
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_evaluation_node(udf::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.TRASH(4); // delete the evaluated value
  } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4); // delete the evaluated value's address
  } else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.TRASH(8); // delete the evaluated value's address //TODO CUIDADO COM O MALLOC
  } else if (node->argument()->is_typed(cdk::TYPE_POINTER)) {
    _pf.TRASH(4); // delete the evaluated value's address
  }
  /*
  else if (node->argument()->is_typed(cdk::TYPE_FUNCTIONAL)) {
    // do nothing, the function pointer is already on the stack
  }
  */
  else if (node->argument()->is_typed(cdk::TYPE_VOID)) {
    // do nothing, the void value is already on the stack
  }
  else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void udf::postfix_writer::do_if_node(udf::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
}

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
  _symtab.push(); // for block-local vars
  if (node->declarations()) node->declarations()->accept(this, lvl + 2);
  if (node->instructions()) node->instructions()->accept(this, lvl + 2);
  _symtab.pop();
}

void udf::postfix_writer::do_function_node(udf::function_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::string funcName = (node->identifier() == "udf") ? "_main" : "_FUNC" + node->identifier();
  _segments.push(funcName);
  auto function = udf::make_symbol(false, node->qualifier(), node->type(), funcName, false, true);
  _function = function;
  _offset = 8; //4 fp + 4 return address
  if(!_symtab.insert(node->identifier(), function))
    std::cout << "Push of " << funcName << " failed\n";
  //HERE confirmed in gdb insertion suceeded as it should

  _symtab.push(); //scope of args
  if (node->args()) {
      auto arg_types = function->getArgTypes();
    _context = Context::Args; 
    for (size_t ix = 0; ix < node->args()->size(); ix++) {
      cdk::typed_node *arg = dynamic_cast<cdk::typed_node*>(node->args()->node(ix));
      if (arg == nullptr) break; // this means an empty sequence of arguments
      arg->accept(this, 0); // the function symbol is at the top of the stack
      arg_types->push_back(arg->type());
    }
    _context = Context::Global;
  }
  if(node->block()==nullptr){
    if(node->qualifier()==tForward){
      _pf.EXTERN(node->identifier());
    }
    else 
      std::cerr << "Non Forward function " << node->identifier() << " declaration must have body\n";
    return;
  }
  _pf.TEXT(_segments.top());
  _pf.ALIGN();
  if (node->qualifier() == tPublic) _pf.GLOBAL(funcName,  _pf.FUNC()); //globais são as public, as private basta o label
  _pf.LABEL(funcName);
  
  // compute stack size to be reserved for local variables
  frame_size_calculator lsc(_compiler, _symtab, function);
  node->accept(&lsc, lvl);
  _pf.ENTER(lsc.localsize()); // total stack size reserved for local variables
  std::cout << lsc.localsize() << '\n';
  if (node->identifier() == "udf") _pf.CALL("mem_init");
  _offset = 0 ;//The locals are offset from 0
  _context = Context::Body;
  os() << "        ;; before body " << std::endl;
  node->block()->accept(this, lvl);
  os() << "        ;; after body " << std::endl;

  _pf.LEAVE(); //TODO Isto não devia estar no return? faz diferença estar aqui para as não tenham return?
  _pf.RET();

  _context=Context::Global; //we always exit a function 
  _symtab.pop(); 
  _function = nullptr; //exited the function declaration
  _segments.pop();
  //HERE porque?
  //_pf.DATA();   //FIXME could be RODATA what
  //_pf.SADDR(funcName);
  
}

void udf::postfix_writer::do_function_call_node(udf::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  std::shared_ptr<udf::symbol> symbol = _symtab.find(node->identifier());
  int args_sz = 0;
  // Evaluate and push arguments (right-to-left)
  if (node->arguments()) {
    auto arg_types= symbol->getArgTypes();
    if(node->arguments()->size() != arg_types->size() )
      throw std::string("Declared number of args doesnt match number in function call\n");
    for (int i = node->arguments()->size() - 1; i >= 0; --i) {
      cdk::expression_node *arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(i));
      arg->accept(this, lvl + 2);
      args_sz+=arg->type()->size();
      if(arg->type() != arg_types->at(i)){
        if(arg->type()->name() == cdk::TYPE_INT && arg_types->at(i)->name()== cdk::TYPE_DOUBLE)
          _pf.I2D();
        else
          throw std::string("Function call arg types dont match declaration\n");
      }
    }
  }


  std::string funcName;
  
  if (symbol->qualifier() == udf::tForward && node->identifier() == "argc") {
    funcName = node->identifier();
  }
  else {
    funcName = (node->identifier() == "udf") ? "_main" : "_FUNC" + node->identifier();
  }
  _pf.CALL(funcName);

  _pf.TRASH(args_sz);
  if(node->type()->name()==cdk::TYPE_VOID)
    return;

  if(node->type()->name()==cdk::TYPE_DOUBLE)
    _pf.LDFVAL64();
  else
    _pf.LDFVAL32();

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

  auto symbol = udf::make_symbol(false, node->qualifier(), node->type(), id, (bool)node->initializer(), false);
  if (_symtab.insert(id, symbol)) {
  } else {
    auto s = _symtab.find(id);  // retry: forward declarations
    if (s->qualifier() == Qualifier::tForward) {
      _symtab.replace(id, symbol);
    }
    
    else {
     throw std::string("variable '" + id + "' redeclared");
    }
  }
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
    _pf.BSS();
    _pf.ALIGN();
    _pf.LABEL(id);
    _pf.SALLOC(typesize);
    return;
  }


  if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_DOUBLE) || node->is_typed(cdk::TYPE_POINTER)) {
    _pf.DATA();
    _pf.ALIGN();
    if(node->qualifier() == udf::tPublic) _pf.GLOBAL(id,_pf.OBJ()); //só se for public
    _pf.LABEL(id);

    if(node->initializer()->label()!="integer_node" && node->initializer()->label()!="double_node")
      throw std::string("Initializer of global variable must be a literal (no math allowed!)");

    if(node->type()->name()==cdk::TYPE_DOUBLE){
      if(node->initializer()->type()->name()==cdk::TYPE_INT){
        //cast expression_node to integer_node so we have access to value()
        cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
        //new instance of double_node taking the integer value
        cdk::double_node ddi(dclini->lineno(), dclini->value());
        //have that one accept the visitor instead
        ddi.accept(this, lvl);
        return;
      }
    }

    node->initializer()->accept(this, lvl);

  }

  if (node->is_typed(cdk::TYPE_STRING)) {
      if (dynamic_cast<cdk::string_node *>(node->initializer())) {
        printf("\033[1;31mRODATA\n\033[0m");
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
        printf("\033[1;31mDATA\n\033[0m");
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

//---------------------------------------------------------------------------

void udf::postfix_writer::do_nullptr_node(udf::nullptr_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  // a pointer is a 32-bit integer
  if (_context == Context::Body) {
    _pf.INT(0);
  } else {
    _pf.SINT(0);
  }
}

void udf::postfix_writer::do_address_of_node(udf::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  // since the argument is an lvalue, it is already an address
  node->lvalue()->accept(this, lvl + 2);
}

void udf::postfix_writer::do_index_node(udf::index_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);
  _pf.INT(node->type()->size());
  std::cout <<  node->type()->size() << std::endl ; 
  _pf.MUL();
  _pf.ADD();
}

void udf::postfix_writer::do_malloc_node(udf::malloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); //argument to the stack
  _pf.INT( cdk::reference_type::cast(node->type())->referenced()->size());
  _pf.MUL();
  _pf.ALLOC(); // allocate
  _pf.SP(); // put base pointer in stack
}

void udf::postfix_writer::do_size_of_node(udf::size_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->expression()->type()->size());
}

void udf::postfix_writer::do_input_node(udf::input_node * const node, int lvl) {
}

//---------------------------------------------------------------------------

void udf::postfix_writer::do_for_node(udf::for_node * const node, int lvl) {
  //Nope, since var_declaration_node creates symbols in the postfix_writer we cant have 
  //the type checker doing the anything after inits until those ave been accpeted here and so forth
  //ASSERT_SAFE_EXPRESSIONS; 
  int forCond = ++_lbl;
  int forStep = ++_lbl;
  int forEnd = ++_lbl;

  _symtab.push();
  _forCond.push_back(forCond); // after init, before body
  _forStep.push_back(forStep); // after intruction
  _forEnd.push_back(forEnd); // after for

  if(node->inits())
    node->inits()->accept(this, lvl + 4);
  _pf.LABEL(mklbl(forCond = ++_lbl));
  if(node->conditions())
    node->conditions()->accept(this, lvl + 4);
  _pf.JZ(mklbl(forEnd = ++_lbl));
  if(node->instr())
    node->instr()->accept(this, lvl+4);
  if(node->steps())
    node->steps()->accept(this, lvl + 4);
  _pf.JMP(mklbl(forCond));
  _pf.LABEL(mklbl(forEnd));
  
  _forCond.pop_back();
  _forStep.pop_back();
  _forEnd.pop_back();
  _symtab.pop();

}

void udf::postfix_writer::do_continue_node(udf::continue_node * const node, int lvl) {
}

void udf::postfix_writer::do_break_node(udf::break_node * const node, int lvl) {
}

//---------------------------------------------------------------------------

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

//---------------------------------------------------------------------------


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