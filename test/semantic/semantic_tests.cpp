#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

TEST_CASE("Development tests", "[semantic]") {
  antlr4::ANTLRInputStream input("42;");
  WPLLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
  WPLParser::CompilationUnitContext* tree = NULL;
  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL);
  SemanticVisitor* sv = new SemanticVisitor(new STManager(), new PropertyManager());  // NEW
  sv->visitCompilationUnit(tree); // NEW
  // Error checking is NEW
  // if (sv->hasErrors()) {
  //   CHECK("foo" == sv->getErrors());
  // }
  CHECK_FALSE(sv->hasErrors());
}