/**
 * @file codegen_tests.cpp
 * @author gpollice
 * @brief Basic tests for code generation
 * @version 0.1
 * @date 2022-08-05
 */
#include <catch2/catch_test_macros.hpp>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "WPLErrorHandler.h"
#include "SemanticVisitor.h"
#include "CodegenVisitor.h"
#include "HashUtils.h"

TEST_CASE("Development Codegen Tests", "[codegen]")
{
    antlr4::ANTLRInputStream input("int func program() { return -1; }");
    WPLLexer lexer(&input);
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
    CodegenVisitor *cv = new CodegenVisitor(pm, "test");
    cv->visitCompilationUnit(tree);
    if (cv->hasErrors())
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors());

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "5176d9cbe3d5f39bad703b71f652afd972037c5cb97818fa01297aa2bb185188");
}

TEST_CASE("programs/test1 - FIXME: DO BETTER", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test1.wpl");
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


    // if(sv->hasErrors())
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors());

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors())
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors());

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "0d04df4ae9f27c56e731772debb2adee582fd37b34798e231df501f8288328ab");
}

TEST_CASE("programs/test1a - FIXME: DO BETTER", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test1a.wpl");
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

    // if(sv->hasErrors())
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE(sv->hasErrors());

    // CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    // cv->visitCompilationUnit(tree);

    // // if (cv->hasErrors())
    // // {
    // //     CHECK("foo" == cv->getErrors());
    // // }
    // REQUIRE_FALSE(cv->hasErrors());

    // REQUIRE(llvmIrToSHA256(cv->getModule()) == "e0f894f1b6dd6613bd18eee553f5d6c8896228b4e4a21400b5dd6f1433ab0670");
}


TEST_CASE("programs/test2 - Scopes, multiple assignments, equality (non-arrays)", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test2.wpl");
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


    // if(sv->hasErrors())
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors());

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors())
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors());

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "70170b49a60e08f2dd0e8f32af6c6b00d43f79762885e151aaf4685959008e23");
}

TEST_CASE("programs/test4a - Use and redeclaration of parameters", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test4a.wpl");
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


    // if(sv->hasErrors())
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors());

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors())
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors());

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "18abfc620390385733b70c402618a1d51c779c39021c07a5d0800be830e70513");
}