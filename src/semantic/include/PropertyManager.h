#pragma once
#include "Symbol.h"
#include "antlr4-runtime.h"

class PropertyManager {
  public:
    // Get the Symbol associated with this node
    Symbol* getBinding(antlr4::tree::ParseTree *ctx) {
      return bindings.get(ctx);
    }

    // Bind the symbol to the node
    void bind(antlr4::tree::ParseTree *ctx, Symbol* symbol) {
      std::cout << "BIND: " << ctx->getText() << " TO " << symbol->toString() << "@" << symbol << std::endl; 
      bindings.put(ctx, symbol);
    }

  private:
    antlr4::tree::ParseTreeProperty<Symbol*> bindings;
};