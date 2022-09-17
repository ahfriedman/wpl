#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"

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

  SECTION("Visiting Compilation Unit Context")
  {
    WPLParser::CompilationUnitContext *tree = NULL;
    REQUIRE_NOTHROW(tree = parser.compilationUnit());
    REQUIRE(tree != NULL);

    // Any errors should be syntax errors.
    // FIXME: Should probably confirm the above statement through testing for syntax errors
    REQUIRE(tree->getText() == "");
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
  Type* TOP =  new Type();
  Type* INT = new TypeInt();

  // Type BOOL = TypeBool();
  // Type STR = TypeStr();
  // Type BOT = TypeBot();

  SECTION("Top Type tests")
  {
    REQUIRE(TOP->is(TOP));
    REQUIRE_FALSE(TOP->isNot(TOP));

    REQUIRE(TOP->is(INT));
    REQUIRE_FALSE(TOP->isNot(INT));

  //   REQUIRE((TOP == BOOL));
  //   REQUIRE_FALSE((TOP != BOOL));

  //   REQUIRE((TOP == STR));
  //   REQUIRE_FALSE((TOP != STR));

  //   REQUIRE((TOP == BOT));
  //   REQUIRE_FALSE((TOP != BOT));
  }


  SECTION("Int Type tests") {
    // REQUIRE(INT->isNot(*TOP));
    // REQUIRE_FALSE(INT->is(*TOP));

    REQUIRE(INT->is(INT));
    // REQUIRE(INT->is(TypeInt()));
  }
  // Why is PL easier to read in mono fonts?
}