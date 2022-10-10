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

enum ErrSev 
{
  ERROR = 1, 
  CRITICAL_WARNING = 2, //Not an error persay, but need to stop compiling. 
  WARNING = 4,
  INFO = 8,
};

struct WPLError
{
  ErrType type;
  antlr4::Token *token;
  std::string message;

  ErrSev severity; 

  WPLError(antlr4::Token *tok, std::string msg, ErrType et, ErrSev es)
  {
    token = tok;
    message = msg;

    type = et;
    severity = es; 
  }

  std::string toString()
  {
    std::ostringstream e;
    e << getStringForSeverity(severity) << ": " << getStringForErrorType(type) << ": [" << token->getLine() << ',' << token->getCharPositionInLine()
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

  static std::string getStringForSeverity(ErrSev e)
  {
    switch(e)
    {
      case ERROR: 
        return "Error";
      case CRITICAL_WARNING: 
        return "Critical Warning";
      case WARNING:
        return "Warning";
      case INFO: 
        return "Informational";
    }
  }
};

class WPLErrorHandler
{
public:
  void addSemanticError(antlr4::Token *t, std::string msg)
  {
    WPLError *e = new WPLError(t, msg, SEMANTIC, ERROR);
    errors.push_back(e);
  }

  void addSemanticCritWarning(antlr4::Token *t, std::string msg)
  {
    WPLError *e = new WPLError(t, msg, SEMANTIC, CRITICAL_WARNING);
    errors.push_back(e);
  }

  void addCodegenError(antlr4::Token *t, std::string msg)
  {
    WPLError *e = new WPLError(t, msg, CODEGEN, ERROR);
    errors.push_back(e);
  }

  std::vector<WPLError *> &getErrors() { return errors; }

  std::string errorList()
  {
    // int i = WARNING | CRITICAL_WARNING; 
    std::ostringstream errList;
    for (WPLError *e : errors)
    {
      errList << e->toString() << std::endl;
    }
    return errList.str();
  }

  bool hasErrors(int errorFlags) { 
    //If no flags provided, then return if we have any 
    if(!errorFlags) return !errors.empty(); 

    for(WPLError * err : errors)
    {
      std::cerr << err->severity << " & " << errorFlags << std::endl; 
      if(err->severity & errorFlags) return true; 
    }

    return false; //!errors.empty(); 
  }

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
    WPLError *e = new WPLError(offendingSymbol, msg, SYNTAX, ERROR);
    errors.push_back(e);
    // throw std::invalid_argument("test error thrown: " + msg);
  }
};