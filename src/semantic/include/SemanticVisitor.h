#pragma once
#include "antlr4-runtime.h"
#include "WPLBaseVisitor.h"
#include "STManager.h"
#include "PropertyManager.h"
#include "WPLErrorHandler.h"

#include "TypeVisitor.h"

class SemanticVisitor : WPLBaseVisitor
{

public:
    SemanticVisitor(STManager *s, PropertyManager *p)
    {
        stmgr = s;
        bindings = p;
    }

    std::string getErrors() { return errorHandler.errorList(); }
    STManager *getSTManager() { return stmgr; }
    PropertyManager *getBindings() { return bindings; }
    bool hasErrors() { return errorHandler.hasErrors(); }

    const Type *visitCtx(WPLParser::CompilationUnitContext *ctx);
    const Type *visitCtx(WPLParser::InvocationContext *ctx);
    const Type *visitCtx(WPLParser::ArrayAccessContext *ctx);
    const Type *visitCtx(WPLParser::ArrayOrVarContext *ctx);
    const Type *visitCtx(WPLParser::IConstExprContext *ctx);
    const Type *visitCtx(WPLParser::ArrayAccessExprContext *ctx);
    const Type *visitCtx(WPLParser::SConstExprContext *ctx);
    const Type *visitCtx(WPLParser::UnaryExprContext *ctx);
    const Type *visitCtx(WPLParser::BinaryArithExprContext *ctx);
    const Type *visitCtx(WPLParser::EqExprContext *ctx);
    const Type *visitCtx(WPLParser::LogAndExprContext *ctx);
    const Type *visitCtx(WPLParser::LogOrExprContext *ctx);
    const Type *visitCtx(WPLParser::CallExprContext *ctx);
    const Type *visitCtx(WPLParser::VariableExprContext *ctx);
    const Type *visitCtx(WPLParser::FieldAccessExprContext *ctx);
    const Type *visitCtx(WPLParser::ParenExprContext *ctx);
    const Type *visitCtx(WPLParser::BinaryRelExprContext *ctx);
    const Type *visitCtx(WPLParser::BConstExprContext *ctx);
    const Type *visitCtx(WPLParser::BlockContext *ctx);
    const Type *visitCtx(WPLParser::ConditionContext *ctx);
    const Type *visitCtx(WPLParser::SelectAlternativeContext *ctx);
    const Type *visitCtx(WPLParser::ParameterListContext *ctx);
    const Type *visitCtx(WPLParser::ParameterContext *ctx);
        const Type * visitCtx(WPLParser::AssignmentContext *ctx) ;
    const Type *visitCtx(WPLParser::ExternStatementContext *ctx);
    const Type *visitCtx(WPLParser::FuncDefContext *ctx);
    const Type *visitCtx(WPLParser::ProcDefContext *ctx);
    const Type *visitCtx(WPLParser::AssignStatementContext *ctx);
    const Type *visitCtx(WPLParser::VarDeclStatementContext *ctx);
    const Type *visitCtx(WPLParser::LoopStatementContext *ctx);
    const Type *visitCtx(WPLParser::ConditionalStatementContext *ctx);
    const Type *visitCtx(WPLParser::SelectStatementContext *ctx);
    const Type *visitCtx(WPLParser::CallStatementContext *ctx);
    const Type *visitCtx(WPLParser::ReturnStatementContext *ctx);
    const Type *visitCtx(WPLParser::BlockStatementContext *ctx);
    const Type *visitCtx(WPLParser::TypeOrVarContext *ctx);
    const Type *visitCtx(WPLParser::TypeContext *ctx);
    const Type *visitCtx(WPLParser::BooleanConstContext *ctx);


    std::any visitCompilationUnit(WPLParser::CompilationUnitContext *ctx) override { return visitCtx(ctx); }
    std::any visitInvocation(WPLParser::InvocationContext *ctx) override { return visitCtx(ctx); }
    std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx) override { return visitCtx(ctx); }
    std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx) override { return visitCtx(ctx); }
    std::any visitIConstExpr(WPLParser::IConstExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitSConstExpr(WPLParser::SConstExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitUnaryExpr(WPLParser::UnaryExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitEqExpr(WPLParser::EqExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitLogAndExpr(WPLParser::LogAndExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitLogOrExpr(WPLParser::LogOrExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitCallExpr(WPLParser::CallExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitVariableExpr(WPLParser::VariableExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitParenExpr(WPLParser::ParenExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBConstExpr(WPLParser::BConstExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBlock(WPLParser::BlockContext *ctx) override { return visitCtx(ctx); }
    std::any visitCondition(WPLParser::ConditionContext *ctx) override { return visitCtx(ctx); }
    std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override { return visitCtx(ctx); }
    std::any visitParameterList(WPLParser::ParameterListContext *ctx) override { return visitCtx(ctx); }
    std::any visitParameter(WPLParser::ParameterContext *ctx) override { return visitCtx(ctx); }
    std::any visitAssignment(WPLParser::AssignmentContext *ctx) override { return visitCtx(ctx); }
    std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override { return visitCtx(ctx); }
    std::any visitProcDef(WPLParser::ProcDefContext *ctx) override { return visitCtx(ctx); }
    std::any visitAssignStatement(WPLParser::AssignStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitLoopStatement(WPLParser::LoopStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitSelectStatement(WPLParser::SelectStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitCallStatement(WPLParser::CallStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitReturnStatement(WPLParser::ReturnStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitBlockStatement(WPLParser::BlockStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitTypeOrVar(WPLParser::TypeOrVarContext *ctx) override { return visitCtx(ctx); }
    std::any visitType(WPLParser::TypeContext *ctx) override { return visitCtx(ctx); }
    std::any visitBooleanConst(WPLParser::BooleanConstContext *ctx) override { return visitCtx(ctx); }

private:
    STManager *stmgr;
    PropertyManager *bindings;
    WPLErrorHandler errorHandler;
};