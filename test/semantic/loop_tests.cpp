#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

#include "test_error_handlers.h"


TEST_CASE("Undefined Params", "[semantic][loop]")
{
  antlr4::ANTLRInputStream input(
    "while i < 10 do {"
    " sum <- sum * 2; "
    " i <- i + 1; "    
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
  CHECK(sv->hasErrors());

  // CHECK_FALSE(sv->hasErrors());

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->is(Types::INT));
}

TEST_CASE("Sit & Spin", "[semantic][loop]")
{
  antlr4::ANTLRInputStream input(
    "while true do {"
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

  CHECK_FALSE(sv->hasErrors());

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->is(Types::INT));
}

TEST_CASE("Int condition", "[semantic][loop]")
{
  antlr4::ANTLRInputStream input(
    "while 1 do {"
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

  CHECK(sv->hasErrors());

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->is(Types::INT));
}

TEST_CASE("Int condition 2", "[semantic][loop]")
{
  antlr4::ANTLRInputStream input(
    "while 1 * 3 / 2 do {"
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

  CHECK(sv->hasErrors());

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->is(Types::INT));
}

TEST_CASE("str condition", "[semantic][loop]")
{
  antlr4::ANTLRInputStream input(
    "while \"This is not a boolean\" do {"
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

  CHECK(sv->hasErrors());

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->is(Types::INT));
}

TEST_CASE("Boolean Expr", "[semantic][loop]")
{
  antlr4::ANTLRInputStream input(
    "while ~false & true | false do {"
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

  CHECK_FALSE(sv->hasErrors());

  // std::optional<Symbol *> opt = stmgr->lookup("a");

  // CHECK(opt.has_value());
  // CHECK(opt.value()->type->is(Types::INT));
}
