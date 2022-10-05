#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

#include "test_error_handlers.h"

TEST_CASE("Sample Progam one", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      "(* \n"
      " * We know that n is odd, so just start with 3\n"
      " *)\n"
      "boolean func isPrime(int n) {\n"
      " int i <- 3;\n"
      " while (i < n) do { \n"
      "   if (n / i * i = n) then { return false; } \n"
      "   i <- i + 2;\n"
      " }\n"
      " return true;\n"
      "}\n"
      "\n"
      "int func program() {\n"
      " int current <- 3;       \n" 
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);

  std::cout << stmgr->toString() << std::endl;
  std::cout << sv->getErrors() << std::endl; 

  CHECK_FALSE(sv->hasErrors(0));
}

TEST_CASE("Sample Progam one w/ Inf", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      "(* \n"
      " * We know that n is odd, so just start with 3\n"
      " *)\n"
      "boolean func isPrime(int n) {\n"
      " var i <- 3;\n"
      " while (i < n) do { \n"
      "   if (n / i * i = n) then { return false; } \n"
      "   i <- i + 2;\n"
      " }\n"
      " return true;\n"
      "}\n"
      "\n"
      "int func program() {\n"
      " var current <- 3;       \n" 
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);

  std::cout << stmgr->toString() << std::endl;
  std::cout << sv->getErrors() << std::endl; 

  CHECK_FALSE(sv->hasErrors(0));
}

TEST_CASE("Block", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      "boolean func isPrime(int n) {\n"
      " var i <- 3;\n"
      " while (i < n) do { \n"
      "   if (n / i * i = n) then { return false; } \n"
      "   i <- i + 2;\n"
      "   {\n"
      "     int i <- 0;\n" //FIXME: TEST THIS CASE IN WPLC
      "   }\n"
      " }\n"
      " return true;\n"
      "}\n"
      "\n"
      "int func program() {\n"
      " var current <- 3;       \n" 
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);

  std::cout << stmgr->toString() << std::endl;
  std::cout << sv->getErrors() << std::endl; 

  CHECK_FALSE(sv->hasErrors(0));
}

TEST_CASE("programs/test4 - Don't allow void to be sent to fn", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test4.wpl");
    antlr4::ANTLRInputStream * input = new antlr4::ANTLRInputStream(*inStream);

    WPLLexer lexer(input);
    antlr4::CommonTokenStream tokens(&lexer);
    WPLParser parser(&tokens);
    parser.removeErrorListeners();
    WPLParser::CompilationUnitContext *tree = NULL;
    REQUIRE_NOTHROW(tree = parser.compilationUnit());
    REQUIRE(tree != NULL);
    STManager *stm = new STManager();
    PropertyManager *pm = new PropertyManager();
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test9Err - Test assign var to array", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9Err.wpl");
    antlr4::ANTLRInputStream * input = new antlr4::ANTLRInputStream(*inStream);

    WPLLexer lexer(input);
    antlr4::CommonTokenStream tokens(&lexer);
    WPLParser parser(&tokens);
    parser.removeErrorListeners();
    WPLParser::CompilationUnitContext *tree = NULL;
    REQUIRE_NOTHROW(tree = parser.compilationUnit());
    REQUIRE(tree != NULL);
    STManager *stm = new STManager();
    PropertyManager *pm = new PropertyManager();
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test11Err - Prevent global exprs", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test11err.wpl");
    antlr4::ANTLRInputStream * input = new antlr4::ANTLRInputStream(*inStream);

    WPLLexer lexer(input);
    antlr4::CommonTokenStream tokens(&lexer);
    WPLParser parser(&tokens);
    parser.removeErrorListeners();
    WPLParser::CompilationUnitContext *tree = NULL;
    REQUIRE_NOTHROW(tree = parser.compilationUnit());
    REQUIRE(tree != NULL);
    STManager *stm = new STManager();
    PropertyManager *pm = new PropertyManager();
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE(sv->hasErrors(0));
}