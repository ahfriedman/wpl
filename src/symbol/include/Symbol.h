#pragma once

/**
 * @file Symbol.h
 * @author Alex Friedman (ahfriedman.com)
 * @brief Stores information about a specific symbol in the language. Based on starter code.
 * @version 0.1
 * @date 2022-09-09
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <string>  //Includes strings
#include <sstream> //Used for string streams

//Needed for anycasts
#include <any>
#include <utility>

//cout 
#include <iostream>

// FIXME: we may need arrays, functions & procedures.

// class Type 
// {

// public: 
//     // enum BaseTypes {
//     //     INT,        // Integers
//     //     BOOL,       // Booleans
//     //     STR,        // Strings
//     //     BOT,        // Bottom - Should be impossible
//     //     TOP,        // Top    - Universal Type - Shouldn't be used
//     // };

// protected:
//     //Used to print the name of the type
//     virtual void toString() const = 0;
//     virtual bool operator==(Type other); 
//     virtual bool operator!=(Type other);
// };


class Type {
public: 
    virtual ~Type() = default;

    virtual std::string toString() { return "TOP"; }
    // friend bool operator==(Type rhs) { return this.equals(rhs); }
    bool operator==(Type* rhs) {
        std::cout << "LHS IS " << this->toString() << std::endl; 
        std::cout << "RHS IS " << rhs->toString() << std::endl; 
        return this->equals(rhs); 
    }
    // bool operator!=(Type other) { std::cout << "47" << std::endl; return false; }

protected:
    virtual bool equals(Type* other) const {
        std::cout << "TOP CALLED" << std::endl; 
        return true;
    }
};



class TypeInt : public Type {
public: 
    std::string toString() override { return "INT"; }
    
protected: 
    bool equals(Type* other) const override {
        std::cout << "INT CALLED" << std::endl; 
        return false; 
    }
};




// The possible types of symbols in the language
enum SymbolType
{
    INT,       // Integers
    BOOL,      // Booleans
    STR,       // Strings
    UNDEFINED, // Used for errors & the such
    BOT,       // Bottom type--never possible. Unsure if needed yet
};

struct Symbol
{
    std::string identifier; // Mostly needed for our tostring function
    SymbolType type;        // Keeps track of the symbol's type

    // Constructs a symbol from an ID and symbol type.
    Symbol(std::string id, SymbolType t)
    {
        identifier = id;
        type = t;
    }

    std::string toString() const
    {
        std::ostringstream description;
        std::string typeName = getStringFor(type);
        description << '[' << identifier << ", " << typeName << ']';
        return description.str();
    }

    const static std::string getStringFor(SymbolType symbol)
    {
        switch (symbol)
        {
        case INT:
            return "INT";
        case BOOL:
            return "BOOL";
        case STR:
            return "STR";
        case UNDEFINED:
            return "UNDEFINED";
        case BOT:
            return "BOTTOM";
        }
    }
};
