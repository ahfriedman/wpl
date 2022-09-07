/**
 * @file STManager.cpp
 * @author gpollice
 * @brief Symbol table manager implementation
 * @version 0.1
 * @date 2022-07-18
 */
#include "STManager.h"

Scope& STManager::enterScope() {
  Scope* s;
  if (currentScope == nullptr) {
    s = new Scope();
  } else {
    s = new Scope(currentScope);
  }
  s->setId(scopeNumber++);
  scopes.push_back(s);
  currentScope = s;
  return *s;
}

/**
 * @brief exit the scope and set the current scope to the parent.
 *  No checking is done to see if exiting the root scope. 
 * 
 * @return Scope& the scope that was exited
 */
Scope& STManager::exitScope() {
  Scope& s = *currentScope;
  currentScope = currentScope->getParent();
  return s;
}

/**
 * @brief add a symbol to the current scope
 * 
 * @param symbol the symbol to add
 * @return Symbol* pointer to the symbol that was added or 
 *  nullptr if a symbol with the same identifier already 
 *  existed in the current scope.
 */
Symbol* STManager::addSymbol(std::string id, SymType t) {
  // Check to see if it exists
  // std::string id = symbol.identifier;
  if (currentScope->findSymbol(id) != nullptr) {
    // Change if you want to throw an exception
    return nullptr;
  }
  return currentScope->addSymbol(id, t);
}

Symbol* STManager::findSymbol(std::string id) {
  Scope* scope = currentScope;
  Symbol* symbol = nullptr;
  while (scope != nullptr && symbol == nullptr) {
    symbol = scope->findSymbol(id);
    if (symbol == nullptr) {
      scope = scope->getParent();
    }
  }
  return symbol;
}

std::string STManager::toString() const {
  std::ostringstream description;
  for (auto scope : scopes) {
    description << scope->toString();
  }
  return description.str();
}