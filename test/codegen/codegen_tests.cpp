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
    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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

    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE(sv->hasErrors(0));

    // CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    // cv->visitCompilationUnit(tree);

    // // if (cv->hasErrors(0))
    // // {
    // //     CHECK("foo" == cv->getErrors());
    // // }
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "8975ef09cde54e108972189d9e7a8337aa89418c7fd94fd8ca14bc31d1cbc8c9");
}

TEST_CASE("programs/test3 - If w/o else - FIXME: DO BETTER", "[codegen]")
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "18abfc620390385733b70c402618a1d51c779c39021c07a5d0800be830e70513");
}

TEST_CASE("programs/test5 - Nested ifs and if equality - FIXME: DO BETTER", "[codegen]")
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "6499fa76c19d5f518248a26b68be585002588e2a52851469d37dd3c6e7529f0b");
}

TEST_CASE("programs/test6 - Basic Select with Return - FIXME: DO BETTER", "[codegen]")
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    if(sv->hasErrors(0))
    {
        CHECK("foo" == sv->getErrors());
    }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "0b667384676913d1b0aee9853fd9be42eabea34887e18cd1de6b54c5cf1823ca");
}

TEST_CASE("programs/test9sa - Global String Array", "[codegen]")
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "9e5834e13db9f511e072a26b22de0bee49ae44d914fc08ecb86f9b14f1c4fdc1");
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
    SemanticVisitor *sv = new SemanticVisitor(stm, pm);
    sv->visitCompilationUnit(tree);


    // if(sv->hasErrors(0))
    // {
    //     CHECK("foo" == sv->getErrors());
    // }
    REQUIRE_FALSE(sv->hasErrors(0));

    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll");
    cv->visitCompilationUnit(tree);

    if (cv->hasErrors(0))
    {
        CHECK("foo" == cv->getErrors());
    }
    CHECK_FALSE(cv->hasErrors(0));

    REQUIRE(llvmIrToSHA256(cv->getModule()) == "52c5ebd78944d1733c68270498483c16acc997d9e33fddcde44dc45d53a26a0b");
}