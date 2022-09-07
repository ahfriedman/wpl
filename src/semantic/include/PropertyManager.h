/**
 * @file PropertyManager.h
 * @author your name (you@domain.com)
 * @brief Handles all of the ParseTreeProperties. This will be created in the driver and
 *  made available to whatever class needs it.
 * @version 0.1
 * @date 2022-07-23
 * @see https://github.com/AuxPort66/FIB-CL-LAB/blob/350636d84d61d4011f0440feac34ef5fd693bf4b/common/TreeDecoration.h
 * @see https://github.com/AuxPort66/FIB-CL-LAB/blob/350636d84d61d4011f0440feac34ef5fd693bf4b/common/TreeDecoration.cpp
 */
#pragma once
#include "Symbol.h"
#include "antlr4-runtime.h"

class PropertyManager {
  public:
    // Get the Symbol associated with this node
    Symbol* getBinding(antlr4::ParserRuleContext *ctx) {
      return bindings.get(ctx);
    }

    // Bind the symbol to the node
    void bind(antlr4::ParserRuleContext *ctx, Symbol* symbol) {
      bindings.put(ctx, symbol);
    }

  private:
    antlr4::tree::ParseTreeProperty<Symbol*> bindings;
};