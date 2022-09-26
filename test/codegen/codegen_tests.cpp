/**
 * @file codegen_tests.cpp
 * @author gpollice
 * @brief Basic tests for code generation
 * @version 0.1
 * @date 2022-08-05
 */
#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"
#include "CodegenVisitor.h"

TEST_CASE("Development tests", "[codegen]") {
  antlr4::ANTLRInputStream input("int func program() {return -1; }");
  WPLLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
  WPLParser::CompilationUnitContext* tree = NULL;
  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL);
  STManager *stm = new STManager();
  PropertyManager *pm = new PropertyManager();
  SemanticVisitor* sv = new SemanticVisitor(stm, pm);
  sv->visitCompilationUnit(tree);
  CodegenVisitor* cv = new CodegenVisitor(pm, "test");
  cv->visitCompilationUnit(tree);
  if (cv->hasErrors()) {
    CHECK("foo" == cv->getErrors());
  }
  CHECK_FALSE(cv->hasErrors());
}