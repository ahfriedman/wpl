#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

#include "test_error_handlers.h"

TEST_CASE("Development tests", "[semantic]")
{
  antlr4::ANTLRInputStream input("42;");
  WPLLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
  WPLParser::CompilationUnitContext *tree = NULL;
  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL);
  SemanticVisitor *sv = new SemanticVisitor(new STManager(), new PropertyManager()); // NEW
  sv->visitCompilationUnit(tree);                                                    // NEW
  // Error checking is NEW
  // if (sv->hasErrors()) {
  //   CHECK("foo" == sv->getErrors());
  // }
  CHECK_FALSE(sv->hasErrors());
}

TEST_CASE("Bool Const Tests", "[semantic]")
{
  antlr4::ANTLRInputStream input("false; true;");
  WPLLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
  WPLParser::CompilationUnitContext *tree = NULL;

  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL);

  SemanticVisitor *sv = new SemanticVisitor(new STManager(), new PropertyManager());
  sv->visitCompilationUnit(tree);

  CHECK_FALSE(sv->hasErrors());
}

TEST_CASE("visitType Tests", "[semantic]")
{
  antlr4::ANTLRInputStream input("int"); // FIXME: We get filtered out, don't we
  WPLLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new TestErrorListener());

  SECTION("Visiting Compilation Unit Context")
  {
    WPLParser::CompilationUnitContext *tree = NULL;
    REQUIRE_THROWS(tree = parser.compilationUnit());
    REQUIRE(tree != NULL);
  }

  SECTION("Visiting Type Context")
  {
    WPLParser::TypeContext *tree = NULL;
    REQUIRE_NOTHROW(tree = parser.type());
    REQUIRE(tree != NULL);

    REQUIRE(tree->getText() != "");
  }
  // REQUIRE_NOTHROW(tree = parser.type());
  // REQUIRE(tree != NULL);
  // REQUIRE(tree->getText() != "");

  // STManager* stmgr = new STManager();
  // SemanticVisitor* sv = new SemanticVisitor(stmgr, new PropertyManager());
  // sv->visitType(tree);//visitCompilationUnit(tree);

  // std::cout << stmgr->toString() << std::endl;
  // std::cout << tree->getText() << std::endl;

  // CHECK_FALSE(true);//sv->hasErrors());
}

TEST_CASE("Test Type Equality", "[semantic]")
{
  Type *TOP = new Type();
  Type *INT = new TypeInt();
  Type *BOOL = new TypeBool();
  Type *STR = new TypeStr();
  Type *BOT = new TypeBot();

  SECTION("Top Type tests")
  {
    REQUIRE(TOP->is(TOP));
    REQUIRE_FALSE(TOP->isNot(TOP));

    REQUIRE(TOP->is(INT));
    REQUIRE_FALSE(TOP->isNot(INT));

    REQUIRE(TOP->is(BOOL));
    REQUIRE_FALSE(TOP->isNot(BOOL));

    REQUIRE(TOP->is(STR));

    REQUIRE(TOP->is(BOT));
  }

  SECTION("Int Type tests")
  {
    REQUIRE(INT->isNot(TOP));
    REQUIRE_FALSE(INT->is(TOP));

    REQUIRE(INT->is(INT));
    // REQUIRE(INT->is(TypeInt()));

    REQUIRE(INT->isNot(BOOL));

    REQUIRE(INT->isNot(STR));

    REQUIRE(INT->isNot(BOT));
  }

  SECTION("Bool Type Tests")
  {
    REQUIRE(BOOL->isNot(TOP));
    REQUIRE(BOOL->isNot(INT));
    REQUIRE(BOOL->isNot(STR));
    REQUIRE(BOOL->is(BOOL));
    REQUIRE(BOOL->isNot(BOT));
  }

  SECTION("Str Type Tests")
  {
    REQUIRE(STR->isNot(TOP));
    REQUIRE(STR->isNot(INT));
    REQUIRE(STR->is(STR));
    REQUIRE(STR->isNot(BOOL));
    REQUIRE(STR->isNot(BOT));
  }

  SECTION("Bot Type Tests")
  {
    REQUIRE(BOT->isNot(TOP));
    REQUIRE(BOT->isNot(INT));
    REQUIRE(BOT->isNot(STR));
    REQUIRE(BOT->isNot(BOOL));
    REQUIRE(BOT->isNot(BOT));
  }
  // Why is PL easier to read in mono fonts?
}

// FIXME: REQUIRE END IN RETURN

TEST_CASE("visitbasicProc", "[semantic]")
{
  SECTION("No arguments & empty")
  {
    antlr4::ANTLRInputStream input("proc program () {}");
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
    std::cout << tree->getText() << std::endl;

    CHECK_FALSE(sv->hasErrors());
  }

  // FIXME: test recursion

  SECTION("1 arg & empty")
  {
    antlr4::ANTLRInputStream input("proc program (int a) {}");
    WPLLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    WPLParser parser(&tokens);
    parser.removeErrorListeners();

    WPLParser::CompilationUnitContext *tree = NULL;
    REQUIRE_NOTHROW(tree = parser.compilationUnit());
    REQUIRE(tree != NULL);

    std::cout << "PAST" << std::endl;
    // Any errors should be syntax errors.
    // FIXME: Should probably confirm the above statement through testing for syntax errors
    REQUIRE(tree->getText() != "");

    STManager *stmgr = new STManager();
    SemanticVisitor *sv = new SemanticVisitor(stmgr, new PropertyManager());

    sv->visitCompilationUnit(tree);

    std::cout << stmgr->toString() << std::endl;
    std::cout << tree->getText() << std::endl;

    CHECK_FALSE(sv->hasErrors());
  }
}

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

    std::optional<Symbol*> opt = stmgr->lookup("a");

    CHECK(opt.has_value());
    CHECK(opt.value()->type->is(Types::INT));
  }
}