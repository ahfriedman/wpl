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
#include "CompilerFlags.h"

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);
    CodegenVisitor *cv = new CodegenVisitor(pm, "test", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);
    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "5176d9cbe3d5f39bad703b71f652afd972037c5cb97818fa01297aa2bb185188");
}

TEST_CASE("programs/test1 - General Overview", "[codegen]")
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "0d04df4ae9f27c56e731772debb2adee582fd37b34798e231df501f8288328ab");
}

TEST_CASE("programs/test1-full - General Overview - full", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test1-full.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "4ec8bf45d3a4b2114b2bd691018473cfed3b8f7688e626a626ea3cf691ad8f4f");
}

TEST_CASE("programs/test1a", "[codegen]")
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE(sv->hasErrors(0));

    // CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    // cv->visitCompilationUnit(tree);

    // REQUIRE_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "6fe85492b8cabea3bee19c2fc54a5276861eb992ebdb4a97de10008cefb0832e");
}

TEST_CASE("programs/test3 - If w/o else", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test3.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "9ed4c86a2b635e43b517c4cf1625006d87237e45aef632e8fcd7d2694175c95e");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "18abfc620390385733b70c402618a1d51c779c39021c07a5d0800be830e70513");
}

TEST_CASE("programs/test5 - Nested ifs and if equality", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test5.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "6499fa76c19d5f518248a26b68be585002588e2a52851469d37dd3c6e7529f0b");
}

TEST_CASE("programs/test6 - Basic Select with Return", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test6.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "686f9e63c3f0c7f09de2dbc0ca6a6e8ae5161be63048a68814c74c5164c33305");
}

TEST_CASE("programs/testSelectBlock1 - Basic Select with Blocks that Return", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/testSelectBlock1.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    //NOTE: THIS SHOULD BE THE SAME AS test6!!!
    REQUIRE(llvmIrToSHA256(cv->getModule()) == "686f9e63c3f0c7f09de2dbc0ca6a6e8ae5161be63048a68814c74c5164c33305");
}

TEST_CASE("programs/test6a - Basic Nested Selects, LEQ, GEQ", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test6a.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "d86d9c224f8b22cfc4a52bab88aa5f9ce60016604aafe0e865dc6fc0aa177093");
}

TEST_CASE("programs/testSelectBlock2 - Select with blocks that don't return", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/testSelectBlock2.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    //NOTE: Should be same as test6a
    REQUIRE(llvmIrToSHA256(cv->getModule()) == "d86d9c224f8b22cfc4a52bab88aa5f9ce60016604aafe0e865dc6fc0aa177093");
}

TEST_CASE("programs/test7 - Test String equality + Nested Loops", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test7.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "a78e889150724d1dee3ee614df0caee51d717748a2b2ef1ace7a76d34da06716");
}

TEST_CASE("programs/test8 - Nested Loops", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test8.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "c632ea5d57a074cd477ef3d48798816a64858c619830e98b463bb3360fdf085b");
}

TEST_CASE("programs/test9i - Global Integers", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9i.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "328e5295eaad33348eee23da5f8302153e778c8bb631707f37fd4d03daae7cfa");
}

TEST_CASE("programs/test9iv - Global Integer Inference", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9iv.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "328e5295eaad33348eee23da5f8302153e778c8bb631707f37fd4d03daae7cfa");
}

TEST_CASE("programs/test9b - Global Booleans", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9b.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "d871755f7c89cd02ee76c67541ec1935a2117a1e3f23cc86a09573fa1ef8422c");
}

TEST_CASE("programs/test9bv - Global Boolean Inference", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9bv.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "d871755f7c89cd02ee76c67541ec1935a2117a1e3f23cc86a09573fa1ef8422c");
}


TEST_CASE("programs/test9s - Global Strings", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9s.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "4b20b6db3a31f068485c29786b8ca97704cff2931829462517cf402811ed711a");
}

TEST_CASE("programs/test9sv - Global String Inference", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9sv.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "4b20b6db3a31f068485c29786b8ca97704cff2931829462517cf402811ed711a");
}

TEST_CASE("programs/test9ba - Global Boolean Array", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9ba.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "b6b98b36e98caffd2ef053023bd97db39bfc12c3a2c38b90bffacb8fb1436097");
}

TEST_CASE("programs/test9ia - Global Integer Array", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9ia.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "0b667384676913d1b0aee9853fd9be42eabea34887e18cd1de6b54c5cf1823ca");
}

