#include "CodegenVisitor.h"

std::optional<Value *> CodegenVisitor::TvisitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    for (auto e : ctx->defs)
    {
        // FIXME: ENSURE CATCH ALL?
        e->accept(this);
    }

    for (auto e : ctx->extens)
    {
        e->accept(this);
    }

    // Pre-declare all functions //FIXME: VERIFY
    for (auto e : ctx->stmts)
    {
        if (WPLParser::FuncDefContext *fnCtx = dynamic_cast<WPLParser::FuncDefContext *>(e))
        {
            // FIXME: VERIFY SWITCH OVER TO PROC AND REM EXTERN, METHODIZE, ALSO, EDIT INVOKE GEN ELSEWHERE TO REFLECT THIS
            std::optional<Symbol *> optSym = props->getBinding(fnCtx);

            if (!optSym)
            {
                errorHandler.addCodegenError(fnCtx->getStart(), "Incorrectly bound symbol in function definition. Probably a compiler error.");
                return {};
            }

            Symbol *symbol = optSym.value();

            if (!symbol->type)
            {
                errorHandler.addCodegenError(fnCtx->getStart(), "Type for function not correctly bound! Probably a compiler errror.");
                return {};
            }

            const Type *generalType = symbol->type;

            if (const TypeInvoke *type = dynamic_cast<const TypeInvoke *>(generalType))
            {
                llvm::Type *genericType = type->getLLVMType(module)->getPointerElementType();

                if (llvm::FunctionType *fnType = static_cast<llvm::FunctionType *>(genericType))
                {
                    Function::Create(fnType, GlobalValue::ExternalLinkage, fnCtx->name->getText(), module);
                }
                else
                {
                    errorHandler.addCodegenError(fnCtx->getStart(), "Could not treat function type as function.");
                    return {};
                }
            }
            else
            {
                errorHandler.addCodegenError(fnCtx->getStart(), "Function bound to: " + generalType->toString() + ". Requires Invokable!");
            }
        }
    }

    for (auto e : ctx->stmts)
    {
        // Generate code for statement
        e->accept(this);
    }

    /*******************************************
     * Extra checks depending on compiler flags
     *******************************************/

    if (flags & CompilerFlags::NO_RUNTIME)
    {
        /*
         * Need to create main method and invoke program()
         * Based on semantic analysis, both of these should be defined.
         *
         * This will segfault if not found, but, as stated, that should be impossible.
         */

        FunctionType *mainFuncType = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
        Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main", module);

        // Create block to attach to main
        BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", mainFunc);
        builder->SetInsertPoint(bBlk);

        llvm::Function *progFn = module->getFunction("program");
        builder->CreateRet(builder->CreateCall(progFn, {}));
    }

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitDefineEnum(WPLParser::DefineEnumContext *ctx)
{
    std::optional<Symbol *> symOpt = props->getBinding(ctx); // FIXME: THIS DOES NOTHING
    if (!symOpt)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Could not locate symbol for enum defintion: " + ctx->name->getText());
        return {};
    }

    Symbol *sym = symOpt.value();

    llvm::Type *ty = sym->type->getLLVMType(module);

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitMatchStatement(WPLParser::MatchStatementContext *ctx)
{
    std::optional<Symbol *> symOpt = props->getBinding(ctx->check); // FIXME: THIS DOES NOTHING
    if (!symOpt)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Could not locate symbol for case");
        return {};
    }

    if (const TypeSum *sumType = dynamic_cast<const TypeSum *>(symOpt.value()->type))
    {
        auto origParent = builder->GetInsertBlock()->getParent();
        BasicBlock *mergeBlk = BasicBlock::Create(module->getContext(), "matchcont");

        std::any anyCheck = ctx->check->accept(this);

        // Attempt to cast the check; if this fails, then codegen for the check failed //FIXME: VERIFY THESE ARE FINE
        if (std::optional<Value *> optVal = std::any_cast<std::optional<Value *>>(anyCheck))
        {
            // Check that the optional, in fact, has a value. Otherwise, something went wrong.
            if (!optVal)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->check->getText());
                return {};
            }

            Value *sumVal = optVal.value();
            llvm::AllocaInst *SumPtr = builder->CreateAlloca(sumVal->getType());
            builder->CreateStore(sumVal, SumPtr);

            // Value *corrPtr = builder->CreateBitCast(sumVal, sumVal->getType()->getPointerTo()); // FIXME: DO BETTER

            Value *tagPtr = builder->CreateGEP(SumPtr, {Int32Zero, Int32Zero});

            Value *tag = builder->CreateLoad(tagPtr->getType()->getPointerElementType(), tagPtr);

            llvm::SwitchInst *switchInst = builder->CreateSwitch(tag, mergeBlk, sumType->getCases().size());

            for (WPLParser::MatchAlternativeContext *altCtx : ctx->cases)
            {
                std::optional<Symbol *> localSymOpt = props->getBinding(altCtx->VARIABLE());

                if (!localSymOpt)
                {
                    errorHandler.addCodegenError(altCtx->getStart(), "Failed to lookup type for case");
                    return {};
                }

                llvm::Type *toFind = localSymOpt.value()->type->getLLVMType(module); // FIXME: DO THIS ALL BETTER, MORE CHECKS!!

                // FIXME: METHODIZE!!
                unsigned int index = [sumType, toFind](llvm::Module *M)
                {
                    unsigned i = 1;

                    for (auto e : sumType->getCases())
                    {
                        if (e->getLLVMType(M) == toFind)
                        {
                            return i;
                        }
                        i++;
                    }

                    return (unsigned int)0;
                }(module);

                if (index == 0)
                {
                    errorHandler.addCodegenError(ctx->getStart(), "Unable to find key for type " + localSymOpt.value()->type->toString() + " in sum");
                    return {}; // FIXME: DO BETTER!
                }

                BasicBlock *matchBlk = BasicBlock::Create(module->getContext(), "tagBranch" + std::to_string(index));

                builder->SetInsertPoint(matchBlk);

                // FIXME: VERIFY SIGNED VS UNSIGNED
                switchInst->addCase(ConstantInt::get(Int32Ty, index, true), matchBlk);
                origParent->getBasicBlockList().push_back(matchBlk);

                // FIME: METHODIZE VAR DECL?

                std::optional<Symbol *> varSymbolOpt = props->getBinding(altCtx->VARIABLE());

                if (!varSymbolOpt)
                {
                    errorHandler.addCodegenError(altCtx->getStart(), "Failed to find symbol in match");
                    return {}; // FIXME: SHOULD WE RETURN OR WOULD THIS BREAK STUFF?
                }

                Symbol *varSymbol = varSymbolOpt.value();

                // FIXME: test for varSymbol->type?
                //  Get the type of the symbol
                llvm::Type *ty = varSymbol->type->getLLVMType(module);

                // if (dynamic_cast<const TypeInvoke *>(varSymbol->type))
                // {
                //     ty = ty->getPointerTo();
                // }

                // Can skip global stuff
                llvm::AllocaInst *v = builder->CreateAlloca(ty, 0, altCtx->VARIABLE()->getText());
                varSymbol->val = v;
                // varSymbol->val = v;

                // Now to store the var
                Value *valuePtr = builder->CreateGEP(SumPtr, {Int32Zero, Int32One});

                Value *corrected = builder->CreateBitCast(valuePtr, ty->getPointerTo()); // FIXME: DO BETTER

                Value *val = builder->CreateLoad(ty, corrected); // FIXME: WILL THIS WORK WITH NESTED SUMS?

                builder->CreateStore(val, v);

                // FIXME: DEFINE AND BIND THE VAR!!!
                altCtx->eval->accept(this);

                if (WPLParser::BlockStatementContext *blkStmtCtx = dynamic_cast<WPLParser::BlockStatementContext *>(altCtx->eval))
                {
                    WPLParser::BlockContext *blkCtx = blkStmtCtx->block();
                    if (!CodegenVisitor::blockEndsInReturn(blkCtx))
                    {
                        builder->CreateBr(mergeBlk);
                    }
                    // if it ends in a return, we're good!
                }
                else if (WPLParser::ReturnStatementContext *retCtx = dynamic_cast<WPLParser::ReturnStatementContext *>(altCtx->eval))
                {
                    // Similarly, we don't need to generate the branch
                }
                else
                {
                    builder->CreateBr(mergeBlk);
                }
            }
        }

        origParent->getBasicBlockList().push_back(mergeBlk);
        builder->SetInsertPoint(mergeBlk);

        return {};
    }

    errorHandler.addCodegenError(ctx->getStart(), "Failed to lookup type for case");

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitInvocation(WPLParser::InvocationContext *ctx)
{
    // Create the argument vector
    std::vector<llvm::Value *> args;

    // Populate the argument vector, breaking out of compilation if any argument fails to generate.
    for (auto e : ctx->args)
    {
        std::optional<Value *> valOpt = std::any_cast<std::optional<Value *>>(e->accept(this));
        if (!valOpt)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code");
            return {};
        }
        args.push_back(valOpt.value());
    }

    // Convert to an array ref, then find and execute the call.
    ArrayRef<Value *> ref = ArrayRef(args);
    if (ctx->lam)
    {
        std::optional<Value *> callOpt = TvisitLambdaConstExpr(ctx->lam);
        if (!callOpt)
        {
            errorHandler.addCodegenError(ctx->lam->getStart(), "Could not generate code for lambda");
            return {};
        }
        llvm::Function *call = (llvm::Function *)callOpt.value();
        Value *val = builder->CreateCall(call, ref); // Needs to be separate line because, C++
        return val;
    }

    llvm::Function *call = module->getFunction(ctx->VARIABLE()->getText());

    if (!call)
    {
        std::optional<Symbol *> symOpt = props->getBinding(ctx);
        if (!symOpt)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Could not locate function for invocation: " + ctx->VARIABLE()->getText() + ". Has it been defined in IR yet?");
            return {};
        }

        Symbol *sym = symOpt.value();

        std::optional<llvm::Value *> fnPtrOpt = sym->val;

        // FIXME: WILL NOT WORK FOR GLOBAL
        if (!fnPtrOpt)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Could not find value for function: " + sym->toString());
            return {};
        }
        // FIXME: TEST GLOBAL LAMBDAS!!!

        llvm::Type *ty = sym->type->getLLVMType(module);

        if (dynamic_cast<const TypeInvoke *>(sym->type))
        {
            llvm::FunctionType *fnType = static_cast<llvm::FunctionType *>(ty->getPointerElementType());
            llvm::Value *fnPtr = fnPtrOpt.value();

            llvm::Value *fn = builder->CreateLoad(fnPtr);

            Value *val = builder->CreateCall(fnType, fn, ref);
            // llvm::FunctionCallee *callee = new llvm::FunctionCallee(fnType, fnPtr);

            // Value *val = builder->CreateCall(*callee, ref); // Needs to be separate line because, C++
            return val;
        }

        errorHandler.addCodegenError(ctx->getStart(), "Invoke got non-function type: " + ctx->VARIABLE()->getText() + " : " + sym->type->toString());
        // errorHandler.addCodegenError(ctx->getStart(), "Could not locate function for invocation: " + ctx->VARIABLE()->getText() + ". Has it been defined in IR yet?");
        return {};
    }

    Value *val = builder->CreateCall(call, ref); // Needs to be separate line because, C++
    return val;
}

