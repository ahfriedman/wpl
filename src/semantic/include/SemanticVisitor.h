#pragma once
#include "antlr4-runtime.h"
#include "WPLBaseVisitor.h"
#include "STManager.h"

class SemanticVisitor : WPLBaseVisitor
{

// public:
//     std::any visitCompilationUnit(WPLParser::CompilationUnitContext *ctx);

//     std::any visitInvocation(WPLParser::InvocationContext *ctx);

//     std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx);

//     std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx);

//     std::any visitIConstExpr(WPLParser::IConstExprContext *ctx);

//     std::any visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx);

//     std::any visitSConstExpr(WPLParser::SConstExprContext *ctx);

//     std::any visitUnaryExpr(WPLParser::UnaryExprContext *ctx);

//     std::any visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx);

//     std::any visitEqExpr(WPLParser::EqExprContext *ctx);

//     std::any visitLogAndExpr(WPLParser::LogAndExprContext *ctx);

//     std::any visitLogOrExpr(WPLParser::LogOrExprContext *ctx);

//     std::any visitCallExpr(WPLParser::CallExprContext *ctx);

//     std::any visitVariableExpr(WPLParser::VariableExprContext *ctx);

//     std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx);

//     std::any visitParenExpr(WPLParser::ParenExprContext *ctx);

//     std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx);

//     std::any visitBConstExpr(WPLParser::BConstExprContext *ctx);

//     std::any visitBlock(WPLParser::BlockContext *ctx);

//     std::any visitCondition(WPLParser::ConditionContext *ctx);

//     std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx);

//     std::any visitParameterList(WPLParser::ParameterListContext *ctx);

//     std::any visitParameter(WPLParser::ParameterContext *ctx);

//     std::any visitAssignment(WPLParser::AssignmentContext *ctx);

//     std::any visitExternStatement(WPLParser::ExternStatementContext *ctx);

//     std::any visitFuncDef(WPLParser::FuncDefContext *ctx);

//     std::any visitProcDef(WPLParser::ProcDefContext *ctx);

//     std::any visitAssignStatement(WPLParser::AssignStatementContext *ctx);

//     std::any visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx);

//     std::any visitLoopStatement(WPLParser::LoopStatementContext *ctx);

//     std::any visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx);

//     std::any visitSelectStatement(WPLParser::SelectStatementContext *ctx);

//     std::any visitCallStatement(WPLParser::CallStatementContext *ctx);

//     std::any visitReturnStatement(WPLParser::ReturnStatementContext *ctx);

//     std::any visitBlockStatement(WPLParser::BlockStatementContext *ctx);

//     std::any visitTypeOrVar(WPLParser::TypeOrVarContext *ctx);

//     std::any visitType(WPLParser::TypeContext *ctx);

//     std::any visitBooleanConst(WPLParser::BooleanConstContext *ctx);
};