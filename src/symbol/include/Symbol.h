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

    /**
     * @brief Returns a human-readable string representation of the type's name.
     *
     * @return std::string The string name of the type
     */
    virtual std::string toString() const { return "TOP"; }

    /**
     * @brief Determines if this type is a subtype of another.
     *
     * @param other The type we are testing to see if we are a subtype of
     * @return true If the current type is a subtype of other
     * @return false If the current type is not a subtype of other.
     */
    virtual bool isSubtype(const Type *other) const;
    virtual bool isNotSubtype(const Type *other) const { return !(isSubtype(other)); }

    /**
     * @brief Determines if this type is a supertype of another
     *
     * @param other The posposed subtype of this type.
     * @return true If this is a supertype for other.
     * @return false If this is not a supertype for other.
     */
    virtual bool isSupertype(const Type *other) const { return isSupertypeFor(other); }
    virtual bool isNotSupertype(const Type *other) const { return !isSupertypeFor(other); }

    /**
     * @brief Gets the llvm::Type* which corresponds to the current type in LLVM
     *
     * @param C The llvm context
     * @return llvm::Type* The representation of this type in LLVM
     */
    virtual llvm::Type *getLLVMType(llvm::LLVMContext &C) const { return llvm::Type::getVoidTy(C); }

protected:
    /**
     * @brief Internal tool used to determine if this type is a supertype for another type. NOTE: THIS SHOULD NEVER BE CALLED DIRECTLY OUTSIDE OF THE TYPE DEFINITIONS. DOING SO MAY LEAD TO UNUSUAL BEHAVIOR!
     *
     * @param other The type to test against
     * @return true if this is a supertype of other
     * @return false if this is not a supertype of other.
     */
    virtual bool isSupertypeFor(const Type *other) const { return true; } // The top type is the universal supertype
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
    bool isSupertypeFor(const Type *other) const override; // Defined in .cpp
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
    bool isSupertypeFor(const Type *other) const override; // Defined in .cpp
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
    bool isSupertypeFor(const Type *other) const override; // Defined in .cpp
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
    bool isSupertypeFor(const Type *other) const override; // Defined in .cpp
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
    /**
     * @brief The type of the array elements
     *
     */
    const Type *valueType;

    /**
     * @brief The length of the array
     *
     */
    int length;

