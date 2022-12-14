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
#pragma once
#include "Scope.h"
#include <vector>
#include <optional>

#include <stack>

class STManager {
  public:
    STManager(){};

    /**
     * @brief Enter a new scope
     * 
     * @return Scope& the scope we entered
     */
    Scope& enterScope() {
      return enterScope(false);
    };

    Scope& enterScope(bool insertStop);
    
    /**
     * @brief Exit the current scope and move up one level
     * 
     * @return std::optional<Scope*> Returns empty if no parent scope to enter; otherwise returns last scope. 
     */
    std::optional<Scope*> exitScope();

    /**
     * @brief Add a symbol to the current scope
     * 
     * @param symbol The symbol to add
     * @return true if successful
     * @return false if unsuccessful (ie, name already bound to another symbol)
     */
    bool addSymbol(Symbol* symbol);
    
    /**
     * @brief Lookup a symbol across all scopes returning the first definition found
     * 
     * @param id The symbol name to lookup
     * @return std::optional<Symbol*>  Empty if symbol not found; present with value if found. 
     */
    std::optional<Symbol*> lookup(std::string id);

    /**
     * @brief Lookup a symbol only in the current scope. 
     * 
     * @param id The symbol name to lookup
     * @return std::optional<Symbol*>  Empty if symbol not found; present with value if found. 
     */
    std::optional<Symbol*> lookupInCurrentScope(std::string id);

    /****************************************
     * Miscellaneous (useful for testing)
     ****************************************/

    /**
     * @brief Get the Current Scope object
     * 
     * @return std::optional<Scope*> 
     */
    std::optional<Scope*> getCurrentScope() { 
      if(currentScope) return std::optional<Scope*> {currentScope.value()};
      return std::optional<Scope*>{}; 
    }

    /**
     * @brief Gets the number of scopes
     * 
     * @return int 
     */
    int scopeCount() { return scopes.size(); }
    std::string toString() const;

    /**
     * @brief Determines if the current scope is the global scope
     * 
     * @return true if current scope is the global scope
     * @return false if the current scope is not the gobal scope
     */
    bool isGlobalScope() {
      if(scopes.size() == 0) return false; 

      if(!currentScope) return false; 

      return currentScope.value()->getId() == 0; 
    }

    int getCurrentStop() {
      return stops.size() == 0 ? 0 : stops.top(); 
    }

  private:
    std::vector<Scope*> scopes;
    std::optional<Scope*> currentScope = {}; 
    int scopeNumber = 0;

    std::stack<int> stops; 
};