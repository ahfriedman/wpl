// #include "Symbol.h"
#include "Scope.h"

/**
 * @brief Adds a symbol to the scope. 
 * 
 * @param id The identifier for the symbol 
 * @param t The type of the symbol
 * @return true If the symbol was added
 * @return false If the symbol was already defined
 */
bool Scope::addSymbol(std::string id,  SymbolType t) {
    Symbol* symbol = new Symbol(id, t); 
    if (symbols.find(id) != symbols.end()) {
        //Symbol already defined 
        delete symbol; //Save the memory 
        return false; 
    }

   auto ret =  symbols.insert({id, symbol}).first;
   return ret->second; 
}

/**
 * @brief Searches for a token in the given scope. 
 * 
 * @param id The identifier of the token to search for
 * @return std::optional<Symbol*> - Empty if not found; value provided if found. 
 */
std::optional<Symbol*> Scope::lookup(std::string id) {
    auto symbol = symbols.find(id); 

    if(symbol == symbols.end()) return {}; 

    return symbol->second; 
}

//Modified from starter 
std::string Scope::toString() const {
  std::ostringstream description;
  description << std::endl << "-------------------" << std::endl << "SCOPE: " << scopeId;
  if (parent) {
    description << " PARENT: " << (parent.value())->scopeId;
  }
  description << std::endl << '{';
  for (auto sym : symbols) {
    description << std::endl << "    " << sym.second->toString();
  }
  description << std:: endl << '}' << std::endl;

  return description.str(); 
}

