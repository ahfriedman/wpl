#include "SemanticVisitor.h"

std::any SemanticVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    // Enter initial scope
    stmgr->enterScope();

    for (auto e : ctx->stmts)
    {
        e->accept(this);
    }

    return SymbolType::UNDEFINED;
}
//     std::any visitInvocation(WPLParser::InvocationContext *ctx) override;
//     std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx) override;
//     std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx) override;
std::any SemanticVisitor::visitIConstExpr(WPLParser::IConstExprContext *ctx)
{
    return SymbolType::INT;
}
//     std::any visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx) override;
std::any SemanticVisitor::visitSConstExpr(WPLParser::SConstExprContext *ctx)
{
    return SymbolType::STR;
}
//     std::any visitUnaryExpr(WPLParser::UnaryExprContext *ctx) override;
//     std::any visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx) override;
//     std::any visitEqExpr(WPLParser::EqExprContext *ctx) override;
//     std::any visitLogAndExpr(WPLParser::LogAndExprContext *ctx) override;
//     std::any visitLogOrExpr(WPLParser::LogOrExprContext *ctx) override;
//     std::any visitCallExpr(WPLParser::CallExprContext *ctx) override;
//     std::any visitVariableExpr(WPLParser::VariableExprContext *ctx) override;
//     std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx) override;
std::any SemanticVisitor::visitParenExpr(WPLParser::ParenExprContext *ctx)
{
    return ctx->ex->accept(this);
}
//     std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx) override;

std::any SemanticVisitor::visitBConstExpr(WPLParser::BConstExprContext *ctx)
{
    return SymbolType::BOOL;
}

std::any SemanticVisitor::visitBlock(WPLParser::BlockContext *ctx)
{
    // FIXME: Probably have to do better with things like type inference!!!
    stmgr->enterScope();

    for (auto e : ctx->stmts)
    {
        e->accept(this);
    }

    stmgr->exitScope();

    return SymbolType::UNDEFINED;
}
//     std::any visitCondition(WPLParser::ConditionContext *ctx) override;
//     std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override;
//     std::any visitParameterList(WPLParser::ParameterListContext *ctx) override;
//     std::any visitParameter(WPLParser::ParameterContext *ctx) override;
//     std::any visitAssignment(WPLParser::AssignmentContext *ctx) override;
//     std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override;
//     std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override;
//     std::any visitProcDef(WPLParser::ProcDefContext *ctx) override;
//     std::any visitAssignStatement(WPLParser::AssignStatementContext *ctx) override;
//     std::any visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx) override;
//     std::any visitLoopStatement(WPLParser::LoopStatementContext *ctx) override;
//     std::any visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx) override;
//     std::any visitSelectStatement(WPLParser::SelectStatementContext *ctx) override;
//     std::any visitCallStatement(WPLParser::CallStatementContext *ctx) override;
//     std::any visitReturnStatement(WPLParser::ReturnStatementContext *ctx) override;

std::any SemanticVisitor::visitBlockStatement(WPLParser::BlockStatementContext *ctx)
{
    return ctx->block()->accept(this);
}
//     std::any visitTypeOrVar(WPLParser::TypeOrVarContext *ctx) override;
//     std::any visitType(WPLParser::TypeContext *ctx) override;

std::any SemanticVisitor::visitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    return SymbolType::BOOL;
}