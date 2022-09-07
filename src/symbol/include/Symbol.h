/**
 * @file Symbol.h
 * @author gpollice
 * @brief Definition of a symbol for the Calculator app
 * @version 0.1
 * @date 2022-07-16
 * 
 * You can use this as a template by changing the data items here. The Symbols
 * are stored in the Scope and then associated with parse tree/AST nodes.
 */
#pragma once
#include<string>
#include<sstream>
#include "llvm/IR/Value.h"

enum SymType {INT, BOOL, UNDEFINED};

class Symbol
{
  public:
    /* data */
    std::string identifier;
    SymType type;
    bool defined;
    llvm::Value *val;

    // The only constructor
    Symbol(std::string id,SymType t) {
      identifier = id;
      type = t;
      defined = false;
      val = nullptr;
    }

    // Copy assignment: same as default
    // Symbol& operator=(const Symbol&) { return *this; }

    static std::string getSymTypeName(SymType st) {
      std::string s;
      switch (st) {
        case INT: 
          s = "INT";
          break;
        case BOOL:
          s = "BOOL";
          break;
        default:
          s = "UNDEFINED";
      }
      return s;
    }

    /**
     * @brief Descriptive string for this symbol.
     * 
     * @return std::string 
     */
    std::string toString() const {
      std::ostringstream description;
      description << '[' << identifier << ", " << getSymTypeName(type) << ']';
      return description.str(); 
    }
};

