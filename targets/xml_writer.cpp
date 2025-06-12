#include <string>
#include "targets/xml_writer.h"
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated

//---------------------------------------------------------------------------

void udf::xml_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void udf::xml_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}

//---------------------------------------------------------------------------

void udf::xml_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  os() << std::string(lvl, ' ') << "<sequence_node size='" << node->size() << "'>" << std::endl;
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void udf::xml_writer::do_integer_node(cdk::integer_node * const node, int lvl) {
  process_literal(node, lvl);
}

void udf::xml_writer::do_string_node(cdk::string_node * const node, int lvl) {
  process_literal(node, lvl);
}

void udf::xml_writer::do_double_node(cdk::double_node * const node, int lvl) {
  process_literal(node, lvl);
}
//---------------------------------------------------------------------------

void udf::xml_writer::do_unary_operation(cdk::unary_operation_node * const node, int lvl) {
  //TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void udf::xml_writer::do_unary_minus_node(cdk::unary_minus_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void udf::xml_writer::do_unary_plus_node(cdk::unary_plus_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void udf::xml_writer::do_not_node(cdk::not_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void udf::xml_writer::do_malloc_node(udf::malloc_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void udf::xml_writer::do_address_of_node(udf::address_of_node * const node, int lvl) {
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void udf::xml_writer::do_size_of_node(udf::size_of_node * const node, int lvl) {
 do_unary_operation(node, lvl);
}
//---------------------------------------------------------------------------

void udf::xml_writer::do_binary_operation(cdk::binary_operation_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void udf::xml_writer::do_add_node(cdk::add_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_div_node(cdk::div_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_le_node(cdk::le_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}


void udf::xml_writer::do_and_node(cdk::and_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
void udf::xml_writer::do_or_node(cdk::or_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}
//---------------------------------------------------------------------------

void udf::xml_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  os() << std::string(lvl, ' ') << "<" << node->label() << ">" << node->name() << "</" << node->label() << ">" << std::endl;
}

void udf::xml_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->lvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

void udf::xml_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  node->lvalue()->accept(this, lvl);
  reset_new_symbol();

  node->rvalue()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void udf::xml_writer::do_program_node(udf::program_node * const node, int lvl) {
  openTag(node, lvl);
  node->declarations()->accept(this, lvl + 4);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void udf::xml_writer::do_evaluation_node(udf::evaluation_node * const node, int lvl) {
  //ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  node->argument()->accept(this, lvl + 2);
  closeTag(node, lvl);
}

void udf::xml_writer::do_if_node(udf::if_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->block()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  closeTag(node, lvl);
}

void udf::xml_writer::do_if_else_node(udf::if_else_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);
  openTag("condition", lvl + 2);
  node->condition()->accept(this, lvl + 4);
  closeTag("condition", lvl + 2);
  openTag("then", lvl + 2);
  node->thenblock()->accept(this, lvl + 4);
  closeTag("then", lvl + 2);
  openTag("else", lvl + 2);
  node->elseblock()->accept(this, lvl + 4);
  closeTag("else", lvl + 2);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------
void udf::xml_writer::do_write_node(udf::write_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  // newline
  openTag("newline", lvl + 2);
  os() << (node->newline() ? "true" : "false");
  closeTag("newline", lvl + 2);

  // arguments
  if (node->arguments()) {
    openTag("arguments", lvl + 2);
    node->arguments()->accept(this, lvl + 4);
    closeTag("arguments", lvl + 2);
  }

  closeTag(node, lvl);
}

void udf::xml_writer::do_block_node(udf::block_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  if (node->declarations()) {
    openTag("declarations", lvl + 2);
    node->declarations()->accept(this, lvl + 4);
    closeTag("declarations", lvl + 2);
  }

  if (node->instructions()) {
    openTag("instructions", lvl + 2);
    node->instructions()->accept(this, lvl + 4);
    closeTag("instructions", lvl + 2);
  }

  closeTag(node, lvl);
}

void udf::xml_writer::do_function_node(udf::function_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  // qualifier as string
  std::string qualifier;
  switch (node->qualifier()) {
    case 1: qualifier = "public"; break;
    case 2: qualifier = "forward"; break;
    default: qualifier = "private"; break;
  }
  openTag("qualifier", lvl + 2);
  os() << qualifier;
  closeTag("qualifier", lvl + 2);

  // return_type
  openTag("return_type", lvl + 2);
  os() << cdk::to_string(node->return_type());
  closeTag("return_type", lvl + 2);

  // auto
  openTag("auto", lvl + 2);
  os() << (node->is_auto() ? "true" : "false");
  closeTag("auto", lvl + 2);
  
  // identifier
  openTag("identifier", lvl + 2);
  os() << node->identifier();
  closeTag("identifier", lvl + 2);

  // arguments
  if (node->args()) {
    openTag("arguments", lvl + 2);
    node->args()->accept(this, lvl + 4);
    closeTag("arguments", lvl + 2);
  }

  // block
  if (node->block()) {
    openTag("body", lvl + 2);
    node->block()->accept(this, lvl + 4);
    closeTag("body", lvl + 2);
  }

  closeTag(node, lvl);
}

void udf::xml_writer::do_function_call_node(udf::function_call_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  // identifier
  openTag("identifier", lvl + 2);
  os() << node->identifier();
  closeTag("identifier", lvl + 2);

  // arguments
  if (node->arguments()) {
    openTag("arguments", lvl + 2);
    node->arguments()->accept(this, lvl + 4);
    closeTag("arguments", lvl + 2);
  }

  closeTag(node, lvl);
}

void udf::xml_writer::do_return_node(udf::return_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  if (node->retvalue()) {
    openTag("value", lvl + 2);
    node->retvalue()->accept(this, lvl + 4);
    closeTag("value", lvl + 2);
  }

  closeTag(node, lvl);
}

void udf::xml_writer::do_var_declaration_node(udf::var_declaration_node * const node, int lvl) {
  // TODO: ASSERT_SAFE_EXPRESSIONS;
  openTag(node, lvl);

  // qualifier
  openTag("qualifier", lvl + 2);
  switch (node->qualifier()) {
    case 0: os() << "private"; break;
    case 1: os() << "public"; break;
    case 2: os() << "forward"; break;
    default: os() << "unknown"; break;
  }
  closeTag("qualifier", lvl + 2);
  
  // auto
  openTag("is_auto", lvl + 2);
  os() << (node->is_auto() ? "true" : "false");
  closeTag("is_auto", lvl + 2);

  // type
  openTag("type", lvl + 2);
  if (node->type())
    os() << node->type()->to_string();
  else
    os() << "undefined";
  closeTag("type", lvl + 2);
  
  // identifier
  openTag("identifier", lvl + 2);
  os() << node->identifier();
  closeTag("identifier", lvl + 2);

  // initializer
  if (node->initializer()) {
    openTag("initializer", lvl + 2);
    node->initializer()->accept(this, lvl + 4);
    closeTag("initializer", lvl + 2);
  }

  closeTag(node, lvl);
}

void udf::xml_writer::do_nullptr_node(udf::nullptr_node * const node, int lvl) {
    os() << std::string(lvl, ' ') << "<" << node->label() << ">" <<  "</" << node->label() << ">" << std::endl;

}

void udf::xml_writer::do_index_node(udf::index_node * const node, int lvl) {
  openTag("index", lvl);
  node->base()->accept(this, lvl + 2);
  node->index()->accept(this, lvl + 2);
  closeTag("index", lvl);
}

void udf::xml_writer::do_for_node(udf::for_node * const node, int lvl) {
  openTag(node, lvl);

  if (node->inits()) {
    openTag("init", lvl + 2);
    node->inits()->accept(this, lvl + 4);
    closeTag("init", lvl + 2);
  }

  if (node->conditions()) {
    openTag("condition", lvl + 2);
    node->conditions()->accept(this, lvl + 4);
    closeTag("condition", lvl + 2);
  }

  if (node->steps()) {
    openTag("step", lvl + 2);
    node->steps()->accept(this, lvl + 4);
    closeTag("step", lvl + 2);
  }

  if (node->instr()) {
    openTag("instruction", lvl + 2);
    node->instr()->accept(this, lvl + 4);
    closeTag("instruction", lvl + 2);
  }

  closeTag(node, lvl); 
}

void udf::xml_writer::do_continue_node(udf::continue_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

void udf::xml_writer::do_break_node(udf::break_node * const node, int lvl) {
  openTag(node, lvl);
  closeTag(node, lvl);
}

//---------------------------------------------------------------------------

void udf::xml_writer::do_capacity_node(udf::capacity_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void udf::xml_writer::do_rank_node(udf::rank_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}


void udf::xml_writer::do_dim_node(udf::dim_node * const node, int lvl) {
  do_binary_operation(node, lvl);
}

void udf::xml_writer::do_dims_node(udf::dims_node * const node, int lvl) {
  do_unary_operation(node, lvl);
}

void udf::xml_writer::do_tensor_indexation_node(udf::tensor_indexation_node * const node, int lvl) {
  openTag(node, lvl);
  
  openTag("tensor", lvl + 2);
  node->tensor()->accept(this, lvl + 4);
  closeTag("tensor", lvl + 2);

  openTag("indexes", lvl + 2);
  node->indexes()->accept(this, lvl + 4);
  closeTag("indexes", lvl + 2);

  closeTag(node, lvl);
}

void udf::xml_writer::do_reshape_node(udf::reshape_node * const node, int lvl){
  openTag(node, lvl);
  
  openTag("tensor", lvl + 2);
  node->tensor()->accept(this, lvl + 4);
  closeTag("tensor", lvl + 2);

  openTag("shape", lvl + 2);
  node->sizes()->accept(this, lvl + 4);
  closeTag("shape", lvl + 2);

  closeTag(node, lvl);
}

void udf::xml_writer::do_contraction_node(udf::contraction_node * const node, int lvl){
   do_binary_operation(node, lvl);
}

void udf::xml_writer::do_tensor_node(udf::tensor_node * const node, int lvl){
  openTag(node, lvl);

  // nested_sequences
  if (node->nested_sequences()) {
    openTag("nested_sequences", lvl + 2);
    node->nested_sequences()->accept(this, lvl + 4);
    closeTag("nested_sequences", lvl + 2);
  }

  closeTag(node, lvl);
}

void udf::xml_writer::do_input_node(udf::input_node * const node, int lvl) {
  openTag(node, lvl);

  closeTag(node, lvl);
}