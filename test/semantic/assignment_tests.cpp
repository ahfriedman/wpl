#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

#include "test_error_handlers.h"

TEST_CASE("Basic Assignments", "[semantic]")
{
  // FIXME: is a <- 2 allowed without a type/previous definition?
  SECTION("Basic Int Test")
  {
    antlr4::ANTLRInputStream input("int a <- 2;");
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

    CHECK_FALSE(sv->hasErrors());

    std::optional<Symbol *> opt = stmgr->lookup("a");

    CHECK(opt.has_value());
    CHECK(opt.value()->type->isSubtype(Types::INT));
  }
}

TEST_CASE("Assignment: Int Expr", "[semantic]")
{
  antlr4::ANTLRInputStream input("int a <- 2 * (2 / 4 + 3 - -2);");
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

  CHECK_FALSE(sv->hasErrors());

  std::optional<Symbol *> opt = stmgr->lookup("a");

  CHECK(opt.has_value());
  CHECK(opt.value()->type->isSubtype(Types::INT));
}

TEST_CASE("Assignment: Bool const", "[semantic]")
{
  antlr4::ANTLRInputStream input("boolean a <- false;");
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

  CHECK_FALSE(sv->hasErrors());

  std::optional<Symbol *> opt = stmgr->lookup("a");

  CHECK(opt.has_value());
  CHECK(opt.value()->type->isSubtype(Types::BOOL));
}

TEST_CASE("Assignment: Bool expr", "[semantic]")
{
  antlr4::ANTLRInputStream input("boolean a <- (false | true) & ~false;");
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

  CHECK_FALSE(sv->hasErrors());

  std::optional<Symbol *> opt = stmgr->lookup("a");

  CHECK(opt.has_value());
  CHECK(opt.value()->type->isSubtype(Types::BOOL));
}

TEST_CASE("Assignment: String const", "[semantic]")
{
  antlr4::ANTLRInputStream input("str a <- \"Hello world!\";");
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

  CHECK_FALSE(sv->hasErrors());

  std::optional<Symbol *> opt = stmgr->lookup("a");

  CHECK(opt.has_value());
  CHECK(opt.value()->type->isSubtype(Types::STR));
}

//FIXME: CAN PARENS BE USED ON STRINGS?
//FIXME: Add some tests to verify that we don't allow ops to be use incorrectly!
