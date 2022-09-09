#pragma once
/**
 * @file Scope.h
 * @author Alex Friedman (ahfriedman.com)
 * @brief Scope header file
 * @version 0.1
 * @date 2022-09-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "Symbol.h"
#include <map>
#include <optional>
// #include <assert.h>


class Scope {
    public:
        Scope() {
            //By default, we set parent to be empty
        }

        /**
         * @brief Construct a new Scope object
         * 
         * @param p The parent to the current scope
         */
        Scope(std::optional<Scope*> p) {
            // assert(p->has_value());  //Ensure we have a value. //FIXME: should probably do this better
            parent = p;
        }
        //FIXME: destructors? 

        bool addSymbol(std::string id, SymbolType t); 
        std::optional<Symbol*>  lookup(std::string id);
        
        std::optional<Scope*> getParent() { return parent; }
        void setId(int id) { scopeId = id; }
        int getId() { return scopeId; }
        std::string toString() const; 



    private:
        int scopeId = -1; 
        std::optional<Scope*> parent = {}; 
        std::map<std::string, Symbol*> symbols; 
};