std::optional<Value *> CodegenVisitor::TvisitInitProduct(WPLParser::InitProductContext *ctx)
{
    std::vector<Value *> args;

    for (auto e : ctx->exprs)
    {
        std::optional<Value *> valOpt = std::any_cast<std::optional<Value *>>(e->accept(this));
        if (!valOpt)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code");
            return {};
        }

        // std::optional<Symbol *> optSym = props->getBinding(e);
        // if (!optSym)
        // {
        //     errorHandler.addCodegenError(ctx->getStart(), "Failed to get binding");
        //     return {};
        // }

        // Symbol *varSym = optSym.value();
        Value *stoVal = valOpt.value();

        // FIXME: TRY PASSING GLOBAL ARG INTO FN

        args.push_back(stoVal);
    }

    std::optional<Symbol *> varSymOpt = props->getBinding(ctx);
    if (!varSymOpt)
    {
        // FIXME: DO BETTER
        errorHandler.addCodegenError(ctx->getStart(), "Incorrectly processed variable in assignment: " + ctx->getText());
        return {};
    }

    Symbol *varSym = varSymOpt.value();

    if (const TypeStruct *product = dynamic_cast<const TypeStruct *>(varSym->type))
    {
        llvm::Type *ty = varSym->type->getLLVMType(module);
        llvm::AllocaInst *v = builder->CreateAlloca(ty, 0, ""); // FIXME: VERIFY
        {
            unsigned i = 0;
            std::vector<std::pair<std::string, const Type *>> elements = product->getElements();

            for (Value *a : args)
            {
                if (const TypeSum *sum = dynamic_cast<const TypeSum *>(elements.at(i).second))
                {
                    // FIXME: METHODIZE!!
                    unsigned int index = [sum, a](llvm::Module *M)
                    {
                        unsigned i = 1;
                        auto toFind = a->getType();
                        for (auto e : sum->getCases())
                        {
                            if (e->getLLVMType(M) == toFind)
                            {
                                return i;
                            }
                            i++;
                        }

                        return (unsigned int)0;
                    }(module);
                    
                    if (index != 0)
                    {
                        llvm::Type * sumTy = sum->getLLVMType(module);
                        llvm::AllocaInst *alloc = builder->CreateAlloca(sumTy, 0, "XK");
                        Value *tagPtr = builder->CreateGEP(alloc, {Int32Zero, Int32Zero});
                        builder->CreateStore(ConstantInt::get(Int32Ty, index, true), tagPtr);
                        Value *valuePtr = builder->CreateGEP(alloc, {Int32Zero, Int32One});
                        Value *corrected = builder->CreateBitCast(valuePtr, a->getType()->getPointerTo()); // FIXME: DO BETTER
                        builder->CreateStore(a, corrected);
                        a = builder->CreateLoad(sumTy, alloc);
                        // a = alloc;
                    }
                }

                // Value*  valIndex =
                Value *ptr = builder->CreateGEP(v, {Int32Zero, ConstantInt::get(Int32Ty, i, true)});
                // Value * corrected = builder->CreateBitCast(ptr, a->getType()->getPointerTo());
                builder->CreateStore(a, ptr);

                i++;
            }
        }

        // return v; //FIXME: DO BETTER?
        Value *loaded = builder->CreateLoad(v->getType()->getPointerElementType(), v);
        return loaded;
    }

    errorHandler.addCodegenError(ctx->getStart(), "Failed to gen init"); // FIXME: DO BETTER
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitArrayAccess(WPLParser::ArrayAccessContext *ctx)
{
    // Attempt to get the index Value
    std::optional<Value *> index = std::any_cast<std::optional<Value *>>(ctx->index->accept(this));

    if (!index)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code in TvisitArrayAccess for index!");
        return {};
    }

    std::optional<Symbol *> symOpt = props->getBinding(ctx);

    // If the symbol could not be found, raise an error
    if (!symOpt)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Undefined symbol in array access");
        return {};
    }

    Symbol *sym = symOpt.value();

    // Create the expression pointer
    std::optional<llvm::Value *> arrayPtr = sym->val;

    if (!arrayPtr)
    {
        if (sym->isGlobal)
        {
            // Lookup the global var for the symbol
            llvm::GlobalVariable *glob = module->getNamedGlobal(sym->identifier);

            // Check that we found the variable. If not, throw an error.
            if (!glob)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Unable to find global variable: " + sym->identifier);
                return {};
            }

            arrayPtr = builder->CreateLoad(glob)->getPointerOperand();

            // auto ptr = builder->CreateGEP(v, {Int32Zero, index.value()});
            // Value *val = builder->CreateLoad(ptr->getType()->getPointerElementType(), ptr);
            // return val;
        }
        else
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to locate array in access");
            return {};
        }
    }

    // FIXME: CAN WE BREAK ARRAYS DUE TO THE ACCESS OF THE ALLOC VALUE?
    auto ptr = builder->CreateGEP(arrayPtr.value(), {Int32Zero, index.value()});
    Value *val = builder->CreateLoad(ptr->getType()->getPointerElementType(), ptr);
    return val;
}

