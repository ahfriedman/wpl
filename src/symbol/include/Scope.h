/**
 * @file Scope.h
 * @author gpollice
 * @brief Scope (symbol table)
 * @version 0.1
 * @date 2022-07-17
 */
#pragma once
#include "Symbol.h"
#include <map>

class Scope {
  public:
    Scope() { parent = nullptr; } // default constructor
    Scope(Scope* p) { parent = p; }
    
    // Symbol* addSymbol(Symbol& symbol);
    Symbol* addSymbol(std::string id, SymType t); // returns nullptr if duplicate
    Symbol* findSymbol(std::string id);
    Scope* getParent() { return parent; }
    void setId(int id) { scopeId = id; }  // used by STManager
    int getId() { return scopeId; }
    std::string toString() const;

  private:
    int scopeId = -1;    // The index in the symbol table manager.
    Scope* parent;
    std::map<std::string, Symbol*> symbols;
};