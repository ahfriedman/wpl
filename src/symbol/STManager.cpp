#include "STManager.h"

Scope &STManager::enterScope()
{
    // This is safe because we use optionals
    Scope *next = new Scope(this->currentScope);
    next->setId(this->scopeNumber++);

    this->currentScope = std::optional<Scope *>{next};
    scopes.push_back(next);

    return *next;
}

std::optional<const Scope *> STManager::exitScope()
{
    // FIXME: Potential memory leak
    if (!currentScope)
    {
        return std::optional<const Scope *>{};
    }

    Scope *last = currentScope.value();

    currentScope = last->getParent();
    // scopes.pop_back(); //Delete last element in vector //FIXME: why doesn't example do this?

    return std::optional<const Scope *>{last};
}

bool STManager::addSymbol(Symbol *symbol)
{
    if(!currentScope){
        return {};
    }

    Scope* current = currentScope.value(); 

    // Check to see if it exists
    std::string id = symbol->identifier;
    if (current->lookup(id))
    {
        // Change if you want to throw an exception
        // return {}; 
        return false; 
    }
   return current->addSymbol(symbol);
}

std::optional<Symbol*> STManager::lookup(std::string id) {
    std::optional<Scope*> opt = currentScope; 

    while(opt) {
        Scope* scope = opt.value(); 

        std::optional<Symbol*> sym = scope->lookup(id);
        if(sym) return sym; 

        opt = scope->getParent(); 
    }

    return {};
}

//Directly from sample
std::string STManager::toString() const {
  std::ostringstream description;
  for (auto scope : scopes) {
    description << scope->toString();
  }
  return description.str();
}