std::optional<Value *> CodegenVisitor::TvisitIConstExpr(WPLParser::IConstExprContext *ctx)
{
    int i = std::stoi(ctx->i->getText());
    Value *v = builder->getInt32(i);
    return v;
}

std::optional<Value *> CodegenVisitor::TvisitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx)
{
    return this->TvisitArrayAccess(ctx->arrayAccess());
}

std::optional<Value *> CodegenVisitor::TvisitSConstExpr(WPLParser::SConstExprContext *ctx)
{
    // TODO: do this better, ensure that we can only escape these chars...
    std::string full(ctx->s->getText());
    std::string actual = full.substr(1, full.length() - 2);

    std::vector<std::pair<std::regex, std::string>> replacements;

    // Reference of all escape characters: https://en.cppreference.com/w/cpp/language/escape
    std::regex SQ("\\\\'");
    std::regex DQ("\\\\\"");
    std::regex QM("\\\\\\?");
    std::regex SL("\\\\\\\\");
    std::regex AB("\\\\a");
    std::regex BS("\\\\b");
    std::regex FF("\\\\f");
    std::regex NL("\\\\n");
    std::regex CR("\\\\r");
    std::regex HT("\\\\t");
    std::regex VT("\\\\v");

    replacements.push_back({SQ, "\'"});
    replacements.push_back({DQ, "\""});
    replacements.push_back({QM, "\?"});
    replacements.push_back({AB, "\a"});
    replacements.push_back({BS, "\b"});
    replacements.push_back({FF, "\f"});
    replacements.push_back({NL, "\n"});
    replacements.push_back({CR, "\r"});
    replacements.push_back({HT, "\t"});
    replacements.push_back({VT, "\v"});
    replacements.push_back({SL, "\\"});

    std::string out = actual;

    for (auto e : replacements)
    {
        out = regex_replace(out, e.first, e.second);
    }

    // Create a constant to represent our string (now with the escape characters corrected)
    llvm::Constant *dat = llvm::ConstantDataArray::getString(module->getContext(), out);

    // Allocate a global variable for the constant, and set flags to make it match what the CreateGlobalStringPtr function would have done
    llvm::GlobalVariable *glob = new llvm::GlobalVariable(
        *module,
        dat->getType(),
        true,
        GlobalValue::PrivateLinkage,
        dat,
        "");
    glob->setAlignment(llvm::MaybeAlign(1));
    glob->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);

    // Allocate the string and return that value.
    // This prevents the issue of CreateGlobalStringPtr where it creates a string AND a pointer to it.
    // Here, we can deal with the pointer later (just as if it were a normal variable)
    llvm::Constant *Indices[] = {Int32Zero, Int32Zero};

    Value *val = llvm::ConstantExpr::getInBoundsGetElementPtr(
        glob->getValueType(),
        glob,
        Indices);

    return val;
}

