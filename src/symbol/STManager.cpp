#include "STManager.h"


Scope& STManager::enterScope() {
    //This is safe because we use optionals
    Scope* next = new Scope(this->currentScope);
    next->setId(this->scopeNumber++);

    this->currentScope = std::optional<Scope*>{next};
}