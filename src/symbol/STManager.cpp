#include "STManager.h"

Scope &STManager::enterScope(bool insertStop)
{
    // This is safe because we use optionals
    Scope *next = new Scope(this->currentScope);
    next->setId(this->scopeNumber++);

    this->currentScope = std::optional<Scope *>{next};
    scopes.push_back(next);

    if (insertStop)
        stops.push(scopes.size() - 1);

    return *next;
}

std::optional<Scope *> STManager::exitScope()
{
    // INFO: Potential memory leak
    if (!currentScope)
    {
        return {};
    }

    Scope *last = currentScope.value();

    currentScope = last->getParent();
    scopes.pop_back(); // FIXME: Delete last element in vector? -> We don't do this because it breaks the scope count

    int depth = scopes.size();
    if (getCurrentStop() == depth && getCurrentStop() != 0)
    {
        stops.pop();
    }

    return std::optional<Scope *>{last};
}

bool STManager::addSymbol(Symbol *symbol)
{
    if (!currentScope)
    {
        return {};
    }

    Scope *current = currentScope.value();

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

std::optional<Symbol *> STManager::lookup(std::string id)
{
    std::optional<Scope *> opt = currentScope;

    int depth = scopes.size() - 1;
    int stop = this->getCurrentStop();
    while (opt)
    {
        Scope *scope = opt.value();

        std::optional<Symbol *> sym = scope->lookup(id);
        if (sym)
        {
            if (depth >= stop || sym.value()->isDefinition || sym.value()->isGlobal)
                return sym;
            return {};
        }

        depth--;
        opt = scope->getParent();
    }

    return {};
}

std::optional<Symbol *> STManager::lookupInCurrentScope(std::string id)
{
    std::optional<Scope *> opt = currentScope;

    if (opt)
    {
        Scope *scope = opt.value();
        std::optional<Symbol *> sym = scope->lookup(id);
        if (sym)
            return sym;

        opt = scope->getParent();
        while (opt)
        {
            scope = opt.value();
            std::optional<Symbol *> sym = scope->lookup(id);
            if (sym)
            {
                // std::cout << sym.value()->toString() << " " << depth << " >= " << stop  << " || " << sym.value()->isDefinition << std::endl;
                if (sym.value()->isDefinition) // FIXME: VERIFY, ALSO MAY GET CONFUSING IF WE HAVE DUPLICATE NAMED VARS!!!
                    return sym;
                return {};
            }
            opt = scope->getParent();
        }
    }

    return {};
}

// Directly from sample
std::string STManager::toString() const
{
    std::ostringstream description;
    for (auto scope : scopes)
    {
        description << scope->toString();
    }
    return description.str();
}