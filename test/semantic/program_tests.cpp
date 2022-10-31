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
      "     int i <- 0;\n"
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

TEST_CASE("programs/test4 - Don't allow void to be sent to fn", "[semantic]")
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

TEST_CASE("programs/test9Err - Test assign var to array", "[semantic]")
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

TEST_CASE("programs/test11Err - Prevent global exprs", "[semantic]")
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

TEST_CASE("programs/doubleArg1 - Prevent Argument reuse in func", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/doubleArg1.wpl");
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

TEST_CASE("programs/doubleArg2 - Prevent Argument reuse in extern", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/doubleArg2.wpl");
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

TEST_CASE("programs/doubleArg3 - Prevent Argument reuse in func and that we don't crash", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/doubleArg3.wpl");
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

TEST_CASE("programs/test15 - No array equalities", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test15.wpl");
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


TEST_CASE("Comment EOF", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      "# Hello # there!");
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

TEST_CASE("programs/test16 - overwrite lhs var", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test16a - overwrite lhs var - other way", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16a.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test16c - overwrite rhs var", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16c.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test16c-1 - overwrite rhs var - bubble up!", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16c-1.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test16c-2 - overwrite rhs var", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16c-2.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test16d - chain var", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16d.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test16e - chain var 2", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16e.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("programs/test16f - var loop", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test16f.wpl");
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
    REQUIRE_FALSE(sv->hasErrors(0));
}


TEST_CASE("programs/test17 - var inf in decl", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test17.wpl");
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
    REQUIRE_FALSE(sv->hasErrors(0));
}

TEST_CASE("Test program() should return int warning", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    R""""(
      proc program () {

      }
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  CHECK_FALSE(sv->hasErrors(ERROR));
  CHECK(sv->hasErrors(CRITICAL_WARNING));
}

TEST_CASE("Test program() should not have parameters warning", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    R""""(
      int func program (int a) {
        return 0; 
      }
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  CHECK_FALSE(sv->hasErrors(ERROR));
  CHECK(sv->hasErrors(CRITICAL_WARNING));
}

TEST_CASE("Dead code in program block", "[semantic][program]")
{
  antlr4::ANTLRInputStream input(
    R""""(
    int func program() {

        return 1; 

        int a; 

        return 0;
    }
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  REQUIRE(sv->hasErrors(ERROR));
}

TEST_CASE("Dead code in if/else", "[semantic][program][conditional]")
{
  antlr4::ANTLRInputStream input(
    R""""(
    int func program() {

    if true then {
        return 0; 

        int a; 
    } else {
        return 1;

        int b; 
    }
}
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  REQUIRE(sv->hasErrors(ERROR));
}

TEST_CASE("Dead code in select", "[semantic][program][select]")
{
  antlr4::ANTLRInputStream input(
    R""""(
int func program () {

    select {
        true : {
            return 0;

            int a;
        }

        false : {
            return 1; 

            int b; 
        }
    }

    return 0; 
}
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  REQUIRE(sv->hasErrors(ERROR));
}

TEST_CASE("Infer In return", "[semantic][program]")
{
  antlr4::ANTLRInputStream input(
    R""""(
    int func program() {
        var a; 
        return a;
    }
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  REQUIRE_FALSE(sv->hasErrors(ERROR));
}

TEST_CASE("Uninferred", "[semantic][program]")
{
  antlr4::ANTLRInputStream input(
    R""""(
    var a; 
    int func program() {
        return 0;
    }
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  REQUIRE(sv->hasErrors(ERROR));
}

TEST_CASE("Nested programs (TEMPORARY)", "[semantic][program]")
{
  antlr4::ANTLRInputStream input(
    R""""(
    var a; 
    int func program() {
        int func test() {
            return 0; 
        }

        return 0; 
    }
    )""""
  );
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
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);
  REQUIRE(sv->hasErrors(ERROR));
}

/*********************************
 * C-Level Example tests
 *********************************/
TEST_CASE("C Level Negative Test #1", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/CLevel/CNegative1.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("C Level Negative Test #2", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/CLevel/CNegative2.wpl");
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
    REQUIRE(sv->hasErrors(0));
}


/*********************************
 * B-Level Example tests
 *********************************/
TEST_CASE("B Level Negative Test #1", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/BLevel/BNegative1.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("B Level Negative Test #2", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/BLevel/BNegative2.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

/*********************************
 * A-Level Example tests
 *********************************/
TEST_CASE("A Level Negative Test #1", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/ALevel/ANegative1.wpl");
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
    REQUIRE(sv->hasErrors(0));
}

TEST_CASE("A Level Negative Test #2", "[semantic]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/ALevel/ANegative2.wpl");
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
    REQUIRE(sv->hasErrors(0));
}