/**
 * @file CalcErrorHandler.h
 * @author your name (you@domain.com)
 * @brief A simple error handler that gathers errors and can print them out
 * @version 0.1
 * @date 2022-07-23
 */
#pragma once
#include "antlr4-runtime.h"
#include <string>
#include <vector>
#include <sstream>

enum ErrType {SEMANTIC, CODEGEN};

struct CalcError {
  antlr4::Token* token;
  std::string message;
  std::string type;

  // Constructor
  CalcError(antlr4::Token* tok, std::string msg, ErrType et) {
    token = tok;
    message = msg;
    type = et == SEMANTIC ? "SEMANTIC" : "CODEGEN";;
  }

  std::string toString() {
    std::ostringstream e;
    e << type << ": [" << token->getLine() << ',' << token->getCharPositionInLine()
      << "]: " << message;
    return e.str();
  }
};

class CalcErrorHandler {
  public:
    void addSemanticError(antlr4::Token* t, std::string msg) {
      CalcError* e = new CalcError(t, msg, SEMANTIC);
      errors.push_back(e);
    }

    void addCodegenError(antlr4::Token* t, std::string msg) {
      CalcError* e = new CalcError(t, msg, CODEGEN);
      errors.push_back(e);;
    }

    std::vector<CalcError*>& getErrors() { return errors; }

    std::string errorList() {
      std::ostringstream errList;
      for (CalcError* e : errors) {
        errList << e->toString() << std::endl;
      }
      return errList.str();
    }

    bool hasErrors() { return !errors.empty(); }
  private:
    std::vector<CalcError*> errors;
};