TEST_CASE("programs/test9sa - Global String Array - FLAWED", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9sa.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "9e5834e13db9f511e072a26b22de0bee49ae44d914fc08ecb86f9b14f1c4fdc1");
}

TEST_CASE("programs/test9sa-1 - Global String Array - CORRECT", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test9sa-1.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "5ee667e609c925ca3dda1d158aeb2e8bf70818e7ef91965f9eadcd5243d6e76c");
}


TEST_CASE("programs/test11 - Expressions in decl (let*) ", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test11.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "52c5ebd78944d1733c68270498483c16acc997d9e33fddcde44dc45d53a26a0b");
}

TEST_CASE("programs/test12 - Scopes & Prime Finder Example! ", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test12.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "209c1960ddf81cfa4490c2d8a0501577b799bdacc5d829ce2530f094559ed915");
}

TEST_CASE("programs/test13 - Recursive Fibonacci", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test13.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "fd2a506796e162de9e1a00a12e154827fa25a34833b68c68aa83750e3c4ab657");
}

TEST_CASE("programs/test-runtime - Basic runtime tests", "[codegen]")
{
    //NOTE: tested linking each runtime function locally 
    std::fstream *inStream = new std::fstream("/home/shared/programs/test-runtime.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "6c84e861ca345b6cf3b11cf5abe99daed731acac1e48a302ac8efe6640cad7b6");
}

TEST_CASE("programs/test-shortcircuit - Basic Short Circuit (and)", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test-shortcircuit.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "e664f3cf5af34edd6f3fecb5248e4c6390565443c942e01f5652c7b5b3c3d4ba");
}

TEST_CASE("programs/test-shortcircuit-rt - Basic Short Circuit (and + or) w/ Runtime", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test-shortcircuit-rt.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "2309d18c37760ca695b7acd1fe81c7bf428ea5e44764f35e6c993780a3815459");
}


TEST_CASE("programs/test-arrayAssign - Assigning one array to another and editing arrays in functions", "[codegen]")
{
    //WPL is pass by value! 

    std::fstream *inStream = new std::fstream("/home/shared/programs/test-arrayAssign.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "62fbd52566574970733029781d99322e75c828df74aece217c00f0fb1795dcf1");
}

TEST_CASE("programs/externProc - Declaring an external proc", "[codegen]")
{
    //FIXME: TEST WITH AN ACTUAL LINK!
    std::fstream *inStream = new std::fstream("/home/shared/programs/externProc.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "0f2daef6c7dfd9b16fb7fa267083989eabdd95407e23e97064eef23fd7a4a319");
}

TEST_CASE("programs/test14a - Test nested/more complex shorting", "[codegen]")
{
    //TODO: MANY OF THE SHORT CIRCUITING CASES COULD BE OPTIMIZED!!! -> Done?
    std::fstream *inStream = new std::fstream("/home/shared/programs/test14a.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "a4ac4a3664f5835bc88328e2dd925472074de7b0f9977106f0685508130628f8");
}

TEST_CASE("programs/test18 - Parody", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test18.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "9c4c64be2b335ecf58d9bbe4b267adc12a5b74518f587f30ba37591be79b09c1");
}

TEST_CASE("programs/test19 - Editing Global String and Using Across Inv", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/test19.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "bd911e955a6da1d18d92cf4ca430456daa8ea8c4c8601f778e0f8bb83178222e");
}

TEST_CASE("programs/testGlobalAndLocal - Parody", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/testGlobalAndLocal.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "f9b33450b1f1522122b1dffbf07819f959e08aac6ccdfc112cac7c25690a5c78");
}

TEST_CASE("programs/forwardWrongArg - Forward Declaration w/ wrong arg name", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/forwardWrongArg.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE(cv->hasErrors(0));

    // REQUIRE(llvmIrToSHA256(cv->getModule()) == "c1698114ebca6c348ee9f7ae41ea95a8d0377d0eda8f21711d3bb5501bee49ba");
}

TEST_CASE("programs/Lambda1 - Basic lambda Test", "[codegen][lambda]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/Lambda1.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "6e352285edacc42db930c25774b68b3a8fff977e2e858a61294b1ac14ad8c3a0");
}

//FIXME: LAMBDAS, ENUMS CAN"T BE ASSIGNED OUTSIDE OF DECL

TEST_CASE("programs/Lambda2 - Basic lambda Test w/ return", "[codegen][lambda]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/Lambda2.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "ee35f7c8a82e6dece344d8a245c48982d894ea3e4db0f2ba939e0bea65e9e1e8");
}

