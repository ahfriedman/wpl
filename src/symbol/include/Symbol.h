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
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"

// Needed for anycasts
#include <any>
#include <utility>

// cout
#include <iostream>

// Vectors
#include <vector>

// Optional
#include <optional>

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
    virtual bool isSubtype(const Type* other) const; 
    virtual bool isNotSubtype(const Type *other) const { return !(isSubtype(other)); }

    virtual bool isSupertype(const Type* other) const { return isSupertypeFor(other); }
    virtual bool isNotSupertype(const Type* other) const { return !isSupertypeFor(other); }

    virtual llvm::Type *getLLVMType(llvm::LLVMContext &C) const { return llvm::Type::getVoidTy(C); }

protected:
    virtual bool isSupertypeFor(const Type *other) const { return true; }
};

class TypeInt : public Type
{
public:
    std::string toString() const override { return "INT"; }
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override { return llvm::Type::getInt32Ty(C); }

protected:
    bool isSupertypeFor(const Type *other) const override;
};

class TypeBool : public Type
{
public:
    std::string toString() const override { return "BOOL"; }
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override { return llvm::Type::getInt1Ty(C); }

protected:
    bool isSupertypeFor(const Type *other) const override;
};

class TypeStr : public Type
{
public:
    std::string toString() const override { return "STR"; }
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override { return llvm::Type::getInt8PtrTy(C); }

protected:
    bool isSupertypeFor(const Type *other) const override;
};

class TypeBot : public Type
{
public:
    std::string toString() const override { return "BOT"; }

protected:
    bool isSupertypeFor(const Type *other) const override;
};

class TypeArray : public Type
{
private:
    const Type *valueType;
    int length;
    // int
    // FIXME: should we have a length defined in here?

public:
    TypeArray(const Type *v, int l)
    {
        valueType = v;
        length = l;
    }

    std::string toString() const override
    {
        // FIXME: DO BETTER
        return valueType->toString() + "[]";
    }

    const Type *getValueType() const { return valueType; }

    // FIXME: ENSURE THESE ARE ALL GOOD ENOUGH!
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override
    {
        uint64_t len = (uint64_t)length;
        llvm::Type *inner = valueType->getLLVMType(C);
        llvm::Type *arr = llvm::ArrayType::get(inner, len);
        // return llvm::Type::getInt8PtrTy(C);
        return arr;
    }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        // FIXME: do better!
        if (const TypeArray *p = dynamic_cast<const TypeArray *>(other))
        {
            return valueType->isSubtype(p->valueType) && this->length == p->length;
        }

        return false;
    }
};

class TypeInvoke : public Type
{
private:
    std::vector<const Type *> paramTypes;
    std::optional<const Type *> retType;

    bool variadic = false;

public:
    TypeInvoke()
    {
        retType = {};
    }

    TypeInvoke(std::vector<const Type *> p)
    {
        paramTypes = p;
        retType = {};
    }

    TypeInvoke(std::vector<const Type *> p, bool v)
    {
        paramTypes = p;
        retType = {};

        variadic = v;
    }

    TypeInvoke(std::vector<const Type *> p, const Type *r)
    {
        paramTypes = p;
        retType = r;
    }

    TypeInvoke(std::vector<const Type *> p, const Type *r, bool v)
    {
        paramTypes = p;
        retType = r;

        // FIXME: MAKE SURE AT LEAST ONE PARAMTYPE!!!
        variadic = v;
    }

    // FIXME: do better
    std::string toString() const override
    {
        bool isProc = !retType.has_value();

        std::ostringstream description;
        description << (isProc ? "PROC " : "FUNC ");
        for (auto param : paramTypes)
        {
            description << param->toString() << " ";
        }

        if (variadic)
            description << "... "; // FIXME: DO BETTER!

        description << (isProc ? "-> BOT" : ("-> " + retType.value()->toString()));
        return description.str();
    }

    // FIXME: DO THESE NEED LLVM TYPES???

    std::vector<const Type *> getParamTypes() const { return paramTypes; }
    std::optional<const Type *> getReturnType() const { return retType; }

    bool isVariadic() const { return variadic; }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        if (const TypeInvoke *p = dynamic_cast<const TypeInvoke *>(other))
        {
            if (p->paramTypes.size() != this->paramTypes.size())
                return false;

            // FIXME: ensure good enough! Probbaly is wrong for arrays--esp if not given len!!!
            for (unsigned int i = 0; i < this->paramTypes.size(); i++)
            {
                if (this->paramTypes.at(i)->isNotSubtype(p->paramTypes.at(i)))
                    return false;
            }

            // check returnh types
            if ((this->retType.has_value() ^ p->retType.has_value()))
                return false;
            if (this->retType.has_value())
            {
                return this->retType.value()->isSubtype(p->retType.value());
            }

            return (this->variadic == p->variadic);
        }
        return false;
    }
};


class TypeInfer : public Type
{
private:
    std::optional<const Type *> valueType; //Actual type acting as 

public:
    TypeInfer()
    {
        valueType = {};
    }

    std::string toString() const override
    {
        // FIXME: DO BETTER
        return "VAR";
    }

    std::optional<const Type *> getValueType() const { return valueType; }

    // FIXME: ENSURE THESE ARE ALL GOOD ENOUGH!
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override
    {
        //FIXME: what happens if we don't get a value type by code gen!!!!
        if(valueType) return valueType.value()->getLLVMType(C); 
        return nullptr; 
    }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        // std::cout << "SYM 300" << std::endl;
        if(valueType) return valueType.value()->isSubtype(other);
        //FIXME: DO BETTER!!! MAY NEED TO LIMIT THIS TO NOT BE FNS, BOTs, ETC!!!!
        std::cout << "SYM 303 - WILL SET AS " << other->toString() << std::endl;
        TypeInfer * mthis =  const_cast<TypeInfer*> (this);
        std::cout << "SYM 305" << std::endl;
        mthis->valueType = other; 
        std::cout << "SYM 307" << std::endl;
        return true;
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

    llvm::AllocaInst *val;

    // Constructs a symbol from an ID and symbol type.
    Symbol(std::string id, const Type *t)
    {
        identifier = id;
        type = t;

        val = nullptr; // FIXME: replace with optional?
    }

    std::string toString() const
    {
        std::ostringstream description;
        std::string typeName = type->toString(); // getStringFor(type);
        description << '[' << identifier << ", " << typeName << ']';
        return description.str();
    }
};