public:
    /**
     * @brief Construct a new TypeArray
     *
     * @param v The type of the array elements
     * @param l The length of the array. NOTE: THIS SHOULD ALWAYS BE AT LEAST ONE!
     */
    TypeArray(const Type *v, int l)
    {
        valueType = v;
        length = l;
    }

    /**
     * @brief Returns the name of the string in form of <valueType name>[<array length>].
     *
     * @return std::string String name representation of this type.
     */
    std::string toString() const override
    {
        std::ostringstream description;
        description << valueType->toString() << "[" << length << "]";

        return description.str();
    }

    /**
     * @brief Get the Value Type object
     *
     * @return const Type*
     */
    const Type *getValueType() const { return valueType; }

    /**
     * @brief Get the Length object
     *
     * @return int
     */
    int getLength() const { return length; }

    /**
     * @brief Gets the LLVM type for an array of the given valueType and length.
     *
     * @param C LLVM Context
     * @return llvm::Type*
     */
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
        // An array can only be a supertype of another array
        if (const TypeArray *p = dynamic_cast<const TypeArray *>(other))
        {
            /*
             * If the other array's value type is a subtype of the current
             * array's type AND their lengths match, then we can consider
             * this to be a supertype of the other array.
             */
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
    /**
     * @brief Represents the types of the function's arguments
     *
     */
    std::vector<const Type *> paramTypes;

    /**
     * @brief Represents the function's return type.
     *
     */
    const Type *retType;

    /**
     * @brief Determines if the function should be variadic.
     *
     */
    bool variadic = false;

public:
    /**
     * @brief Construct a new Type Invoke object that has no return and no arguments
     *
     */
    TypeInvoke()
    {
        retType = Types::UNDEFINED;
    }

    /**
     * @brief Construct a new Type Invoke object with the provided arguments and no return type
     *
     * @param p The types of the arguments
     */
    TypeInvoke(std::vector<const Type *> p)
    {
        paramTypes = p;
        retType = Types::UNDEFINED;
    }

    /**
     * @brief Construct a new Type Invoke object
     *
     * @param p List of type parameters
     * @param v Determines if this should be a variadic
     */
    TypeInvoke(std::vector<const Type *> p, bool v)
    {
        paramTypes = p;
        retType = Types::UNDEFINED;

        variadic = v;
    }

    /**
     * @brief Construct a new Type Invoke object
     *
     * @param p List of type parameters
     * @param r Return type
     */
    TypeInvoke(std::vector<const Type *> p, const Type *r)
    {
        paramTypes = p;
        retType = r;
    }

    /**
     * @brief Construct a new Type Invoke object
     *
     * @param p List of type parameters
     * @param r Return type
     * @param v Determines if this should be a variadic
     */
    TypeInvoke(std::vector<const Type *> p, const Type *r, bool v)
    {
        paramTypes = p;
        retType = r;

        variadic = v;
    }

    /**
     * @brief Returns a string representation of the type in format: <PROC | FUNC> (param_0, param_1, ...) -> return_type.
     *
     * @return std::string
     */
    std::string toString() const override
    {
        bool isProc = dynamic_cast<const TypeBot *>(retType);

        std::ostringstream description;
        description << (isProc ? "PROC " : "FUNC ");
        for (unsigned int i = 0; i < paramTypes.size(); i++)
        {
            description << paramTypes.at(i)->toString();

            if (i + 1 < paramTypes.size())
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

    /**
     * @brief Get the Param Types object
     *
     * @return std::vector<const Type *>
     */
    std::vector<const Type *> getParamTypes() const { return paramTypes; }

    /**
     * @brief Get the Return Type object
     *
     * @return const Type*
     */
    const Type *getReturnType() const { return retType; }

    /**
     * @brief Returns if this is a variadic
     *
     * @return true
     * @return false
     */
    bool isVariadic() const { return variadic; }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        //Checks that the other type is also invokable
        if (const TypeInvoke *p = dynamic_cast<const TypeInvoke *>(other))
        {
            //Makes sure that both functions have the same number of parameters
            if (p->paramTypes.size() != this->paramTypes.size())
                return false;

            //Makes sure both functions have the same variadic status
            if (this->variadic != p->variadic)
                return false;

            //Checks that the parameters of this function are all subtypes of the other
            for (unsigned int i = 0; i < this->paramTypes.size(); i++)
            {
                if (this->paramTypes.at(i)->isNotSubtype(p->paramTypes.at(i)))
                    return false;
            }

            //Makes sure that the return type of this function is a subtype of the other
            return this->retType->isSubtype(p->retType);
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
    /**
     * @brief Optional type that represents the inferred type (type this is acting as). Empty if inference was unable to determine the type or is not complete
     * 
     */
    std::optional<const Type *> *valueType; 

    /**
     * @brief Keeps track of all the other inferred types that this shares a dependency with.
     * 
     */
    std::vector<const TypeInfer *> infTypes;

public:
    TypeInfer()
    {
        valueType = new std::optional<const Type *>;
    }

    /**
     * @brief Returns if type inference has detemined the type of this var yet
     * 
     * @return true 
     * @return false 
     */
    bool hasBeenInferred() const { return valueType->has_value(); }

    /**
     * @brief Returns VAR if type inference has not been completed or {VAR/<INFERRED TYPE>} if type inference has completed.
     * 
     * @return std::string 
     */
    std::string toString() const override
    {
        if (hasBeenInferred())
        {
            return "{VAR/" + valueType->value()->toString() + "}";
        }
        return "VAR";
    }


    /**
     * @brief Gets the LLVM representation of the inferred type. 
     * 
     * @param C LLVM Context
     * @return llvm::Type* the llvm type for the inferred type. 
     */
    llvm::Type *getLLVMType(llvm::LLVMContext &C) const override
    {
        if (valueType->has_value())
            return valueType->value()->getLLVMType(C);

        // This should never happen: we should have always detected such cases in our semantic analyis
        return nullptr;
    }

    // TODO: There shouldn't be any parody in codegen, but something does seem off.

    // NOTE: SHOULD NEVER PASS INF TO THIS!
    bool setValue(const Type *other) const
    {
        if (dynamic_cast<const TypeInfer *>(other))
            return false;

        if (valueType->has_value())
        {
            return valueType->value()->isSubtype(other);
        }

        TypeInfer *mthis = const_cast<TypeInfer *>(this);
        *mthis->valueType = other;

        bool valid = true;
        for (const TypeInfer *ty : infTypes)
        {
            if (!ty->setValue(other))
            {
                valid = false;
            }
        }

        return valid;
    }

protected:
    bool isSupertypeFor(const Type *other) const override
    {
        if (valueType->has_value())
            return valueType->value()->isSubtype(other);

        TypeInfer *mthis = const_cast<TypeInfer *>(this);

        if (const TypeInfer *oinf = dynamic_cast<const TypeInfer *>(other))
        {
            if (oinf->valueType->has_value())
            {
                // *mthis->valueType = oinf->valueType->value();
                return setValue(oinf->valueType->value());
                // return true;
            }

            mthis->infTypes.push_back(oinf);
            // return true;

            TypeInfer *moth = const_cast<TypeInfer *>(oinf);
            moth->infTypes.push_back(this);
            return true;
            // if(oinf->valueType->has_value())
            // {
            // mthis->valueType = oinf->valueType; //other;
            // return true;
            // }

            // Cannot assign undefined
            //  return false;
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
    Symbol(std::string id, const Type *t, bool glob = false)
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
