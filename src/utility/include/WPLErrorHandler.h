#include "antlr4-runtime.h"
#include <string>
#include <vector>
#include <sstream>

enum ErrType {SEMANTIC};

struct CalcError {
  ErrType type;
  antlr4::Token* token;
  std::string message;

  std::string toString() {
    std::ostringstream e;
    e << '[' << token->getLine() << ',' << token->getCharPositionInLine()
      << "]: " << message;
    return e.str();
  }
};

class CalcErrorHandler {
  public:
    void addSemanticError(antlr4::Token* t, std::string msg) {
      CalcError* e = new CalcError();
      e->type = SEMANTIC;
      e->token = t;
      e->message = msg;
      errors.push_back(e);
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