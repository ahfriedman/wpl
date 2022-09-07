/**
 * @file STManager.h
 * @author gpollice
 * @brief Symbol table (environment) manager
 * @version 0.1
 * @date 2022-07-18
 */
#pragma once
#include "Scope.h"
#include <vector>

class STManager {
  public:
    STManager(){};
    Scope& enterScope();
    Scope& exitScope();

    // Pass through methods
    Symbol* addSymbol(std::string id, SymType t);
    Symbol* findSymbol(std::string id);

    // Miscellaneous (useful for testing)
    Scope& getCurrentScope() { return *currentScope; }
    int scopeCount() { return scopes.size(); }
    std::string toString() const;

  private:
    std::vector<Scope*> scopes;
    Scope* currentScope = nullptr;
    int scopeNumber = 0;
};