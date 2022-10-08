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
#include "CompilerFlags.h"

// #include "WPLTypedVisitor.h"

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
#include <regex>

#include <variant>

// using namespace llvm;
using llvm::ArrayRef;
using llvm::ArrayType;
using llvm::BasicBlock;
using llvm::Constant;
using llvm::ConstantInt;
using llvm::Function;
using llvm::FunctionCallee;
using llvm::FunctionType;
using llvm::GlobalValue;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;
using llvm::NoFolder;
using llvm::StringRef;
using llvm::Value;
using llvm::PHINode; 

class CodegenVisitor : WPLBaseVisitor
{

public:
    CodegenVisitor(PropertyManager *p, std::string moduleName, int f=0)
    {
        props = p;
        flags = f; 

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

    std::optional<Value *> TvisitCompilationUnit(WPLParser::CompilationUnitContext *ctx);
    std::optional<Value *> TvisitInvocation(WPLParser::InvocationContext *ctx);
    std::optional<Value *> TvisitArrayAccess(WPLParser::ArrayAccessContext *ctx);
    std::optional<Value *> TvisitArrayOrVar(WPLParser::ArrayOrVarContext *ctx);

    std::optional<Value *> TvisitIConstExpr(WPLParser::IConstExprContext *ctx);
    std::optional<Value *> TvisitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx);
    std::optional<Value *> TvisitSConstExpr(WPLParser::SConstExprContext *ctx);
    std::optional<Value *> TvisitUnaryExpr(WPLParser::UnaryExprContext *ctx);
    std::optional<Value *> TvisitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx);
    std::optional<Value *> TvisitEqExpr(WPLParser::EqExprContext *ctx);
    std::optional<Value *> TvisitLogAndExpr(WPLParser::LogAndExprContext *ctx);
    std::optional<Value *> TvisitLogOrExpr(WPLParser::LogOrExprContext *ctx);
    std::optional<Value *> TvisitCallExpr(WPLParser::CallExprContext *ctx);
    std::optional<Value *> TvisitVariableExpr(WPLParser::VariableExprContext *ctx);
    std::optional<Value *> TvisitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx);
    std::optional<Value *> TvisitParenExpr(WPLParser::ParenExprContext *ctx);
    std::optional<Value *> TvisitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx);
    std::optional<Value *> TvisitBConstExpr(WPLParser::BConstExprContext *ctx);
    std::optional<Value *> TvisitBlock(WPLParser::BlockContext *ctx);
    std::optional<Value *> TvisitCondition(WPLParser::ConditionContext *ctx);
    std::optional<Value *> TvisitSelectAlternative(WPLParser::SelectAlternativeContext *ctx);
    std::optional<Value *> TvisitParameterList(WPLParser::ParameterListContext *ctx);
    std::optional<Value *> TvisitParameter(WPLParser::ParameterContext *ctx);
    std::optional<Value *> TvisitAssignment(WPLParser::AssignmentContext *ctx);
    std::optional<Value *> TvisitExternStatement(WPLParser::ExternStatementContext *ctx);
    std::optional<Value *> TvisitFuncDef(WPLParser::FuncDefContext *ctx);
    std::optional<Value *> TvisitProcDef(WPLParser::ProcDefContext *ctx);
    std::optional<Value *> TvisitAssignStatement(WPLParser::AssignStatementContext *ctx);
    std::optional<Value *> TvisitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx);
    std::optional<Value *> TvisitLoopStatement(WPLParser::LoopStatementContext *ctx);
    std::optional<Value *> TvisitConditionalStatement(WPLParser::ConditionalStatementContext *ctx);
    std::optional<Value *> TvisitSelectStatement(WPLParser::SelectStatementContext *ctx);
    std::optional<Value *> TvisitCallStatement(WPLParser::CallStatementContext *ctx);
    std::optional<Value *> TvisitReturnStatement(WPLParser::ReturnStatementContext *ctx);
    std::optional<Value *> TvisitBlockStatement(WPLParser::BlockStatementContext *ctx);
    std::optional<Value *> TvisitTypeOrVar(WPLParser::TypeOrVarContext *ctx);
    std::optional<Value *> TvisitType(WPLParser::TypeContext *ctx);
    std::optional<Value *> TvisitBooleanConst(WPLParser::BooleanConstContext *ctx);

    std::any visitCompilationUnit(WPLParser::CompilationUnitContext *ctx) override { return TvisitCompilationUnit(ctx); };
    std::any visitInvocation(WPLParser::InvocationContext *ctx) override { return TvisitInvocation(ctx); };
    std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx) override { return TvisitArrayAccess(ctx); };
    std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx) override { return TvisitArrayOrVar(ctx); };

    std::any visitIConstExpr(WPLParser::IConstExprContext *ctx) override { return TvisitIConstExpr(ctx); };
    std::any visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx) override { return TvisitArrayAccessExpr(ctx); };
    std::any visitSConstExpr(WPLParser::SConstExprContext *ctx) override { return TvisitSConstExpr(ctx); };
    std::any visitUnaryExpr(WPLParser::UnaryExprContext *ctx) override { return TvisitUnaryExpr(ctx); };
    std::any visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx) override { return TvisitBinaryArithExpr(ctx); };
    std::any visitEqExpr(WPLParser::EqExprContext *ctx) override { return TvisitEqExpr(ctx); };
    std::any visitLogAndExpr(WPLParser::LogAndExprContext *ctx) override { return TvisitLogAndExpr(ctx); };
    std::any visitLogOrExpr(WPLParser::LogOrExprContext *ctx) override { return TvisitLogOrExpr(ctx); };
    std::any visitCallExpr(WPLParser::CallExprContext *ctx) override { return TvisitCallExpr(ctx); };
    std::any visitVariableExpr(WPLParser::VariableExprContext *ctx) override { return TvisitVariableExpr(ctx); };
    std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx) override { return TvisitFieldAccessExpr(ctx); };
    std::any visitParenExpr(WPLParser::ParenExprContext *ctx) override { return TvisitParenExpr(ctx); };
    std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx) override { return TvisitBinaryRelExpr(ctx); };
    std::any visitBConstExpr(WPLParser::BConstExprContext *ctx) override { return TvisitBConstExpr(ctx); };
    std::any visitBlock(WPLParser::BlockContext *ctx) override { return TvisitBlock(ctx); };
    std::any visitCondition(WPLParser::ConditionContext *ctx) override { return TvisitCondition(ctx); };
    std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override { return TvisitSelectAlternative(ctx); };
    std::any visitParameterList(WPLParser::ParameterListContext *ctx) override { return TvisitParameterList(ctx); };
    std::any visitParameter(WPLParser::ParameterContext *ctx) override { return TvisitParameter(ctx); };
    std::any visitAssignment(WPLParser::AssignmentContext *ctx) override { return TvisitAssignment(ctx); };
    std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override { return TvisitExternStatement(ctx); };
    std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override { return TvisitFuncDef(ctx); };
    std::any visitProcDef(WPLParser::ProcDefContext *ctx) override { return TvisitProcDef(ctx); };
    std::any visitAssignStatement(WPLParser::AssignStatementContext *ctx) override { return TvisitAssignStatement(ctx); };
    std::any visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx) override { return TvisitVarDeclStatement(ctx); };
    std::any visitLoopStatement(WPLParser::LoopStatementContext *ctx) override { return TvisitLoopStatement(ctx); };
    std::any visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx) override { return TvisitConditionalStatement(ctx); };
    std::any visitSelectStatement(WPLParser::SelectStatementContext *ctx) override { return TvisitSelectStatement(ctx); };
    std::any visitCallStatement(WPLParser::CallStatementContext *ctx) override { return TvisitCallStatement(ctx); };
    std::any visitReturnStatement(WPLParser::ReturnStatementContext *ctx) override { return TvisitReturnStatement(ctx); };
    std::any visitBlockStatement(WPLParser::BlockStatementContext *ctx) override { return TvisitBlockStatement(ctx); };
    std::any visitTypeOrVar(WPLParser::TypeOrVarContext *ctx) override { return TvisitTypeOrVar(ctx); };
    std::any visitType(WPLParser::TypeContext *ctx) override { return TvisitType(ctx); };
    std::any visitBooleanConst(WPLParser::BooleanConstContext *ctx) override { return TvisitBooleanConst(ctx); };

    bool hasErrors(int flags) { return errorHandler.hasErrors(flags); }
    std::string getErrors() { return errorHandler.errorList(); }

    PropertyManager *getProperties() { return props; }

    Module *getModule() { return module; }
    void modPrint() { module->print(llvm::outs(), nullptr); }

    // From C++ Documentation for visitors
    template <class... Ts>
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };
    template <class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    std::optional<Value *> visitInvokeable(std::variant<WPLParser::ProcDefContext *, WPLParser::FuncDefContext *> sum)
    {
        std::vector<llvm::Type *> typeVec;

        // From C++ Documentation for visitors
        antlr4::ParserRuleContext *ctx = std::visit(overloaded{[](WPLParser::ProcDefContext *arg)
                                                               { return (antlr4::ParserRuleContext *)arg; },
                                                               [](WPLParser::FuncDefContext *arg)
                                                               { return (antlr4::ParserRuleContext *)arg; }},
                                                    sum);

        Symbol *sym = props->getBinding(ctx);

        std::string funcId = std::visit(overloaded{[](WPLParser::ProcDefContext *arg)
                                                   { return arg->name->getText(); },
                                                   [](WPLParser::FuncDefContext *arg)
                                                   { return arg->name->getText(); }},
                                        sum);

        if (!sym)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Unbound function: " + funcId);
            return {};
        }

        WPLParser::ParameterListContext *paramList = std::visit(overloaded{[](WPLParser::ProcDefContext *arg)
                                                                           { return arg->paramList; },
                                                                           [](WPLParser::FuncDefContext *arg)
                                                                           { return arg->paramList; }},
                                                                sum);

        if (paramList)
        {
            for (auto e : paramList->params)
            {
                std::optional<llvm::Type *> type = CodegenVisitor::llvmTypeFor(e->ty);

                if(!type)
                {
                    errorHandler.addCodegenError(ctx->getStart(), "Could not generate code for type: " + e->ty->toString());
                    return {};
                }

                typeVec.push_back(type.value());
            }
        }

        ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);

        // Can't use visitor because visitor can't see `this'
        std::optional<llvm::Type *>retType = std::holds_alternative<WPLParser::FuncDefContext *>(sum) ? CodegenVisitor::llvmTypeFor(std::get<WPLParser::FuncDefContext *>(sum)->ty)
                                                                                       : VoidTy;

        if(!retType)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Could not generate code for return type of " + funcId);
            return {};
        }

        FunctionType *fnType = FunctionType::get(
            retType.value(),
            paramRef,
            false);

        Function *fn = Function::Create(fnType, GlobalValue::ExternalLinkage, funcId, module);

        // Create basic block
        BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", fn);
        builder->SetInsertPoint(bBlk);

        for (auto &arg : fn->args())
        {
            int argNumber = arg.getArgNo();
            llvm::Type *type = typeVec.at(argNumber);

            // FIXME: not convinced this will work with arrays--not that WPL requires that... seems suspicious like it'd include the type here??
            std::string argName = paramList->params.at(argNumber)->getText();

            llvm::AllocaInst *v = builder->CreateAlloca(type, 0, argName);
            Symbol *sym = props->getBinding(paramList->params.at(argNumber));

            if (!sym)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Unable to generate parameter for function: " + argName);
            }
            else
            {
                sym->val = v;

                builder->CreateStore(&arg, v);
            }
        }

        WPLParser::BlockContext* block = std::visit(overloaded{[](WPLParser::ProcDefContext *arg)
                                                   { return arg->block(); },
                                                   [](WPLParser::FuncDefContext *arg)
                                                   { return arg->block(); }},
                                        sum);


        for(auto e : block->stmts)
        {
            e->accept(this);
        }

        if(std::holds_alternative<WPLParser::ProcDefContext *>(sum) && !CodegenVisitor::blockEndsInReturn(block)) {
            builder->CreateRetVoid();
        }

        return {};
    }

