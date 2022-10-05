#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

#include "test_error_handlers.h"

TEST_CASE("Basic if", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    "if true {"  
    "}"
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

  //FIXME: CHECK MORE PRECISELY 
  CHECK_FALSE(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("Basic if 2", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    "if ~true {"  
    "} else {"
    "}"
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

  //FIXME: CHECK MORE PRECISELY 
  CHECK_FALSE(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("If Example 1", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    "if a < b {"
    " isNegative <- true;"  
    "} else {"
    " isNegative <- false;"
    "}"
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

  //FIXME: CHECK MORE PRECISELY 
  CHECK(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("If Example 1-2", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    "int a, b <- 0;"
    "if a < b {"
    " isNegative <- true;"  
    "} else {"
    " isNegative <- false;"
    "}"
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

  //FIXME: CHECK MORE PRECISELY 
  CHECK(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("If Example 1-3", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    "int a, b <- 0;"
    "int isNegative;"
    "if a < b {"
    " isNegative <- true;"  
    "} else {"
    " isNegative <- false;"
    "}"
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

  //FIXME: CHECK MORE PRECISELY 
  CHECK(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("If Example 1-4", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    "int a, b <- 0;"
    "int isNegative <- 1;"
    "if a < b {"
    " isNegative <- true;"  
    "} else {"
    " isNegative <- false;"
    "}"
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

  //FIXME: CHECK MORE PRECISELY 
  CHECK(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("If Example 1-5", "[semantic][conditional]")
{
  antlr4::ANTLRInputStream input(
    "int a, b <- 0;"
    "boolean isNegative;"
    "if a < b {"
    " isNegative <- true;"  
    "} else {"
    " isNegative <- false;"
    "}"
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

  //FIXME: CHECK MORE PRECISELY 
  CHECK_FALSE(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}


TEST_CASE("Inference If Errors - 1", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      R""""(
      proc infTest() {
        var a; 
        
        if(1 < 2) then {
          a <- true; 
        } else {
          a <- 10; 
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

  // Any errors should be syntax errors.
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);

  // std::cout << stmgr->toString() << std::endl;
  // std::cout << sv->getErrors() << std::endl; 

  CHECK(sv->hasErrors(0));
}

TEST_CASE("Inference If - 1", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      R""""(
      proc infTest() {
        var a; 
        
        if(1 < 2) then {
          var a <- false; 
        } else {
          a <- 10; 
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

  // Any errors should be syntax errors.
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);

  std::cout << stmgr->toString() << std::endl;
  std::cout << sv->getErrors() << std::endl; 

  CHECK_FALSE(sv->hasErrors(0));
}

TEST_CASE("Inference If - 2", "[semantic]")
{
  antlr4::ANTLRInputStream input(
      R""""(
      proc infTest() {
        var a; 
        
        if(1 < 2) then {
          a <- false; 
        } else {
          var a <- 10; 
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

  // Any errors should be syntax errors.
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

  sv->visitCompilationUnit(tree);

  std::cout << stmgr->toString() << std::endl;
  std::cout << sv->getErrors() << std::endl; 

  CHECK_FALSE(sv->hasErrors(0));
}

