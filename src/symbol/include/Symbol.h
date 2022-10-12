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

#include <any>      // Needed for anycasts
#include <utility>  // Needed for anycasts
#include <iostream> // cout
#include <vector>   // Vectors
#include <optional> // Optionals


/*******************************************
 * 
 * Top Type Definition
 * 
 *******************************************/
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

/*******************************************
 * 
 * Integer (32 bit, signed) Type Definition
 * 
 *******************************************/
class TypeInt : public Type
{
public:
    std::string toString() const override { return "INT"; }
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override { return llvm::Type::getInt32Ty(C); }

protected:
    bool isSupertypeFor(const Type *other) const override;
};


/*******************************************
 * 
 *     Boolean (1 bit) Type Definition
 * 
 *******************************************/

class TypeBool : public Type
{
public:
    std::string toString() const override { return "BOOL"; }
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override { return llvm::Type::getInt1Ty(C); }

protected:
    bool isSupertypeFor(const Type *other) const override;
};


/*********************************************
 * 
 * String (dynamic allocation) Type Definition
 * 
 *********************************************/

class TypeStr : public Type
{
public:
    std::string toString() const override { return "STR"; }
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override { return llvm::Type::getInt8PtrTy(C); }

protected:
    bool isSupertypeFor(const Type *other) const override;
};


/*******************************************
 * 
 * Bottom/Unit Type
 * 
 *******************************************/

class TypeBot : public Type
{
public:
    std::string toString() const override { return "BOT"; }

protected:
    bool isSupertypeFor(const Type *other) const override;
};


/*******************************************
 * 
 * Basic Types
 * 
 *******************************************/

namespace Types
{
    inline const Type *INT = new TypeInt();
    inline const Type *BOOL = new TypeBool();
    inline const Type *STR = new TypeStr();
    inline const Type *UNDEFINED = new TypeBot();
};

/*******************************************
 * 
 * Fixed-Length Array Type Definition
 * 
 *******************************************/
class TypeArray : public Type
{
private:
    const Type *valueType;
    int length;

public:
    TypeArray(const Type *v, int l)
    {
        valueType = v;
        length = l;
    }

    std::string toString() const override
    {
        std::ostringstream description;
        description << valueType->toString()  << "[" << length << "]";

        return description.str(); 
    }

    const Type *getValueType() const { return valueType; }


    int getLength() const { return length; }

    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override
    {
        uint64_t len = (uint64_t)length;
        llvm::Type *inner = valueType->getLLVMType(C);
        llvm::Type *arr = llvm::ArrayType::get(inner, len);

        return arr;
    }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        if (const TypeArray *p = dynamic_cast<const TypeArray *>(other))
        {
            return p->valueType->isSubtype(valueType) && this->length == p->length;
        }

        return false;
    }
};


/*******************************************
 * 
 * Invokable (FUNC/PROC) Type Definition
 * 
 *******************************************/

class TypeInvoke : public Type
{
private:
    std::vector<const Type *> paramTypes;
    const Type * retType;

    bool variadic = false;

public:
    TypeInvoke()
    {
        retType = Types::UNDEFINED;
    }

    TypeInvoke(std::vector<const Type *> p)
    {
        paramTypes = p;
        retType = Types::UNDEFINED;
    }

    TypeInvoke(std::vector<const Type *> p, bool v)
    {
        paramTypes = p;
        retType = Types::UNDEFINED;

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

        variadic = v;
    }
    
    std::string toString() const override
    {
        bool isProc = dynamic_cast<const TypeBot*>(retType);

        std::ostringstream description;
        description << (isProc ? "PROC " : "FUNC ");
        for(unsigned int i = 0; i < paramTypes.size(); i++)
        {
            description << paramTypes.at(i)->toString(); 

            if(i + 1 < paramTypes.size())
            {
                description << ", "; 
            }
        }

        if (variadic)
            description << ", ... ";

        description << "-> ";

        description << retType->toString();
        return description.str();
    }

