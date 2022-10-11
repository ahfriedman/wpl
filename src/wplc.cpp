/**
 * @file wplc.cpp
 * @author gpollice
 * @brief
 * @version 0.1
 * @date 2022-09-07
 *
 * @copyright Copyright (c) 2022
 *
 * NOTE: You may want to allow multiple files and
 * loop through them so that you can compile multiple
 * files with one command line.
 */
#include <iostream>
#include <fstream>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
// #include "WPLErrorHandler.h"
#include "SemanticVisitor.h"
#include "CodegenVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"

llvm::cl::OptionCategory WPLCOptions("wplc Options");
static llvm::cl::list<std::string>
    inputFileName(llvm::cl::Positional,
                  llvm::cl::desc("<input files>"),
                  // llvm::cl::init("-")
                  llvm::cl::OneOrMore,
                  llvm::cl::cat(WPLCOptions));

static llvm::cl::opt<bool>
    printOutput("p",
                llvm::cl::desc("Print the IR"),
                llvm::cl::cat(WPLCOptions));

static llvm::cl::opt<std::string>
    inputString("s",
                llvm::cl::desc("Take input from a string, Do not use an input file if -s is used"),
                llvm::cl::value_desc("input string"),
                llvm::cl::init("-"),
                llvm::cl::cat(WPLCOptions));

static llvm::cl::opt<std::string>
    outputFileName("o",
                   llvm::cl::desc("supply alternate output file"),
                   llvm::cl::value_desc("output file"),
                   llvm::cl::init("-.ll"),
                   llvm::cl::cat(WPLCOptions));

static llvm::cl::opt<bool>
    noCode("nocode",
           llvm::cl::desc("Do not generate any output file"),
           llvm::cl::cat(WPLCOptions));

static llvm::cl::opt<bool>
    noRuntime("no-runtime",
              llvm::cl::desc("Program will not use the WPL runtime; Compiler will automatically treat program() as the entry point."),
              llvm::cl::cat(WPLCOptions));

/**
 * @brief Main compiler driver.
 */
int main(int argc, const char *argv[])
{
  /******************************************************************
   * Commandline handling from the llvm::cl classes.
   * @see https://llvm.org/docs/CommandLine.html
   * ******************************************************************/
  llvm::cl::HideUnrelatedOptions(WPLCOptions);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  if (inputFileName.empty() && inputString == "-")
  {
    std::cerr << "Please enter a file or an input string to compile." << std::endl;
    std::exit(-1);
  }

  if (!inputFileName.empty() && inputString != "-")
  {
    std::cerr << "You can only have an input file or and input string, but not both" << std::endl;
    std::exit(-1);
  }

  /******************************************************************
   * Now that we have the input, we can perform the first stage:
   * 1. Create the lexer from the input.
   * 2. Create the parser with the lexer's token stream as input.
   * 3. Parse the input and get the parse tree for then exit stage.
   * 4. TBD: handle errors
   ******************************************************************/

  // 1. Create the lexer

  std::vector<std::pair<antlr4::ANTLRInputStream *, std::string>> inputs;

  if (!inputFileName.empty())
  {

    bool useOutputFileName = outputFileName != "-.ll";

    if (inputFileName.size() > 1 && useOutputFileName)
    {
      std::cerr << "Cannot specify output file name when generating multiple files." << std::endl;
      std::exit(-1);
    }

    for (auto fileName : inputFileName)
    {
      std::fstream *inStream = new std::fstream(fileName);

      if (inStream->fail())
      {
        std::cerr << "Error loading file: " << fileName << ". Does it exist?" << std::endl;
        std::exit(-1);
      }

      // inputFileName.substr(0, inputFileName.find_last_of('.')) + ".ll"
      inputs.push_back({new antlr4::ANTLRInputStream(*inStream),
                        useOutputFileName ? outputFileName : fileName.substr(0, fileName.find_last_of('.')) + ".ll"});
    }
  }
  else
  {
    inputs.push_back({new antlr4::ANTLRInputStream(inputString),
                      outputFileName});
  }

  for (auto input : inputs)
  {
    WPLLexer lexer(input.first);
    antlr4::CommonTokenStream tokens(&lexer);

    // 2. Create a parser from the token stream
    WPLParser parser(&tokens);

    parser.removeErrorListeners();
    WPLSyntaxErrorListener *syntaxListener = new WPLSyntaxErrorListener();
    parser.addErrorListener(syntaxListener);
    // delete syntaxListener;

    WPLParser::CompilationUnitContext *tree = NULL;

    // 3. Parse the program and get the parse tree
    tree = parser.compilationUnit();

    if (syntaxListener->hasErrors(0)) // Want to see all errors.
    {
      std::cerr << syntaxListener->errorList() << std::endl;
      return -1;
    }

    /*
     * Sets up compiler flags. These need to be sent to the visitors.
     */

    int flags = (noRuntime) ? CompilerFlags::NO_RUNTIME : 0;

    /******************************************************************
     * Perform semantic analysis and populate the symbol table
     * and bind nodes to Symbols using the property manager. If
     * there are any errors we print them out and exit.
     ******************************************************************/
    STManager *stm = new STManager();
    PropertyManager *pm = new PropertyManager();
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, flags);
    sv->visitCompilationUnit(tree);

    if (sv->hasErrors(0)) // Want to see all errors
    {
      std::cerr << sv->getErrors() << std::endl;
      return -1;
    }

    std::cout << "Semantic analysis completed for " << input.second << " without errors. Starting code generation..." << std::endl;

    // Generate the LLVM IR code
    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", flags);
    cv->visitCompilationUnit(tree);
    if (cv->hasErrors(0)) // Want to see all errors
    {
      std::cerr << cv->getErrors() << std::endl;
      return -1;
    }

    // Print out the module contents.
    llvm::Module *module = cv->getModule();

    if (printOutput)
    {
      std::cout << std::endl
                << std::endl;
      cv->modPrint();
    }

    // Dump the code to an output file
    if (!noCode)
    {
      std::string irFileName = input.second;
      std::error_code ec;
      llvm::raw_fd_ostream irFileStream(irFileName, ec);
      module->print(irFileStream, nullptr);
      irFileStream.flush();
    }
  }

  if (noRuntime)
  {
    std::cout << "Code generation completed; program does NOT support runtime." << std::endl;
  }
  else
  {
    std::cout << "Code generation completed; program may require runtime." << std::endl;
  }

  return 0;
}