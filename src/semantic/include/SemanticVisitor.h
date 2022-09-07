/**
 * @file SemanticVisitor.h
 * @author your name (you@domain.com)
 * @brief Interface for the semantic visitor
 * @version 0.1
 * @date 2022-07-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "antlr4-runtime.h"
#include "CalculatorBaseVisitor.h"
#include "STManager.h"
#include "PropertyManager.h"
#include "CalcErrorHandler.h"

class SemanticVisitor : CalculatorBaseVisitor {
  public :
    // Pass in the appropriate elements
    SemanticVisitor(STManager* stm, PropertyManager* pm) {
      stmgr = stm;
      bindings = pm;
    }

    std::any visitProgram(CalculatorParser::ProgramContext *ctx) override;
    std::any visitBooleanConstant(CalculatorParser::BooleanConstantContext *ctx) override;
    std::any visitIConstExpr(CalculatorParser::IConstExprContext *ctx) override;
    std::any visitParenExpr(CalculatorParser::ParenExprContext *ctx) override;
    std::any visitUnaryMinusExpr(CalculatorParser::UnaryMinusExprContext *ctx) override;
    std::any visitUnaryNotExpr(CalculatorParser::UnaryNotExprContext *ctx) override;
    std::any visitBinaryArithExpr(CalculatorParser::BinaryArithExprContext *ctx) override;
    std::any visitBinaryRelExpr(CalculatorParser::BinaryRelExprContext *ctx) override;
    std::any visitEqExpr(CalculatorParser::EqExprContext *ctx) override;
    std::any visitAssignExpression(CalculatorParser::AssignExpressionContext *ctx) override;
    std::any visitVariableExpr(CalculatorParser::VariableExprContext *ctx) override;

    std::string getErrors() { return errors.errorList(); }
    STManager* getSTManager() { return stmgr; }
    PropertyManager* getBindings() { return bindings; }
    bool hasErrors() { return errors.hasErrors(); }

  private: 
    STManager* stmgr;
    PropertyManager* bindings; 
    CalcErrorHandler errors;
};