    // FIXME: DO THESE NEED LLVM TYPES???

    std::vector<const Type *> getParamTypes() const { return paramTypes; }
    const Type * getReturnType() const { return retType; }

    bool isVariadic() const { return variadic; }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        if (const TypeInvoke *p = dynamic_cast<const TypeInvoke *>(other))
        {
            if (p->paramTypes.size() != this->paramTypes.size())
                return false;

            for (unsigned int i = 0; i < this->paramTypes.size(); i++)  
            {
                if (this->paramTypes.at(i)->isNotSubtype(p->paramTypes.at(i)))
                    return false;
            }

            // check return types
            // if ((this->retType.has_value() ^ p->retType.has_value()))
            //     return false;
            // if (this->retType.has_value())
            // {
                return this->retType->isSubtype(p->retType);
            // }

            return (this->variadic == p->variadic);
        }
        return false;
    }
};

/*******************************************
 * 
 * Type used for Type Inference
 * 
 *******************************************/
class TypeInfer : public Type
{
private:
    std::optional<const Type *> * valueType; //Actual type acting as 
    std::vector<const TypeInfer *> infTypes; 

public:
    TypeInfer()
    {
        valueType = new std::optional<const Type*>;
    }

    bool hasBeenInferred() const {return valueType->has_value(); }

    std::string toString() const override
    {
        if(valueType->has_value())
        {
            return "{VAR/" + valueType->value()->toString() + "}";
        }
        return "VAR";
    }

    // std::optional<const Type *> getValueType() const { return valueType; }

    
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override
    {
        if(valueType->has_value()) return valueType->value()->getLLVMType(C); 

        //This should never happen: we should have always detected such cases in our semantic analyis
        return nullptr; 
    }

    //FIXME: VERIFY NO PARODY IN CODEGEN!

    //NOTE: SHOULD NEVER PASS INF TO THIS!
    bool setValue(const Type * other) const
    {   
        if(dynamic_cast<const TypeInfer*>(other)) return false; 

        if(valueType->has_value())
        {
            return valueType->value()->isSubtype(other);
        }
        
        
        TypeInfer * mthis =  const_cast<TypeInfer*> (this);
        *mthis->valueType = other;

        bool valid = true; 
        for(const TypeInfer* ty : infTypes) {
            if(!ty->setValue(other)) {
                valid = false; 
            }
        }

        return valid; 
    }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        if(valueType->has_value()) return valueType->value()->isSubtype(other);

        TypeInfer * mthis =  const_cast<TypeInfer*> (this);

        if(const TypeInfer * oinf = dynamic_cast<const TypeInfer*>(other))
        {
            if(oinf->valueType->has_value())
            {
                // *mthis->valueType = oinf->valueType->value(); 
                return setValue(oinf->valueType->value());
                // return true; 
            }

            mthis->infTypes.push_back(oinf);
            // return true; 



            TypeInfer *moth = const_cast<TypeInfer*>(oinf); 
            moth->infTypes.push_back(this);
            return true;
            // if(oinf->valueType->has_value())
            // {
                // mthis->valueType = oinf->valueType; //other; 
                // return true; 
            // }

            //Cannot assign undefined
            // return false;
        }

        
        // std::optional<const Type *> * next = new std::optional<const Type*>(other);


        // *mthis->valueType = other;//next;///other; 

        // return true;

        return setValue(other);
    }
};


/*******************************************
 * 
 * Symbol Definition
 * 
 *******************************************/
struct Symbol
{
    std::string identifier; // Mostly needed for our tostring function
    const Type *type;       // Keeps track of the symbol's type

    llvm::AllocaInst *val;

    bool isGlobal; 

    // Constructs a symbol from an ID and symbol type.
    Symbol(std::string id, const Type *t, bool glob=false)
    {
        identifier = id;
        type = t;
        isGlobal = glob; 

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
