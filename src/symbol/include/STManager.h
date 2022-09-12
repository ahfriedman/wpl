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
    Scope& exitScope();

    // Pass through methods
    Symbol* addSymbol(Symbol& symbol);
    // Symbol* addSymbol(std::string id, SymType t);
    std::optional<Symbol*> findSymbol(std::string id);

    // Miscellaneous (useful for testing)
    // Scope& getCurrentScope() { return *currentScope; }
    int scopeCount() { return scopes.size(); }
    std::string toString() const;

  private:
    std::vector<Scope*> scopes;
    std::optional<Scope*> currentScope = {}; //FIXME: Do something better here!!! 
    int scopeNumber = 0;
};