std::optional<Value *> CodegenVisitor::TvisitUnaryExpr(WPLParser::UnaryExprContext *ctx)
{
    switch (ctx->op->getType())
    {
    case WPLParser::MINUS:
    {
        std::optional<Value *> innerVal = std::any_cast<std::optional<Value *>>(ctx->ex->accept(this));

        if (!innerVal)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
            return {};
        }

        Value *v = builder->CreateNSWSub(builder->getInt32(0), innerVal.value());
        return v;
    }

    case WPLParser::NOT:
    {
        std::optional<Value *> v = std::any_cast<std::optional<Value *>>(ctx->ex->accept(this));

        if (!v)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
            return {};
        }

        v = builder->CreateNot(v.value());
        return v;
    }
    }

    errorHandler.addCodegenError(ctx->getStart(), "Unknown unary operator: " + ctx->op->getText());
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx)
{
    std::optional<Value *> lhs = std::any_cast<std::optional<Value *>>(ctx->left->accept(this));
    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

    if (!lhs || !rhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
        return {};
    }

    switch (ctx->op->getType())
    {
    case WPLParser::PLUS:
        return builder->CreateNSWAdd(lhs.value(), rhs.value());
    case WPLParser::MINUS:
        return builder->CreateNSWSub(lhs.value(), rhs.value());
    case WPLParser::MULTIPLY:
        return builder->CreateNSWMul(lhs.value(), rhs.value());
    case WPLParser::DIVIDE:
        return builder->CreateSDiv(lhs.value(), rhs.value());
    }

    errorHandler.addCodegenError(ctx->getStart(), "Unknown arith op: " + ctx->op->getText());
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitEqExpr(WPLParser::EqExprContext *ctx)
{
    std::optional<Value *> lhs = std::any_cast<std::optional<Value *>>(ctx->left->accept(this));
    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

    if (!lhs || !rhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
        return {};
    }

    switch (ctx->op->getType())
    {
    case WPLParser::EQUAL:
    {
        Value *v1 = builder->CreateICmpEQ(lhs.value(), rhs.value());
        Value *v = builder->CreateZExtOrTrunc(v1, Int1Ty);
        return v;
    }

    case WPLParser::NOT_EQUAL:
    {
        Value *v1 = builder->CreateICmpNE(lhs.value(), rhs.value());
        Value *v = builder->CreateZExtOrTrunc(v1, Int1Ty);
        return v;
    }
    }

    errorHandler.addCodegenError(ctx->getStart(), "Unknown equality operator: " + ctx->op->getText());
    return {};
}

/**
 * @brief Generates code for Logical Ands
 *
 * Tested in: test2.wpl
 *
 * @param ctx LogAndExprContext to generate this from
 * @return std::optional<Value *> The resulting value or {} if errors.
 */
std::optional<Value *> CodegenVisitor::TvisitLogAndExpr(WPLParser::LogAndExprContext *ctx)
{
    // FIXME: DO BETTER W/ AST
    std::vector<WPLParser::ExpressionContext *> toVisit = ctx->exprs;
    std::vector<WPLParser::ExpressionContext *> toGen;

    while (toVisit.size() > 0)
    {
        WPLParser::ExpressionContext *curr = toVisit.at(0);
        toVisit.erase(toVisit.begin());

        if (WPLParser::LogAndExprContext *orCtx = dynamic_cast<WPLParser::LogAndExprContext *>(curr))
        {
            toVisit.insert(toVisit.end(), orCtx->exprs.begin(), orCtx->exprs.end());
        }
        else
        {
            toGen.push_back(curr);
        }
    }

    // Create the basic block for our conditions
    BasicBlock *current = builder->GetInsertBlock();
    BasicBlock *mergeBlk = BasicBlock::Create(module->getContext(), "mergeBlkAnd");

    /*
     * PHI node to merge both sides back together
     */
    builder->SetInsertPoint(mergeBlk);
    PHINode *phi = builder->CreatePHI(Int1Ty, toGen.size(), "logAnd");

    builder->SetInsertPoint(current);

    std::optional<Value *> first = std::any_cast<std::optional<Value *>>(toGen.at(0)->accept(this));

    if (!first)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + toGen.at(0)->getText());
        return {};
    }

    Value *lastValue = first.value();

    auto parent = current->getParent();
    phi->addIncoming(lastValue, current);

    BasicBlock *falseBlk;

    // Branch on the lhs value
    for (unsigned int i = 1; i < toGen.size(); i++)
    {
        falseBlk = BasicBlock::Create(module->getContext(), "prevTrueAnd", parent);
        builder->CreateCondBr(lastValue, falseBlk, mergeBlk);

        /*
         * LHS False - Need to check RHS value
         */
        builder->SetInsertPoint(falseBlk);

        std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(toGen.at(i)->accept(this));

        if (!rhs)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + toGen.at(i)->getText());
            return {};
        }
        lastValue = rhs.value();

        falseBlk = builder->GetInsertBlock();
        parent = falseBlk->getParent();
        phi->addIncoming(lastValue, falseBlk);
    }

    builder->CreateBr(mergeBlk);
    // falseBlk = builder->GetInsertBlock();

    /*
     * LHS True - Can skip checking RHS and return true
     */
    parent->getBasicBlockList().push_back(mergeBlk);
    builder->SetInsertPoint(mergeBlk);

    // phi->addIncoming(rhs.value(), falseBlk);
    return phi;
}

/**
 * @brief Generates code for Logical Ors.
 *
 * Tested in: test2.wpl
 *
 * @param ctx Context to generate code from
 * @return std::optional<Value *> The resulting value or {} if errors.
 */
std::optional<Value *> CodegenVisitor::TvisitLogOrExpr(WPLParser::LogOrExprContext *ctx)
{
    // FIXME: DO BETTER W/ AST
    std::vector<WPLParser::ExpressionContext *> toVisit = ctx->exprs;
    std::vector<WPLParser::ExpressionContext *> toGen;

    while (toVisit.size() > 0)
    {
        WPLParser::ExpressionContext *curr = toVisit.at(0);
        toVisit.erase(toVisit.begin());

        if (WPLParser::LogOrExprContext *orCtx = dynamic_cast<WPLParser::LogOrExprContext *>(curr))
        {
            toVisit.insert(toVisit.end(), orCtx->exprs.begin(), orCtx->exprs.end());
        }
        else
        {
            toGen.push_back(curr);
        }
    }

    // Create the basic block for our conditions
    BasicBlock *current = builder->GetInsertBlock();
    BasicBlock *mergeBlk = BasicBlock::Create(module->getContext(), "mergeBlkOr");

    /*
     * PHI node to merge both sides back together
     */
    builder->SetInsertPoint(mergeBlk);
    PHINode *phi = builder->CreatePHI(Int1Ty, toGen.size(), "logOr");

    builder->SetInsertPoint(current);

    std::optional<Value *> first = std::any_cast<std::optional<Value *>>(toGen.at(0)->accept(this));

    if (!first)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + toGen.at(0)->getText());
        return {};
    }

    Value *lastValue = first.value();

    auto parent = current->getParent();
    phi->addIncoming(lastValue, current);

    BasicBlock *falseBlk;

    // Branch on the lhs value
    for (unsigned int i = 1; i < toGen.size(); i++)
    {
        falseBlk = BasicBlock::Create(module->getContext(), "prevFalseOr", parent);
        builder->CreateCondBr(lastValue, mergeBlk, falseBlk);

        /*
         * LHS False - Need to check RHS value
         */
        builder->SetInsertPoint(falseBlk);

        std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(toGen.at(i)->accept(this));

        if (!rhs)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + toGen.at(i)->getText());
            return {};
        }
        lastValue = rhs.value();

        falseBlk = builder->GetInsertBlock();
        parent = falseBlk->getParent();
        phi->addIncoming(lastValue, falseBlk);
    }

    builder->CreateBr(mergeBlk);

    /*
     * LHS True - Can skip checking RHS and return true
     */
    parent->getBasicBlockList().push_back(mergeBlk);
    builder->SetInsertPoint(mergeBlk);

    // phi->addIncoming(rhs.value(), falseBlk);
    return phi;
}