protected:
    static bool blockEndsInReturn(WPLParser::BlockContext *ctx)
    {
        return ctx->stmts.size() > 0 && dynamic_cast<WPLParser::ReturnStatementContext *>(ctx->stmts.at(ctx->stmts.size() - 1));
    }

    std::optional<llvm::Type *>llvmTypeFor(WPLParser::TypeContext *ctx)
    {
        llvm::Type *ty;
        bool valid = false;

        if (ctx->TYPE_INT())
        {
            ty = Int32Ty;
            valid = true;
        }
        else if (ctx->TYPE_BOOL())
        {
            ty = Int1Ty;
            valid = true;
        }
        else if (ctx->TYPE_STR())
        {
            ty = i8p;
            valid = true;
        }

        if (!valid)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Unknown type: " + ctx->getText());
            return {};
        }

        if (ctx->len)
        {
            // Semantic analysis ensures this is positive.
            uint64_t len = (uint64_t)std::stoi(ctx->len->getText());
            llvm::Type *arr = ArrayType::get(ty, len);

            return arr;
        }

        return ty;
    }

private:
    PropertyManager *props;
    int flags; 

    WPLErrorHandler errorHandler;

    // LLVM
    LLVMContext *context;
    Module *module;
    IRBuilder<NoFolder> *builder;

    // Commonly used types
    llvm::Type *VoidTy;
    llvm::Type *Int1Ty;
    llvm::Type *Int8Ty;
    llvm::Type *Int32Ty; // Things like 32 bit integers
    llvm::Type *i8p;
    llvm::Type *Int8PtrPtrTy;
    Constant *Int32Zero;
    Constant *Int32One;
};
