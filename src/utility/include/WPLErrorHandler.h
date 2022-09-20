/**
 * @file WPLErrorHandler.h
 * @author Alex Friedman (ahfriedman.com)
 * @brief Basic error handler for reporting during compile time. Based on example.
 * @version 0.1
 * @date 2022-09-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "antlr4-runtime.h"
#include <string>
#include <vector>
#include <sstream>

enum ErrType
{
  SYNTAX,
  SEMANTIC,
  CODEGEN
};

struct WPLError
{
  ErrType type;
  antlr4::Token *token;
  std::string message;

  WPLError(antlr4::Token *tok, std::string msg, ErrType et)
  {
    token = tok;
    message = msg;

    type = et;
  }

  std::string toString()
  {
    std::ostringstream e;
    e << getStringForErrorType(type) << ": [" << token->getLine() << ',' << token->getCharPositionInLine()
      << "]: " << message;
    return e.str();
  }

  static std::string getStringForErrorType(ErrType e)
  {
    switch (e)
    {
    case SYNTAX:
      return "SYNTAX";
    case SEMANTIC:
      return "SEMANTIC";
    case CODEGEN:
      return "CODEGEN";
    }
  }
};

class WPLErrorHandler
{
public:
  void addSemanticError(antlr4::Token *t, std::string msg)
  {
    WPLError *e = new WPLError(t, msg, SEMANTIC);
    errors.push_back(e);
  }

  void addCodegenError(antlr4::Token *t, std::string msg)
  {
    WPLError *e = new WPLError(t, msg, CODEGEN);
    errors.push_back(e);
  }

  std::vector<WPLError *> &getErrors() { return errors; }

  std::string errorList()
  {
    std::ostringstream errList;
    for (WPLError *e : errors)
    {
      errList << e->toString() << std::endl;
    }
    return errList.str();
  }

  bool hasErrors() { return !errors.empty(); }

protected:
  std::vector<WPLError *> errors;
};

class WPLSyntaxErrorListener : public antlr4::BaseErrorListener, public WPLErrorHandler
{
  virtual void syntaxError(
      antlr4::Recognizer *recognizer,
      antlr4::Token *offendingSymbol,
      size_t line,
      size_t charPositionInLine,
      const std::string &msg,
      std::exception_ptr ex) override
  {
    WPLError *e = new WPLError(offendingSymbol, msg, SYNTAX);
    errors.push_back(e);
    // throw std::invalid_argument("test error thrown: " + msg);
  }
};