// Passthrough to TvisitInvocation
std::optional<Value *> CodegenVisitor::TvisitCallExpr(WPLParser::CallExprContext *ctx) { return this->TvisitInvocation(ctx->call); }

std::optional<Value *> CodegenVisitor::TvisitVariableExpr(WPLParser::VariableExprContext *ctx)
{
    // Get the variable name and look it up
    std::string id = ctx->v->getText();
    return this->visitVariable(id, props->getBinding(ctx), ctx);
}

std::optional<Value *> CodegenVisitor::TvisitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx)
{
    // This is ONLY array length for now...

    // Make sure we cna find the symbol, and that it has a val and type defined
    std::optional<Symbol *> symOpt = props->getBinding(ctx->VARIABLE().at(0));

    if (!symOpt)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized array access symbol.");
        return {};
    }

    Symbol *sym = symOpt.value();

    if (!symOpt || !sym->val || !sym->type)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized array access symbol.");
        return {};
    }

    if (ctx->fields.at(ctx->fields.size() - 1)->getText() == "length")
    {
        // std::optional<Symbol *> finalOpt = props->getBinding(ctx->VARIABLE().at(ctx->VARIABLE().size() - 1));

        // if(finalOpt && finalOpt.value()->type == Types::INT) //FIXME: DO
        // {

        // }
        std::optional<Symbol *> modOpt = props->getBinding(ctx->VARIABLE().at(ctx->VARIABLE().size() - 2));

        if (modOpt)
        {
            if (const TypeArray *ar = dynamic_cast<const TypeArray *>(modOpt.value()->type))
            {
                // FIXME: VERIFY THIS STILL WORKS WHEN NESTED!
                // If it is, correctly, an array type, then we can get the array's length (this is the only operation currently, so we can just do thus)
                Value *v = builder->getInt32(ar->getLength());

                return v; // FIXME: GOING TO HAVE TO CHANGE THIS, WON'T WORK NOW WITH ITERATIONS!
            }
        }
    }

    const Type *ty = sym->type;
    std::optional<Value *> baseOpt = visitVariable(ctx->VARIABLE().at(0)->getText(), props->getBinding(ctx->VARIABLE().at(0)), ctx);
    // std::optional<Value *> val = {};

    for (unsigned int i = 1; i < ctx->fields.size(); i++)
    {
        if (const TypeStruct *s = dynamic_cast<const TypeStruct *>(ty))
        {
            if (!baseOpt)
            {
                errorHandler.addCodegenError(ctx->getStart(), "FIXME");
                return {};
            }

            std::string field = ctx->fields.at(i)->getText(); // FIXME: LOOP
            std::optional<unsigned int> indexOpt = [s, field]()
            {
                unsigned int i = 0;
                for (auto e : s->getElements())
                {
                    if (e.first == field) // FIXME: DO BETTER CONSIDERING WE HAVE MAPS
                    {
                        return std::optional<unsigned int>{i};
                    }
                    i++;
                }
                std::optional<unsigned int> ret = {};
                return ret;
            }();

            if (!indexOpt)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Could not lookup " + field);
                return {};
            }

            unsigned int index = indexOpt.value();

            std::optional<Symbol *> fieldOpt = props->getBinding(ctx->VARIABLE().at(i));

            if (!fieldOpt)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Could not get binding for " + field);
                return {};
            }

            Value *baseValue = baseOpt.value();

            Symbol *fieldSym = fieldOpt.value();
            llvm::AllocaInst *v = builder->CreateAlloca(baseValue->getType()); //(fieldSym->type->getLLVMType(module), 0, "");
            builder->CreateStore(baseValue, v);
            Value *valPtr = builder->CreateGEP(v, {Int32Zero, ConstantInt::get(Int32Ty, index, true)});

            llvm::Type *ansType = fieldSym->type->getLLVMType(module);

            ty = fieldSym->type;
            baseOpt = builder->CreateLoad(ansType, valPtr);
            // return val;
        }
    }

    return baseOpt.value(); // FIXME: DO BETTER, VERIFY CORRECT

    // // Throw an error as we currently only support array length.
    // errorHandler.addCodegenError(ctx->getStart(), "Given non-array type in TvisitFieldAccessExpr!");
    // return {};
}

std::optional<Value *> CodegenVisitor::TvisitParenExpr(WPLParser::ParenExprContext *ctx)
{
    return std::any_cast<std::optional<Value *>>(ctx->ex->accept(this));
}

std::optional<Value *> CodegenVisitor::TvisitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx)
{
    // Generate code for LHS and RHS
    std::optional<Value *> lhs = std::any_cast<std::optional<Value *>>(ctx->left->accept(this));
    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

    // Ensure we successfully generated LHS and RHS
    if (!lhs || !rhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
        return {};
    }

    Value *v1;

    switch (ctx->op->getType())
    {
    case WPLParser::LESS:
        v1 = builder->CreateICmpSLT(lhs.value(), rhs.value());
        break;
    case WPLParser::LESS_EQ:
        v1 = builder->CreateICmpSLE(lhs.value(), rhs.value());
        break;
    case WPLParser::GREATER:
        v1 = builder->CreateICmpSGT(lhs.value(), rhs.value());
        break;
    case WPLParser::GREATER_EQ:
        v1 = builder->CreateICmpSGE(lhs.value(), rhs.value());
        break;

    default:
        errorHandler.addCodegenError(ctx->getStart(), "Unknown rel operator: " + ctx->op->getText());
        return {};
    }

    Value *v = builder->CreateZExtOrTrunc(v1, Int1Ty);
    return v;
}

// Passthrough to TvisitBooleanConst
std::optional<Value *> CodegenVisitor::TvisitBConstExpr(WPLParser::BConstExprContext *ctx) { return this->TvisitBooleanConst(ctx->booleanConst()); }

std::optional<Value *> CodegenVisitor::TvisitCondition(WPLParser::ConditionContext *ctx)
{
    // Passthrough to visiting the conditon
    return std::any_cast<std::optional<Value *>>(ctx->ex->accept(this));
}

std::optional<Value *> CodegenVisitor::TvisitExternStatement(WPLParser::ExternStatementContext *ctx)
{
    std::optional<Symbol *> optSym = props->getBinding(ctx);

    if (!optSym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Incorrectly bound symbol in extern statement. Probably a compiler error.");
        return {};
    }

    Symbol *symbol = optSym.value();

    if (!symbol->type)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Type for extern statement not correctly bound! Probably a compiler errror.");
        return {};
    }

    const Type *generalType = symbol->type;

    if (const TypeInvoke *type = dynamic_cast<const TypeInvoke *>(generalType))
    {
        llvm::Type *genericType = type->getLLVMType(module)->getPointerElementType();

        if (llvm::FunctionType *fnType = static_cast<llvm::FunctionType *>(genericType))
        {
            Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);
        }
        else
        {
            errorHandler.addCodegenError(ctx->getStart(), "Could not treat extern type as function.");
            return {};
        }
    }
    else
    {
        errorHandler.addCodegenError(ctx->getStart(), "Extern statement bound to: " + generalType->toString() + ". Requires Invokable!");
    }

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitFuncDef(WPLParser::FuncDefContext *ctx)
{
    return CodegenVisitor::visitInvokeable(ctx);
}