TEST_CASE("programs/Lambda3 - Basic lambda Test w/ return and same name", "[codegen][lambda]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/Lambda3.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "526b0bc9f9b76dad00ad13676e16a11273b40e928342c848bdb9b7e8dfd75ab9");
}

TEST_CASE("programs/externLambda", "[codegen][lambda]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/externLambda.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "69a65ba7c965baab8ac203f032c7a7e8956f67ebf4699dbf682866de770404cb");
}

TEST_CASE("programs/enum1 - Basic Enum 1", "[codegen][enum]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/enum1.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "c8988c284c4ae357ff29cf4e222a3ea2a53e1ce6e641b2098355c4c0c57b5279");
}

TEST_CASE("programs/enum2 - Basic Enum 2", "[codegen][enum]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/enum2.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "b82b68dc5cd14fe95a69d8e78dc5dac3544c95f36643cd8f72102c458f9b5c1d");
}

TEST_CASE("programs/enumAssign - Same a  Enum 2 but with assignmens outside of decl", "[codegen][enum]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/enumAssign.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "5fb322db3a1c17135df54896d03d1312116a66754a7504660052950f6390e0df");
}

TEST_CASE("programs/StructTest2", "[codegen][struct]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/adv/StructTest2.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "959a3a3209316c2f10186c59f5db5552fd2f8b9ca5624a95f4b837e1cc0e8b0e");
}

TEST_CASE("programs/StructTest3", "[codegen][struct]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/adv/StructTest3.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "d6c8f8d7281225520fe7b276946031ef6ff0aa7be0fc65e7c2f2a610f6f0721d");
}

TEST_CASE("programs/StructTest4", "[codegen][struct]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/adv/StructTest4.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "c70f78537551fa7e0c20633aa8dcbdaa3635ffde514d919758dc42dd62e13e25");
}

TEST_CASE("Out of order function", "[codegen][program]")
{
  antlr4::ANTLRInputStream input(
      R""""(
extern int func printf(...);

str a <- "hello";

int func program() {
    foo(); 
    return 0;
}

proc foo() {
    printf("a = %s\n", a);
}
    )"""");
  WPLLexer lexer(&input);
  // lexer.removeErrorListeners();
  // lexer.addErrorListener(new TestErrorListener());
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
//   parser.addErrorListener(new TestErrorListener()); //FIXME: SHOULD WE TEST THESE HERE?

  WPLParser::CompilationUnitContext *tree = NULL;
  REQUIRE_NOTHROW(tree = parser.compilationUnit());
  REQUIRE(tree != NULL);
  REQUIRE(tree->getText() != "");

  STManager *stmgr = new STManager();
  PropertyManager *pm = new PropertyManager();
  SemanticVisitor *sv = new SemanticVisitor(stmgr, pm);

  sv->visitCompilationUnit(tree);
  REQUIRE_FALSE(sv->hasErrors(ERROR));
  CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
  cv->visitCompilationUnit(tree);
  REQUIRE_FALSE(cv->hasErrors(0));

  REQUIRE(llvmIrToSHA256(cv->getModule()) == "c226659bfe066e66d2491aa77e5073a4d512f3ddb376a29a8355cfe7b7022e18");
}


/************************************
 * Example C-Level Tests
 ************************************/
TEST_CASE("C Level Positive Test #1", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/CLevel/CPositive1.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "fdf9d19f4d205022b83770d7ec87c120a51d65fc5719eaad7d3a65d955aee64c");
}

TEST_CASE("C Level Positive Test #2", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/CLevel/CPositive2.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "638d4d94b07e609e157cf894755225435fd1d19d52f47f5c4a0b1eb8ca08e2b2");
}

/************************************
 * Example B-Level Tests
 ************************************/
TEST_CASE("B Level Positive Test #1", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/BLevel/BPositive1.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "989e43c3b967d4cc7e230e39a6d1d00bf14e3306e50c1fd2d6dbed6b668924eb");
}

TEST_CASE("B Level Positive Test #2", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/BLevel/BPositive2.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "a7d0860f47d046fb0a819057f8abc6cac2fc3046f0dd8b9ad631a6ededb7c328");
}

/************************************
 * Example A-Level Tests
 ************************************/
TEST_CASE("A Level Positive Test #1", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/ALevel/APositive1.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "2838be9e472fa2455af9851905f03f970170d67e7c1ba49cc110a274d578651b");
}

TEST_CASE("A Level Positive Test #2", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/ALevel/APositive2.wpl");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, 0);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "3ab074cdd00ea287689233a158d1bc268eaa11ac1d07fb1e2fb633e8bca8eaca");
}