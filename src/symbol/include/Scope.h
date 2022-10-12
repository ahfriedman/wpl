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
            parent = p;
        }

        bool addSymbol(Symbol* symbol);
        bool addSymbol(std::string id, Type* t); 
        std::optional<Symbol*>  lookup(std::string id);
        
        std::optional<Scope*> getParent() { return parent; }
        void setId(int id) { scopeId = id; }
        int getId() { return scopeId; }
        std::string toString() const; 


        std::vector<const Symbol*> getUninferred() {
            std::vector<const Symbol*> ans; 

            for(auto item : symbols) {
                if(const TypeInfer * inf = dynamic_cast<const TypeInfer *>(item.second->type))
                {
                    if(!inf->hasBeenInferred())
                        ans.push_back(item.second); 
                }
            }

            return ans; 
        }



    private:
        int scopeId = -1; 
        std::optional<Scope*> parent = {}; 
        std::map<std::string, Symbol*> symbols; 
};
