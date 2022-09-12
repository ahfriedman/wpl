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

// FIXME: we may need arrays, functions & procedures.

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
