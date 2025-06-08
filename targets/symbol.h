#pragma once

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace udf {

  class symbol {

    std::string _name; // identifier
    int _value;
    bool _constant; // is it a constant?
    int _qualifier; // qualifiers: public, forward, "private" (i.e., none)
    std::shared_ptr<cdk::basic_type> _type; // type (type id + type size)
    bool _initialized; // initialized?
    int _offset = 0; // 0 (zero) means global variable/function
    bool _function; // false for variables
    bool _forward = false;

  public:
    symbol(bool constant, int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name, bool initialized,
           bool function, bool forward = false) :
        _name(name), _value(0), _constant(constant), _qualifier(qualifier), _type(type), _initialized(initialized), _function(
            function), _forward(forward) {
    }

    ~symbol() {
    }

    const std::string& name() const {
      return _name;
    }
    int value() const {
      return _value;
    }
    int value(int v) {
      return _value = v;
    }

    bool constant() const {
      return _constant;
    }
    int qualifier() const {
      return _qualifier;
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    void set_type(std::shared_ptr<cdk::basic_type> t) {
      _type = t;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }

    const std::string& identifier() const {
      return name();
    }
    bool initialized() const {
      return _initialized;
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }
    bool isFunction() const {
      return _function;
    }

    bool global() const {
      return _offset == 0;
    }
    bool isVariable() const {
      return !_function;
    }

    bool forward() const {
      return _forward;
    }

  };

  inline auto make_symbol(bool constant, int qualifier, std::shared_ptr<cdk::basic_type> type, const std::string &name,
                          bool initialized, bool function, bool forward = false) {
    return std::make_shared<symbol>(constant, qualifier, type, name, initialized, function, forward);
  }

} // udf

