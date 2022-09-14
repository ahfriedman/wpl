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

TEST_CASE("Bool Const Tests", "[semantic]") {
  antlr4::ANTLRInputStream input("false; true;");
  WPLLexer lexer(&input); 
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens); 
  parser.removeErrorListeners();
  WPLParser::CompilationUnitContext* tree = NULL; 
  
  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL); 

  SemanticVisitor* sv = new SemanticVisitor(new STManager(), new PropertyManager());
  sv->visitCompilationUnit(tree); 

  CHECK_FALSE(sv->hasErrors());
}

TEST_CASE("visitType Tests", "[semantic]") {
  antlr4::ANTLRInputStream input("int"); //FIXME: We get filtered out, don't we
  WPLLexer lexer(&input); 
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens); 
  parser.removeErrorListeners();
  WPLParser::CompilationUnitContext* tree = NULL; 
  
  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL); 

  STManager* stmgr = new STManager(); 
  SemanticVisitor* sv = new SemanticVisitor(stmgr, new PropertyManager());
  sv->visitCompilationUnit(tree); 

  std::cout << stmgr->toString() << std::endl;
  std::cout << tree->getText() << std::endl;  

  CHECK_FALSE(true);//sv->hasErrors());
}