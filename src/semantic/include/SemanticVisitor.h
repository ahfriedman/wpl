#pragma once
#include "antlr4-runtime.h"
#include "WPLBaseVisitor.h"
#include "STManager.h"
#include "PropertyManager.h"
#include "WPLErrorHandler.h"

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
    //     std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx) override;
    std::any visitParenExpr(WPLParser::ParenExprContext *ctx) override;
    std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx) override;
    std::any visitBConstExpr(WPLParser::BConstExprContext *ctx) override;
    std::any visitBlock(WPLParser::BlockContext *ctx) override;
    std::any visitCondition(WPLParser::ConditionContext *ctx) override;
    std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override;
    std::any visitParameterList(WPLParser::ParameterListContext *ctx) override;
    std::any visitParameter(WPLParser::ParameterContext *ctx) override;
    //     std::any visitAssignment(WPLParser::AssignmentContext *ctx) override;
    //     std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override;
    //     std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override;
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

private:
    STManager *stmgr;
    PropertyManager *bindings;
    WPLErrorHandler errorHandler;
};