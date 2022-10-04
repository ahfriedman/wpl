#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

#include "test_error_handlers.h"


TEST_CASE("Basic select", "[semantic][conditional]")
{
  //FIXME: should select allow 0 cases then throw an error semantically?
  antlr4::ANTLRInputStream input(
    R""""(
      select {
        false : int a <- 2;


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

  //FIXME: CHECK MORE PRECISELY 
  CHECK_FALSE(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}



TEST_CASE("Basic select inf error 1", "[semantic][conditional]")
{
  //FIXME: should select allow 0 cases then throw an error semantically?
  antlr4::ANTLRInputStream input(
    R""""(
      var a; 

      select {
        false : a <- true; 
        true : a <- 10; 
        

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

  //FIXME: CHECK MORE PRECISELY 
  CHECK(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("Basic select inf 1", "[semantic][conditional]")
{
  //FIXME: should select allow 0 cases then throw an error semantically?
  antlr4::ANTLRInputStream input(
    R""""(
      var a; 

      select {
        false : boolean a <- true; 
        true : a <- 10; 
        

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

  //FIXME: CHECK MORE PRECISELY 
  CHECK_FALSE(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("Basic select inf 2", "[semantic][conditional]")
{
  //FIXME: should select allow 0 cases then throw an error semantically?
  antlr4::ANTLRInputStream input(
    R""""(
      var a; 

      select {
        false : a <- true; 
        true : int a <- 10; 
        

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

  std::cout << sv->getErrors() << std::endl; 
  //FIXME: CHECK MORE PRECISELY 
  CHECK_FALSE(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}

TEST_CASE("Basic select inf 3", "[semantic][conditional]")
{
  //FIXME: should select allow 0 cases then throw an error semantically?
  antlr4::ANTLRInputStream input(
    R""""(
      var a; 

      select {
        false : a <- 11; 
        true : a <- 10; 
        

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

  //FIXME: CHECK MORE PRECISELY 
  CHECK_FALSE(sv->hasErrors(ERROR));

  // CHECK_FALSE(sv->hasErrors(ERROR));

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->isSubtypeOf(Types::INT));
}