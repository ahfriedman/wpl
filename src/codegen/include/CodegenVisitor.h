/**
 * @file CodegenVisitor.h
 * @author Alex Friedman (ahfriedman.com)
 * @brief Code Generation visitor header inspired by sample code
 * @version 0.1
 * @date 2022-09-19
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "antlr4-runtime.h"
#include "WPLBaseVisitor.h"

#include "PropertyManager.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/NoFolder.h"


#include <any> 
#include <string>

using namespace llvm;

class CodegenVisitor : WPLBaseVisitor
{

public:
    CodegenVisitor(PropertyManager *p, std::string moduleName)
    {
        props = p;

        // LLVM Stuff
        context = new LLVMContext();
        module = new Module(moduleName, *context);

        // Use the NoFolder to turn off constant folding
        builder = new IRBuilder<NoFolder>(module->getContext());

        // LLVM Types

        VoidTy = llvm::Type::getVoidTy(module->getContext());
        Int32Ty = llvm::Type::getInt32Ty(module->getContext());
        Int1Ty = llvm::Type::getInt1Ty(module->getContext());
        Int8Ty = llvm::Type::getInt8Ty(module->getContext());
        Int32Zero = ConstantInt::get(Int32Ty, 0, true);
        Int32One = ConstantInt::get(Int32Ty, 1, true);
        i8p = llvm::Type::getInt8PtrTy(module->getContext());
        Int8PtrPtrTy = i8p->getPointerTo();
    }

    std::any visitCompilationUnit(WPLParser::CompilationUnitContext *ctx) override;
    std::any visitInvocation(WPLParser::InvocationContext *ctx) override;
    std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx) override;
    std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx) override;
    std::any visitIConstExpr(WPLParser::IConstExprContext *ctx) override;
    std::any visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx) override;
    std::any visitSConstExpr(WPLParser::SConstExprContext *ctx) override;
    std::any visitUnaryExpr(WPLParser::UnaryExprContext *ctx) override;
    std::any visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx) override;
    std::any visitEqExpr(WPLParser::EqExprContext *ctx) override;
    std::any visitLogAndExpr(WPLParser::LogAndExprContext *ctx) override;
    std::any visitLogOrExpr(WPLParser::LogOrExprContext *ctx) override;
    std::any visitCallExpr(WPLParser::CallExprContext *ctx) override;
    std::any visitVariableExpr(WPLParser::VariableExprContext *ctx) override;
    std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx) override;
    std::any visitParenExpr(WPLParser::ParenExprContext *ctx) override;
    std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx) override;
    std::any visitBConstExpr(WPLParser::BConstExprContext *ctx) override;
    std::any visitBlock(WPLParser::BlockContext *ctx) override;
    std::any visitCondition(WPLParser::ConditionContext *ctx) override;
    std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override;
    std::any visitParameterList(WPLParser::ParameterListContext *ctx) override;
    std::any visitParameter(WPLParser::ParameterContext *ctx) override;
        std::any visitAssignment(WPLParser::AssignmentContext *ctx) override;
    std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override;
    std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override;
    std::any visitProcDef(WPLParser::ProcDefContext *ctx) override;
    std::any visitAssignStatement(WPLParser::AssignStatementContext *ctx) override;
    std::any visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx) override;
    std::any visitLoopStatement(WPLParser::LoopStatementContext *ctx) override;
    std::any visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx) override;
    std::any visitSelectStatement(WPLParser::SelectStatementContext *ctx) override;
    std::any visitCallStatement(WPLParser::CallStatementContext *ctx) override;
    std::any visitReturnStatement(WPLParser::ReturnStatementContext *ctx) override;
    std::any visitBlockStatement(WPLParser::BlockStatementContext *ctx) override;
    std::any visitTypeOrVar(WPLParser::TypeOrVarContext *ctx) override;
    std::any visitType(WPLParser::TypeContext *ctx) override;
    std::any visitBooleanConst(WPLParser::BooleanConstContext *ctx) override;



    bool hasErrors() { return errorHandler.hasErrors(); }
    std::string getErrors() { return errorHandler.errorList(); }

    PropertyManager* getProperties() { return props; }

    //FIXME: REVIEW LLVM STUFF
    Module * getModule() { return module; }
    void modPrint() { module -> print(llvm::outs(), nullptr); }

private:
    PropertyManager *props;
    WPLErrorHandler errorHandler;

    // LLVM
    LLVMContext *context;
    Module *module;
    IRBuilder<NoFolder> *builder;

    // Commonly used types
    llvm::Type *VoidTy;
    llvm::Type *Int1Ty;
    llvm::Type *Int8Ty;
    llvm::Type *Int32Ty; //Things like 32 bit integers
    llvm::Type *i8p;
    llvm::Type *Int8PtrPtrTy;
    Constant *Int32Zero;
    Constant *Int32One;
};
