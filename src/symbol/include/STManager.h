#pragma once

/**
 * @file STManager.h
 * @author Alex Friedman (ahfriedman.com)
 * @brief 
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 * 
 * Modified from starter code. 
 */
#include "Scope.h"
#include <vector>
#include <optional>

class STManager {
  public:
    STManager(){};
    Scope& enterScope();
    std::optional<Scope*> exitScope();

    // Pass through methods
    bool addSymbol(Symbol* symbol);
    // Symbol* addSymbol(std::string id, SymType t);
    std::optional<Symbol*> lookup(std::string id);

    // Miscellaneous (useful for testing)
    std::optional<Scope*> getCurrentScope() { 
      if(currentScope) return std::optional<Scope*> {currentScope.value()};
      return std::optional<Scope*>{}; 
    }
    int scopeCount() { return scopes.size(); }
    std::string toString() const;

  private:
    std::vector<Scope*> scopes;
    std::optional<Scope*> currentScope = {}; //FIXME: Do something better here!!! 
    int scopeNumber = 0;
};