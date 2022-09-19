#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

#include "test_error_handlers.h"

// FIXME: REQUIRE END IN RETURN

TEST_CASE("Sample Progam one", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      "(* \n"
      " * We know that n is odd, so just start with 3\n"
      " *)\n"
      "boolean func isPrime(int n) {\n"
      " int i <- 3;\n" //FIXME: MOVE BACK TO TYPE INFERENCE
      " while (i < n) do { \n"
      "   if (n / i * i = n) then { return false; } \n"
      "   i <- i + 2;\n"
      " }\n"
      " return true;\n"
      "}\n"
      "\n"
      "int func program() {\n"
      " int current <- 3;       \n" // FIXME SWITCH TO TYPE INFERENCE
      " int nPrimes <- 2;       # explicit type \n"
      " while current < 100 do { \n"
      "   if isPrime(current) then { nPrimes <- nPrimes + 1;}\n"
      "   current <- current + 2;\n"
      " }\n"
      " return nPrimes;\n"
      "}");
  WPLLexer lexer(&input);
  // lexer.removeErrorListeners();
  // lexer.addErrorListener(new TestErrorListener());
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new TestErrorListener());

  WPLParser::CompilationUnitContext *tree = NULL;
  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL);

  // Any errors should be syntax errors.
  // FIXME: Should probably confirm the above statement through testing for syntax errors
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);

  std::cout << stmgr->toString() << std::endl;
  std::cout << sv->getErrors() << std::endl; 

  CHECK_FALSE(sv->hasErrors());
}