std::optional<Value *> CodegenVisitor::TvisitAssignStatement(WPLParser::AssignStatementContext *ctx)
{
    // Visit the expression to get the value we will assign
    std::optional<Value *> exprVal = std::any_cast<std::optional<Value *>>(ctx->ex->accept(this));

    // Check that the expression generated
    if (!exprVal)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->ex->getText());
        return {};
    }

    // Lookup the binding for the variable we are assigning to and and ensure that we find it
    std::optional<Symbol *> varSymOpt = props->getBinding(ctx->to);
    if (!varSymOpt)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Incorrectly processed variable in assignment: " + ctx->to->getText());
        return {};
    }

    Symbol *varSym = varSymOpt.value();

    // Get the allocation instruction for the symbol
    std::optional<Value *> val = varSym->val;

    // If the symbol is global
    if (varSym->isGlobal)
    {
        // Find the global variable that corresponds to our symbol
        llvm::GlobalVariable *glob = module->getNamedGlobal(varSym->identifier);

        // If we can't find it, then throw an error.
        if (!glob)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Unable to find global variable: " + varSym->identifier);
            return {};
        }

        // Load a pointer to the global variable
        val = builder->CreateLoad(glob)->getPointerOperand();
    }

    // Sanity check to ensure that we now have a value for the variable
    if (!val)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized variable in assignment: " + ctx->to->getText() + "@" + varSym->identifier);
        return {};
    }

    // Checks to see if we are dealing with an array
    if (!ctx->to->VARIABLE())
    {
        // As this is an array access, we need to determine the index we will be accessing
        std::optional<Value *> index = std::any_cast<std::optional<Value *>>(ctx->to->array->index->accept(this));

        // Ensure we built an index
        if (!index)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->to->getText());
            return {};
        }

        // Create a GEP to the index based on our previously calculated value and index
        Value *built = builder->CreateGEP(val.value(), {Int32Zero, index.value()});
        val = built;
    }

    // Store the expression's value
    // FIXME: METHODIZE?
    Value *v = val.value();
    Value *stoVal = exprVal.value();
    if (const TypeSum *sum = dynamic_cast<const TypeSum *>(varSym->type))
    {
        // FIXME: METHODIZE!!
        unsigned int index = [sum, stoVal](llvm::Module *M)
        {
            unsigned i = 1;
            auto toFind = stoVal->getType();
            for (auto e : sum->getCases())
            {
                if (e->getLLVMType(M) == toFind)
                {
                    return i;
                }
                i++;
            }

            return (unsigned int)0;
        }(module);

        if (index == 0)
        {
            Value *corrected = builder->CreateBitCast(stoVal, varSym->type->getLLVMType(module));
            builder->CreateStore(corrected, v);
            // errorHandler.addCodegenError(ctx->getStart(), "Unable to find key for type in sum");
            return {}; // FIXME: DO BETTER!
        }

        Value *tagPtr = builder->CreateGEP(v, {Int32Zero, Int32Zero});

        builder->CreateStore(ConstantInt::get(Int32Ty, index, true), tagPtr);
        Value *valuePtr = builder->CreateGEP(v, {Int32Zero, Int32One});

        Value *corrected = builder->CreateBitCast(valuePtr, stoVal->getType()->getPointerTo()); // FIXME: DO BETTER
        builder->CreateStore(stoVal, corrected);
    }
    else
    {
        builder->CreateStore(stoVal, v);
    }

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    /*
     * Visit each of the assignments in the context (variables paired with an expression)
     */
    for (auto e : ctx->assignments)
    {

        std::optional<Value *> exVal = std::nullopt;

        // If the declaration has a value, attempt to generate that value
        if (e->ex)
        {
            std::any anyVal = e->ex->accept(this);

            if (std::optional<Value *> opt = std::any_cast<std::optional<Value *>>(anyVal))
            {
                exVal = opt;
            }
            else
            {
                errorHandler.addCodegenError(e->ex->getStart(), "Could not generate code for: " + e->ex->getText());
                return {};
            }
        }

        if ((e->ex) && !exVal)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + e->ex->getText());
            return {};
        }

        // For each of the variabes being assigned to that value
        for (auto var : e->VARIABLE())
        {

            // Get the Symbol for the var based on its binding
            std::optional<Symbol *> varSymbolOpt = props->getBinding(var);

            if (!varSymbolOpt)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Issue creating variable: " + var->getText());
                return {};
            }

            Symbol *varSymbol = varSymbolOpt.value();

            // FIXME: test for varSymbol->type?
            //  Get the type of the symbol
            llvm::Type *ty = varSymbol->type->getLLVMType(module);

            ty = varSymbol->type->getLLVMType(module);

            // if (dynamic_cast<const TypeInvoke *>(varSymbol->type))
            // {
            //     ty = ty->getPointerTo();
            // }

            // Branch depending on if the var is global or not
            if (varSymbol->isGlobal)
            {
                // If it is global, then we need to insert a new gobal variable of this type.
                // A lot of these options are done to make it match what a C program would
                // generate for global vars
                module->getOrInsertGlobal(var->getText(), ty);
                llvm::GlobalVariable *glob = module->getNamedGlobal(var->getText());
                glob->setLinkage(GlobalValue::ExternalLinkage);
                glob->setDSOLocal(true);

                // If we had an expression to set the var equal to
                if (e->ex)
                {
                    // Ensure that the value is a constant, then, if so, initialize it.
                    if (llvm::Constant *constant = static_cast<llvm::Constant *>(exVal.value()))
                    {
                        glob->setInitializer(constant);
                    }
                    else
                    {
                        // Should already be checked in semantic, and I don't think we could get here anyways, but still might as well have it.
                        errorHandler.addCodegenError(ctx->getStart(), "Global variable can only be initalized to a constant!");
                        return {};
                    }
                }
                else
                {
                    // As there was no constant, just set the global var to be initalized to zero as C does with llvm.
                    llvm::ConstantAggregateZero *constant = llvm::ConstantAggregateZero::get(ty);
                    glob->setInitializer(constant);
                }
            }
            else
            {
                // FIXME: TRY PASSING VAR AS SUM IN ARGUMENT. IE, FUNCTION TAKES SUM AND WE JUST PROVIDE REGULAR TYPE!
                //  As this is a local var we can just create an allocation for it
                llvm::AllocaInst *v = builder->CreateAlloca(ty, 0, var->getText());
                varSymbol->val = v;

                // Similarly, if we have an expression for the local var, we can store it. Otherwise, we can leave it undefined.
                if (e->ex)
                {
                    Value *stoVal = exVal.value();
                    // FIXME: NEED TO ADD THIS FOR OTHER ASSIGN TYPE!
                    if (const TypeSum *sum = dynamic_cast<const TypeSum *>(varSymbol->type))
                    {
                        // FIXME: METHODIZE!!
                        unsigned int index = [sum, stoVal](llvm::Module *M)
                        {
                            unsigned i = 1;
                            auto toFind = stoVal->getType();
                            for (auto e : sum->getCases())
                            {
                                if (e->getLLVMType(M) == toFind)
                                {
                                    return i;
                                }
                                i++;
                            }

                            return (unsigned int)0;
                        }(module);

                        if (index == 0)
                        {
                            Value *corrected = builder->CreateBitCast(stoVal, varSymbol->type->getLLVMType(module));
                            builder->CreateStore(corrected, v); // FIXME: VERIFY, THIS ALSO MEANS SOME OF OUR TESTS WONT WORK
                            // errorHandler.addCodegenError(ctx->getStart(), "Unable to find key for type in sum");
                            return {}; // FIXME: DO BETTER!
                        }

                        // llvm::AllocaInst *inner = builder->CreateAlloca(ty, 0, var->getText());
                        // FIXME: SUMS CANT BE GENERATED AT GLOBAL LEVEL?
                        Value *tagPtr = builder->CreateGEP(v, {Int32Zero, Int32Zero});

                        builder->CreateStore(ConstantInt::get(Int32Ty, index, true), tagPtr);
                        Value *valuePtr = builder->CreateGEP(v, {Int32Zero, Int32One});

                        Value *corrected = builder->CreateBitCast(valuePtr, stoVal->getType()->getPointerTo()); // FIXME: DO BETTER
                        builder->CreateStore(stoVal, corrected);

                        // Value *loaded = builder->CreateLoad(inner->getType()->getPointerElementType(), inner);
                        // builder->CreateStore(loaded, v);
                    }
                    else
                    {
                        builder->CreateStore(stoVal, v);
                    }
                }
            }
        }
    }
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitLoopStatement(WPLParser::LoopStatementContext *ctx)
{
    // Very similar to conditionals

    std::optional<Value *> check = this->TvisitCondition(ctx->check); // std::any_cast<std::optional<Value *>>(ctx->check->accept(this));

    if (!check)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->check->getText());
        return {};
    }

    auto parent = builder->GetInsertBlock()->getParent();

    BasicBlock *loopBlk = BasicBlock::Create(module->getContext(), "loop", parent);
    BasicBlock *restBlk = BasicBlock::Create(module->getContext(), "rest");

    builder->CreateCondBr(check.value(), loopBlk, restBlk);

    // Need to add here otherwise we will overwrite it
    // parent->getBasicBlockList().push_back(loopBlk);

    /*
     * In the loop block
     */
    builder->SetInsertPoint(loopBlk);
    for (auto e : ctx->block()->stmts)
    {
        e->accept(this);
    }

    // Re-calculate the loop condition
    check = this->TvisitCondition(ctx->check);
    if (!check)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->check->getText());
        return {};
    }
    // Check if we need to loop back again...
    builder->CreateCondBr(check.value(), loopBlk, restBlk);
    loopBlk = builder->GetInsertBlock();

    /*
     * Out of loop
     */
    parent->getBasicBlockList().push_back(restBlk);
    builder->SetInsertPoint(restBlk);

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitConditionalStatement(WPLParser::ConditionalStatementContext *ctx)
{
    // Get the condition that the if statement is for
    std::optional<Value *> cond = this->TvisitCondition(ctx->check);

    if (!cond)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->check->getText());
        return {};
    }

    /*
     * Generate the basic blocks for then, else, and the remaining code.
     * (NOTE: We set rest to be else if there is no else branch).
     */
    auto parentFn = builder->GetInsertBlock()->getParent();

    BasicBlock *thenBlk = BasicBlock::Create(module->getContext(), "then", parentFn);
    BasicBlock *elseBlk = BasicBlock::Create(module->getContext(), "else");

    BasicBlock *restBlk = ctx->falseBlk ? BasicBlock::Create(module->getContext(), "ifcont")
                                        : elseBlk;

    builder->CreateCondBr(cond.value(), thenBlk, elseBlk);

    /*
     * Then block
     */
    builder->SetInsertPoint(thenBlk);
    for (auto e : ctx->trueBlk->stmts)
    {
        e->accept(this);
    }

    // If the block ends in a return, then we can't make the branch; things would break
    if (!CodegenVisitor::blockEndsInReturn(ctx->trueBlk))
    {
        builder->CreateBr(restBlk);
    }

    thenBlk = builder->GetInsertBlock();

    /*
     * Insert the else block (same as rest if no else branch)
     */
    parentFn->getBasicBlockList().push_back(elseBlk);
    builder->SetInsertPoint(elseBlk);

    if (ctx->falseBlk) // If we have an else branch
    {
        // Generate the code for the else block; follows the same logic as the then block.
        for (auto e : ctx->falseBlk->stmts)
        {
            e->accept(this);
        }

        if (!CodegenVisitor::blockEndsInReturn(ctx->falseBlk))
        {
            builder->CreateBr(restBlk);
        }

        elseBlk = builder->GetInsertBlock();

        // As we have an else block, rest and else are different, so we have to merge back in.
        parentFn->getBasicBlockList().push_back(restBlk);
        builder->SetInsertPoint(restBlk);
    }

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitSelectStatement(WPLParser::SelectStatementContext *ctx)
{
    /*
     * Set up the merge block that all cases go to after the select statement
     */
    auto origParent = builder->GetInsertBlock()->getParent();
    BasicBlock *mergeBlk = BasicBlock::Create(module->getContext(), "ifcont");

    // Iterate through each of the cases
    for (unsigned long i = 0; i < ctx->cases.size(); i++)
    {
        WPLParser::SelectAlternativeContext *evalCase = ctx->cases.at(i);

        // Visit the check code
        std::any anyCheck = evalCase->check->accept(this);

        // Attempt to cast the check; if this fails, then codegen for the check failed
        if (std::optional<Value *> optVal = std::any_cast<std::optional<Value *>>(anyCheck))
        {
            // Check that the optional, in fact, has a value. Otherwise, something went wrong.
            if (!optVal)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + evalCase->getText());
                return {};
            }

            // Knowing that we have a value, get what the value is.
            Value *val = optVal.value();

            // Helpful check for later on
            bool isLast = i == ctx->cases.size() - 1;

            // Create the then and else blocks as if this were an if statement
            auto parent = builder->GetInsertBlock()->getParent();
            BasicBlock *thenBlk = BasicBlock::Create(module->getContext(), "then", parent);
            BasicBlock *elseBlk = isLast ? mergeBlk : BasicBlock::Create(module->getContext(), "else");

            // Branch based on the value
            builder->CreateCondBr(val, thenBlk, elseBlk);

            /*
             *
             * THEN BLOCK
             *
             */
            builder->SetInsertPoint(thenBlk);

            // Visit the evaluation code for the case
            evalCase->eval->accept(this);

            /*
             * As codegen worked, we now need to determine if
             * the code we generated was for a block ending in
             * a return or if it is a return statement. This
             * Must be done as it determines if we create
             * a merge into the merge block or not.
             */
            if (WPLParser::BlockStatementContext *blkStmtCtx = dynamic_cast<WPLParser::BlockStatementContext *>(evalCase->eval))
            {
                WPLParser::BlockContext *blkCtx = blkStmtCtx->block();
                if (!CodegenVisitor::blockEndsInReturn(blkCtx))
                {
                    builder->CreateBr(mergeBlk);
                }
                // if it ends in a return, we're good!
            }
            else if (WPLParser::ReturnStatementContext *retCtx = dynamic_cast<WPLParser::ReturnStatementContext *>(evalCase->eval))
            {
                // Similarly, we don't need to generate the branch
            }
            else
            {
                builder->CreateBr(mergeBlk);
            }

            thenBlk = builder->GetInsertBlock();

            /*
             *
             * Else Block
             *
             */
            if (!isLast)
            {
                parent->getBasicBlockList().push_back(elseBlk);
                builder->SetInsertPoint(elseBlk);
            }
        }
    }

    // We could probably do this as an else on the is !isLast check, but this works
    origParent->getBasicBlockList().push_back(mergeBlk);
    builder->SetInsertPoint(mergeBlk);

    std::optional<Value *> ans = {};
    return ans;
}

