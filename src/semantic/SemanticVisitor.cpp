/**
 * @file SemanticVisitor.cpp
 * @author gpollice
 * @brief AST/Parse tree visitor that creates the symbol table
 *  and does the simple type checking for the Calculator application
 * @version 0.1
 * @date 2022-07-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "SemanticVisitor.h"
#include <any>

/**
 * @brief Just visit the expressions below the program
 */
std::any SemanticVisitor::visitProgram (CalculatorParser::ProgramContext *ctx) {
  stmgr->enterScope();    // initial scope (only one for this example)
  for (auto e : ctx->exprs) {
    e->accept(this);
  }
  return SymType::UNDEFINED;
}

/**
 * @brief booleanConstant.type = BOOL
 */
std::any SemanticVisitor::visitBooleanConstant(CalculatorParser::BooleanConstantContext *ctx) {
  return SymType::BOOL;
}

/**
 * @brief IConstExpr.type = INT
 */
std::any SemanticVisitor::visitIConstExpr(CalculatorParser::IConstExprContext *ctx) {
  return SymType::INT;
}

/**
 * @brief ParenExpr.type = ex.type
 */
std::any SemanticVisitor::visitParenExpr(CalculatorParser::ParenExprContext *ctx) {
  return ctx->ex->accept(this);
}

/**
 * @brief UnaryMinusExpr.type = ex.type && ex.type == INT;
 * @return SymTyp::INT if no error, SymType::UNDEFINED if an error.
 */
std::any SemanticVisitor::visitUnaryMinusExpr(CalculatorParser::UnaryMinusExprContext *ctx) {
  auto t = std::any_cast<SymType>(ctx -> ex ->accept(this));
  if (t != SymType::INT){ // Type mismatch
    errors.addSemanticError(ctx->getStart(), "INT expression expected, but was " + Symbol::getSymTypeName(t));
    t = SymType::UNDEFINED;
  }
  return t;
}

std::any SemanticVisitor::visitUnaryNotExpr(CalculatorParser::UnaryNotExprContext *ctx) {
  auto t = std::any_cast<SymType>(ctx -> ex ->accept(this));
  if (t != SymType::BOOL){ // Type mismatch
    errors.addSemanticError(ctx->getStart(), "BOOL expression expected, but was " + Symbol::getSymTypeName(t));
  }
  return t;
}

/**
 * @brief BinaryArithExpr.type = INT && left.type == INT && right.type == INT
 * 
 * @return SymType::INT if there are no errors or SymType::UNDEFINED if there are errors.  
 */
std::any SemanticVisitor::visitBinaryArithExpr(CalculatorParser::BinaryArithExprContext *ctx) {
  SymType type = INT;
  auto left = std::any_cast<SymType>(ctx -> left ->accept(this));
  if (left != SymType::INT){ // Type mismatch
    errors.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + Symbol::getSymTypeName(left));
    type = SymType::UNDEFINED;;
  }
  auto right = std::any_cast<SymType>(ctx -> right ->accept(this));
  if (right != SymType::INT){ // Type mismatch
    errors.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + Symbol::getSymTypeName(right));
    type = SymType::UNDEFINED;
  }
  return type;
}

/**
 * @brief BinaryRelExpr.type = BOOL && left.type == INT && right.type == INT
 * 
 * @return SymType::BOOL if there are no errors or SymType::UNDEFINED if there are errors. 
 */
std::any SemanticVisitor::visitBinaryRelExpr(CalculatorParser::BinaryRelExprContext *ctx) {
  SymType type = BOOL;
  auto left = std::any_cast<SymType>(ctx -> left ->accept(this));
  if (left != SymType::INT){ // Type mismatch
    errors.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " +        Symbol::getSymTypeName(left));
    type = SymType::UNDEFINED;
  }
  auto right = std::any_cast<SymType>(ctx -> right ->accept(this));
  if (right != SymType::INT){ // Type mismatch
    errors.addSemanticError(ctx->getStart(), "BOOL right expression expected, but was " + Symbol::getSymTypeName(right));
    type = SymType::UNDEFINED;
  }
  return type;
}

/**
 * @brief BinaryRelExpr.type = SymType::BOOL && left.type == right.type
 * 
 * @return SymType::BOOL if there are no errors or SymType::UNDEFINED if there are errors.
 */
std::any SemanticVisitor::visitEqExpr(CalculatorParser::EqExprContext *ctx) {
  SymType result = SymType::BOOL;
  result = std::any_cast<SymType>(ctx -> right ->accept(this));
  auto left = std::any_cast<SymType>(ctx -> left ->accept(this));
  if (result != left) {
    errors.addSemanticError(ctx->getStart(), "Both sides of '=' must have the same type");
    result = SymType::UNDEFINED;
  }
  return result;
}

/**
 * @brief assignExpression.defined = TRUE 
 *        assignExpression.type = ex.type
 * 
 * @return the type of the expression to which the variable is assigned 
 */
std::any SemanticVisitor::visitAssignExpression(CalculatorParser::AssignExpressionContext *ctx) {
  // Visit the expression
  SymType result = std::any_cast<SymType>(ctx->ex->accept(this));
  if (result == UNDEFINED) {
    errors.addSemanticError(ctx->getStart(), "Expression evaluates to an UNDEFINED type");
  }
  std::string varId = ctx->v->getText(); 
  Symbol *symbol = stmgr->findSymbol(varId);
  if (symbol == nullptr) {
    // Undefined: add it
    symbol = stmgr->addSymbol(varId, result);
  } else {
    symbol->type = result;
  }
  // For the Calculator, we don't have to check the type of the variable since
  // it would be redefined here.
  bindings->bind(ctx, symbol);
  return result;
}

/**
 * @brief v.defined == TRUE && VariableExpr.type = v.type
 * 
 * @return The type of the variable as found in the symbol table or UNDEFINED if
 *  it is not defined in the symbol table.
 */
std::any SemanticVisitor::visitVariableExpr(CalculatorParser::VariableExprContext *ctx) {
  SymType result = UNDEFINED;
  std::string varId = ctx->v->getText(); 
  Symbol *symbol = stmgr->findSymbol(varId);
  if (symbol == nullptr) {
    // Undefined: error
    errors.addSemanticError(ctx->getStart(), "Undefined variable in expression: " + varId);
  } else {
    // bind the symbol to this node
    bindings->bind(ctx, symbol);
    result = symbol->type;
  }
  return result;
}