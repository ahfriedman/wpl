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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "f4b7fbda0e0f7f25c20056aacce4eed4c1bcfffb1464c6ba525d9f156238b4c9");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);

    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "e1e33543132cb39593a2b3bac2192c7b657c38c7c6c1f3c708c5b2a7bb7aa2bc");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, CompilerFlags::NO_RUNTIME);
    sv->visitCompilationUnit(tree);


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", CompilerFlags::NO_RUNTIME);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "875789f4dd74cb624ba74d21d3079a6fd89076028f5c8065875aa170cf94f785");
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

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "759cb24b5efa3fa2fc27b00fc36c7b7863b312f156c6489e5cb802926a5bcd24");
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
    //TODO: MANY OF THE SHORT CIRCUITING CASES COULD BE OPTIMIZED!!!
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

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "e6a2a65891ddb6fdc968fea7091c96cbceefca2f243e18c3f1aceb5776e04dfe");
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

TEST_CASE("programs/testForwardDecl - Forward Declaration", "[codegen]")
{
    std::fstream *inStream = new std::fstream("/home/shared/programs/testForwardDecl.wpl");
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

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "3412ac53de5141c955eae0f21cc22df848126af700acb8dc2ec84649f2a4ce9f");
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


    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", 0);
    cv->visitCompilationUnit(tree);

    REQUIRE_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "c1698114ebca6c348ee9f7ae41ea95a8d0377d0eda8f21711d3bb5501bee49ba");
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

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "28857ea4eb9243486dea893cf00fa69d95b811df8a327308f6a7a89a1da47a6f");
}

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

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "5a0a559931acaef13bd08bcea1dfc4f76824129f176ad43cc4ae39f79cd61951");
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