// Passthrough function
std::optional<Value *> CodegenVisitor::TvisitCallStatement(WPLParser::CallStatementContext *ctx) { return this->TvisitInvocation(ctx->call); }

std::optional<Value *> CodegenVisitor::TvisitReturnStatement(WPLParser::ReturnStatementContext *ctx)
{
    // Check if we are returning an expression or not
    if (ctx->expression())
    {
        // If we are, then visit that expression
        std::any anyInner = ctx->expression()->accept(this);

        // Perform some checks to make sure that code was generated
        if (std::optional<Value *> inner = std::any_cast<std::optional<Value *>>(anyInner))
        {
            if (!inner)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
                return {};
            }

            // As the code was generated correctly, build the return statement; we ensure no following code due to how block visitors work in semantic analysis.
            Value *v = builder->CreateRet(inner.value());

            return v;
        }
        else
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
            return {};
        }
    }

    // If there is no value, return void. We ensure no following code and type-correctness in the semantic pass.
    Value *v = builder->CreateRetVoid();
    return v;
}

std::optional<Value *> CodegenVisitor::TvisitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    Value *val = ctx->TRUE() ? builder->getTrue() : builder->getFalse();
    return val;
}

// Passthrough function
std::optional<Value *> CodegenVisitor::TvisitBlockStatement(WPLParser::BlockStatementContext *ctx) { return this->TvisitBlock(ctx->block()); }

