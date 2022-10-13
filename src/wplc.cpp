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

  /*******************************************************************
   * Prepare Input
   * ================================================================
   *
   * To do this, we  must first check if we were given a string
   * input or file(s). To make both cases easy to handle later on,
   * we create a vector of input streams/output file pairs.
   *******************************************************************/
  std::vector<std::pair<antlr4::ANTLRInputStream *, std::string>> inputs;

  // Case 1: We were given input files
  if (!inputFileName.empty())
  {

    bool useOutputFileName = outputFileName != "-.ll";

    // Used to prevent giving file names when compiling multiple files---just as clang does
    if (inputFileName.size() > 1 && useOutputFileName)
    {
      std::cerr << "Cannot specify output file name when generating multiple files." << std::endl;
      std::exit(-1);
    }

    // For each file name, make sure the file exist. If so, create an input stream to it
    // and set its output filename to be the provided name (if we are compiling just
    // one file, and a name was provided), or the file's name but with the .wpl
    // extension replaced with .ll
    for (auto fileName : inputFileName)
    {
      std::fstream *inStream = new std::fstream(fileName);

      if (inStream->fail())
      {
        std::cerr << "Error loading file: " << fileName << ". Does it exist?" << std::endl;
        std::exit(-1);
      }

      // TODO: THIS DOESN'T WORK IF NOT GIVEN A PROPER FILE EXTENSION
      inputs.push_back({new antlr4::ANTLRInputStream(*inStream),
                        useOutputFileName ? outputFileName : fileName.substr(0, fileName.find_last_of('.')) + ".ll"});
    }
  }
  else
  {
    // As we were given a string input, create a new String input with the output file
    inputs.push_back({new antlr4::ANTLRInputStream(inputString),
                      outputFileName});
  }

  // For each input...
  for (auto input : inputs)
  {
    /*******************************************************************
     * Create the Lexer from the input.
     * ================================================================
     *
     * Run the lexer on the input
     *******************************************************************/
    WPLLexer lexer(input.first);
    antlr4::CommonTokenStream tokens(&lexer);

    /*******************************************************************
     * Create + Run the Parser 
     * ================================================================
     *
     * Run the parser on our previously generated tokens
     *******************************************************************/
    WPLParser parser(&tokens);
    parser.removeErrorListeners();
    WPLSyntaxErrorListener *syntaxListener = new WPLSyntaxErrorListener();
    parser.addErrorListener(syntaxListener);
    // delete syntaxListener;

    // Run The parser
    WPLParser::CompilationUnitContext *tree = NULL;
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

    /*******************************************************************
     * Semantic Analysis
     * ================================================================
     *
     * Perform semantic analysis and populate the symbol table
     * and bind nodes to Symbols using the property manager. If
     * there are any errors we print them out and exit.
     *******************************************************************/
    STManager *stm = new STManager();
    PropertyManager *pm = new PropertyManager();
    SemanticVisitor *sv = new SemanticVisitor(stm, pm, flags);
    sv->visitCompilationUnit(tree);

    if (sv->hasErrors(0)) // Want to see all errors
    {
      std::cout << "Semantic analysis completed for " << input.second << " with errors: " << std::endl;
      std::cerr << sv->getErrors() << std::endl;
      // return -1;
      continue;
    }

    std::cout << "Semantic analysis completed for " << input.second << " without errors. Starting code generation..." << std::endl;

    /*******************************************************************
     * Code Generation
     * ================================================================
     *
     * If we have yet to recieve any errors for the file, then 
     * generate code for it. 
     *******************************************************************/
    CodegenVisitor *cv = new CodegenVisitor(pm, "WPLC.ll", flags);
    cv->visitCompilationUnit(tree);
    if (cv->hasErrors(0)) // Want to see all errors
    {
      std::cerr << cv->getErrors() << std::endl;
      continue;
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

    if (noRuntime)
    {
      std::cout << "Code generation completed for " << input.second << "; program does NOT support runtime." << std::endl;
    }
    else
    {
      std::cout << "Code generation completed for " << input.second << "; program may require runtime." << std::endl;
    }
  }

  return 0;
}