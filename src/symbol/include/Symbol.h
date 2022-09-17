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

// Needed for anycasts
#include <any>
#include <utility>

// cout
#include <iostream>

// Vectors
#include <vector>

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

// Object slicing is NOT fun
class Type
{
public:
    virtual ~Type() = default;

    virtual std::string toString() const { return "TOP"; }

    virtual bool is(const Type *other) const { return this->equals(other); }
    virtual bool isNot(const Type *other) const { return !(this->equals(other)); }

protected:
    virtual bool equals(const Type *other) const { return true; }
};

class TypeInt : public Type
{
public:
    std::string toString() const override { return "INT"; }

protected:
    bool equals(const Type *other) const override
    {
        return dynamic_cast<const TypeInt *>(other);
    }
};

class TypeBool : public Type
{
public:
    std::string toString() const override { return "BOOL"; }

protected:
    bool equals(const Type *other) const override
    {
        return dynamic_cast<const TypeBool *>(other);
    }
};

class TypeStr : public Type
{
public:
    std::string toString() const override { return "STR"; }

protected:
    bool equals(const Type *other) const override
    {
        return dynamic_cast<const TypeStr *>(other);
    }
};

class TypeBot : public Type
{
public:
    std::string toString() const override { return "BOT"; }

protected:
    bool equals(const Type *other) const override
    {
        return false;
    }
};

class TypeProc : public Type
{
private:
    std::vector<const Type *> paramTypes;

public:
    TypeProc(std::vector<const Type *> p)
    {
        paramTypes = p;
    }

    // FIXME: do better
    std::string toString() const override
    {
        std::ostringstream description;
        description << "PROC ";
        for (auto param : paramTypes)
        {
            description << param->toString() << " ";
        }
        description << " -> BOT"; 
        return description.str();
    }

protected:
    bool equals(const Type *other) const override
    {
        if(const TypeProc* p = dynamic_cast<const TypeProc *>(other)) {
            if(p->paramTypes.size() != this->paramTypes.size()) return false; 

            //FIXME: ensure good enough!
            for(unsigned int i = 0; i < this->paramTypes.size(); i++) {
                if(this->paramTypes.at(i)->isNot(p->paramTypes.at(i)))
                    return false; 
            }

            return true; 
        }
        return false;
    }
};

namespace Types
{
    inline const Type *INT = new TypeInt();
    inline const Type *BOOL = new TypeBool();
    inline const Type *STR = new TypeStr();
    inline const Type *UNDEFINED = new TypeBot();
};

struct Symbol
{
    std::string identifier; // Mostly needed for our tostring function
    const Type *type;       // Keeps track of the symbol's type

    // Constructs a symbol from an ID and symbol type.
    Symbol(std::string id, const Type *t)
    {
        identifier = id;
        type = t;
    }

    std::string toString() const
    {
        std::ostringstream description;
        std::string typeName = type->toString(); // getStringFor(type);
        description << '[' << identifier << ", " << typeName << ']';
        return description.str();
    }
};