std::optional<Value *> CodegenVisitor::TvisitBlock(WPLParser::BlockContext *ctx)
{
    for (auto e : ctx->stmts)
    {
        e->accept(this);
    }

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitLambdaConstExpr(WPLParser::LambdaConstExprContext *ctx)
{
    // FIXME: SEE ALL ISSUES W GENERAL INVOKE VISIT
    // FIXME: SET OTHER LAMBDAS TO BE INTERNAL LINKAGE!!!
    // FIXME: TEST MULTIPLE LAMBDAS W/ SAME NAME!!
    // Get the current insertion point
    BasicBlock *ins = builder->GetInsertBlock();

    // Lookup the binding
    std::optional<Symbol *> symOpt = props->getBinding(ctx);

    if (!symOpt)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unbound lambda: " + ctx->getText());
        return {};
    }

    Symbol *sym = symOpt.value();

    if (!sym->type)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Symbol in lambda missing type. Probably compiler error.");
        return {};
    }

    const Type *type = sym->type;

    llvm::Type *genericType = type->getLLVMType(module)->getPointerElementType(); // FIXME: CHECK IF SAFE!

    if (llvm::FunctionType *fnType = static_cast<llvm::FunctionType *>(genericType))
    {
        Function *fn = Function::Create(fnType, GlobalValue::PrivateLinkage, "LAM", module); // FIXME: VERIFY
        WPLParser::ParameterListContext *paramList = ctx->parameterList();

        // Create basic block
        BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", fn);
        builder->SetInsertPoint(bBlk);

        // Bind all of the arguments
        for (auto &arg : fn->args())
        {
            // Get the argumengt number (just seems easier than making my own counter)
            int argNumber = arg.getArgNo();

            // Get the argument's type
            llvm::Type *type = fnType->params()[argNumber];

            // Get the argument name (This even works for arrays!)
            std::string argName = paramList->params.at(argNumber)->getText();

            // Create an allocation for the argumentr
            llvm::AllocaInst *v = builder->CreateAlloca(type, 0, argName);

            // Try to find the parameter's bnding to determine what value to bind to it.
            std::optional<Symbol *> symOpt = props->getBinding(paramList->params.at(argNumber));

            if (!symOpt)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Unable to generate parameter for lambda: " + argName);
            }
            else
            {
                symOpt.value()->val = v;

                builder->CreateStore(&arg, v);
            }
        }

        // Generate code for the block
        for (auto e : ctx->block()->stmts)
        {
            e->accept(this);
        }

        // FIXME: NOTE HOW WE DONT NEED TO CREATE RET VOID EVER BC NO FN!

        // Return to original insert point
        builder->SetInsertPoint(ins);

        return fn;
    }
    else
    {
        errorHandler.addCodegenError(ctx->getStart(), "Invocation type could not be cast to function!");
    }

    // Return to original insert point
    builder->SetInsertPoint(ins);

    return {}; // FIXME: if we ever get here, LLVM will probs seg fault
}

/*
 *
 * UNUSED VISITORS
 * ===============
 *
 * These are visitors which should NEVER be seen during the compilation process.
 *
 */
std::optional<Value *> CodegenVisitor::TvisitTypeOrVar(WPLParser::TypeOrVarContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "TypeOrVar fragment should never be visited directly by codegen!");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitAssignment(WPLParser::AssignmentContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Assignment fragment should never be visited directly during codegen!");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitParameterList(WPLParser::ParameterListContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown error: Codegen should not have to visits parameter list!");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitParameter(WPLParser::ParameterContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown error: Codegen should not have to visit parameter!");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitType(WPLParser::TypeContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown error: Codegen should never directly visit types looking for values!");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitArrayOrVar(WPLParser::ArrayOrVarContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown Error: Codegen should never directly visit ArrayOrVar!");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitSelectAlternative(WPLParser::SelectAlternativeContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown Error: Codegen should never directly visit SelectAlternative!");
    return {};
}
