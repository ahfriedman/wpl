#include <iostream>
#include <fstream>
#include "antlr4-runtime.h"
#include "CalculatorLexer.h"
#include "CalculatorParser.h"
#include "CalcErrorHandler.h"
#include "SemanticVisitor.h"
#include "CodegenVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"

llvm::cl::OptionCategory CalculatorOptions("Calculator Options");
static llvm::cl::opt<std::string>
    inputFileName(llvm::cl::Positional,
          llvm::cl::desc("<input file>"),
          llvm::cl::init("-"),
          llvm::cl::cat(CalculatorOptions));

static llvm::cl::opt<bool>
    printOutput("p", 
          llvm::cl::desc("Print the IR"),
          llvm::cl::cat(CalculatorOptions));

static llvm::cl::opt<std::string>
    inputString("s",
      llvm::cl::desc("Take input from a string, Do not use an input file if -s is used"),
      llvm::cl::value_desc("input string"),
      llvm::cl::init("-"),
      llvm::cl::cat(CalculatorOptions));

static llvm::cl::opt<std::string>
    outputFileName("o",
      llvm::cl::desc("supply alternate output file"),
      llvm::cl::value_desc("output file"),
      llvm::cl::init("-"),
      llvm::cl::cat(CalculatorOptions));

static llvm::cl::opt<bool>
    noCode("nocode", 
          llvm::cl::desc("Do not generate any output file"),
          llvm::cl::cat(CalculatorOptions));

/**
 * @brief Main compiler driver.
 */
int main(int argc, const char* argv[]) {
  /******************************************************************
   * Commandline handling from the llvm::cl classes.
   * @see https://llvm.org/docs/CommandLine.html
   * ******************************************************************/
  llvm::cl::HideUnrelatedOptions(CalculatorOptions);
  llvm::cl::ParseCommandLineOptions(argc, argv);

  if (((inputFileName == "-") && (inputString == "-")) 
      || ((inputFileName != "-") && (inputString != "-")))
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
  antlr4::ANTLRInputStream *input = nullptr;
  if (inputFileName != "-") {
    std::fstream *inStream = new std::fstream(inputFileName);
    input = new antlr4::ANTLRInputStream(*inStream);
  } else {
    input = new antlr4::ANTLRInputStream(inputString);
  }
  CalculatorLexer lexer(input);
  antlr4::CommonTokenStream tokens(&lexer);
  
  // 2. Create a parser from the token stream
  CalculatorParser parser(&tokens);   
  CalculatorParser::ProgramContext* tree = NULL;

  // 3. Parse the program and get the parse tree
  tree = parser.program();

  /******************************************************************
   * Perform semantic analysis and populate the symbol table
   * and bind nodes to Symbols using the property manager. If
   * there are any errors we print them out and exit.
   ******************************************************************/
  STManager *stm = new STManager();
  PropertyManager *pm = new PropertyManager();
  SemanticVisitor* sv = new SemanticVisitor(stm, pm);
  sv->visitProgram(tree);
  if (sv->hasErrors()) {
    std::cerr << sv->getErrors() << std::endl;
    return -1;
  }

  // Generate the LLVM IR code
  CodegenVisitor* cv = new CodegenVisitor(pm, "calculator.ll");
  cv->visitProgram(tree);
  if (cv->hasErrors()) {
    std::cerr << cv->getErrors() << std::endl;
    return -1;
  }

  // Print out the module contents.
  llvm::Module *module = cv->getModule();
  std::cout << std::endl << std::endl;
  if (printOutput) {
    cv->modPrint();
  }

  // Dump the code to an output file
  if (!noCode) {
    std::string irFileName;
    if (outputFileName != "-") {
      irFileName = outputFileName;
    } else {
      irFileName = inputFileName.substr(0,inputFileName.find_last_of('.'))+".ll";
    }
    std::error_code ec;
    llvm::raw_fd_ostream irFileStream(irFileName, ec);
    module->print(irFileStream, nullptr);
    irFileStream.flush();
  }

  return 0;
}