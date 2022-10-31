#include "CodegenVisitor.h"

std::optional<Value *> CodegenVisitor::TvisitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    for (auto e : ctx->extens)
    {
        e->accept(this);
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
            errorHandler.addCodegenError(e->getStart(), "Failed to generate code");
            return {};
        }
        args.push_back(valOpt.value());
    }

    // Convert to an array ref, then find and execute the call.
    ArrayRef<Value *> ref = ArrayRef(args);
    llvm::Function *call = module->getFunction(ctx->VARIABLE()->getText());

    Value *val = builder->CreateCall(call, ref); // Needs to be separate line because, C++
    return val;
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

    Symbol * sym = symOpt.value(); 

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

    std::optional<Value *> lhs = std::any_cast<std::optional<Value *>>(ctx->left->accept(this));

    if (!lhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->left->getText());
        return {};
    }

    // Create the basic block for our conditions
    BasicBlock *current = builder->GetInsertBlock();
    auto parent = current->getParent();
    BasicBlock *trueBlk = BasicBlock::Create(module->getContext(), "lhsTrue", parent);
    BasicBlock *falseBlk = BasicBlock::Create(module->getContext(), "lhsFalse");

    // Branch on the lhs value
    builder->CreateCondBr(lhs.value(), trueBlk, falseBlk);

    /*
     * LHS True - Need to check RHS
     */
    builder->SetInsertPoint(trueBlk);
    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

    if (!rhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->right->getText());
        return {};
    }

    builder->CreateBr(falseBlk);
    trueBlk = builder->GetInsertBlock();

    /*
     * LHS False - Can short as statement being false
     */
    parent->getBasicBlockList().push_back(falseBlk);
    builder->SetInsertPoint(falseBlk);

    /*
     * Add PHI node to merge things back together
     */
    PHINode *phi = builder->CreatePHI(Int1Ty, 2, "logAnd");
    phi->addIncoming(builder->getFalse(), current);
    phi->addIncoming(rhs.value(), trueBlk);
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
    std::optional<Value *> lhs = std::any_cast<std::optional<Value *>>(ctx->left->accept(this));

    if (!lhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->left->getText());
        return {};
    }

    // Create the basic block for our conditions
    BasicBlock *current = builder->GetInsertBlock();
    auto parent = current->getParent();
    BasicBlock *falseBlk = BasicBlock::Create(module->getContext(), "lhsFalse", parent);
    BasicBlock *trueBlk = BasicBlock::Create(module->getContext(), "lhsTrue");

    // Branch on the lhs value
    builder->CreateCondBr(lhs.value(), trueBlk, falseBlk);

    /*
     * LHS False - Need to check RHS value
     */
    builder->SetInsertPoint(falseBlk);

    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

    if (!rhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->right->getText());
        return {};
    }

    builder->CreateBr(trueBlk);
    falseBlk = builder->GetInsertBlock();

    /*
     * LHS True - Can skip checking RHS and return true
     */
    parent->getBasicBlockList().push_back(trueBlk);
    builder->SetInsertPoint(trueBlk);

    /*
     * PHI node to merge both sides back together
     */
    PHINode *phi = builder->CreatePHI(Int1Ty, 2, "logOr");
    phi->addIncoming(lhs.value(), current);
    phi->addIncoming(rhs.value(), falseBlk);
    return phi;
}

// Passthrough to TvisitInvocation
std::optional<Value *> CodegenVisitor::TvisitCallExpr(WPLParser::CallExprContext *ctx) { return this->TvisitInvocation(ctx->call); }

std::optional<Value *> CodegenVisitor::TvisitVariableExpr(WPLParser::VariableExprContext *ctx)
{
    // Get the variable name and look it up
    std::string id = ctx->v->getText();
    std::optional<Symbol *> symOpt = props->getBinding(ctx);

    // If the symbol could not be found, raise an error
    if (!symOpt)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Undefined variable access: " + id);
        return {};
    }

    Symbol * sym = symOpt.value(); 

    // Try getting the type for the symbol, raising an error if it could not be determined
    llvm::Type *type = sym->type->getLLVMType(module->getContext());
    if (!type)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unable to find type for variable: " + ctx->getText());
    }

    // Make sure the variable has an allocation (or that we can find it due to it being a global var)
    if (!sym->val)
    {
        // If the symbol is a global var
        if (sym->isGlobal)
        {
            // Lookup the global var for the symbol
            llvm::GlobalVariable *glob = module->getNamedGlobal(sym->identifier);

            // Check that we found the variable. If not, throw an error.
            if (!glob)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Unable to find global variable: " + id);
                return {};
            }

            // Create and return a load for the global var
            Value *val = builder->CreateLoad(glob);
            return val;
        }

        errorHandler.addCodegenError(ctx->getStart(), "Unable to find allocation for variable: " + ctx->getText());
    }

    // Otherwise, we are a local variable with an allocation and, thus, can simply load it.
    Value *v = builder->CreateLoad(type, sym->val.value(), id);
    return v;
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

    Symbol * sym = symOpt.value(); 

    if (!symOpt || !sym->val || !sym->type)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized array access symbol.");
        return {};
    }

    // Check that the type is an array type
    if (const TypeArray *ar = dynamic_cast<const TypeArray *>(sym->type))
    {
        // If it is, correctly, an array type, then we can get the array's length (this is the only operation currently, so we can just do thus)
        Value *v = builder->getInt32(ar->getLength());

        return v;
    }

    // Throw an error as we currently only support array length.
    errorHandler.addCodegenError(ctx->getStart(), "Given non-array type in TvisitFieldAccessExpr!");
    return {};
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
    // Cretae a vector for our argument types
    std::vector<llvm::Type *> typeVec;

    // If the extern has a paramlist
    if (ctx->paramList)
    {
        // Go through each parameter, and get it its type. Stop if any errors occur
        for (auto e : ctx->paramList->params)
        {
            std::optional<llvm::Type *> type = CodegenVisitor::llvmTypeFor(e->ty);

            if (!type)
            {
                errorHandler.addCodegenError(e->getStart(), "Could not generate code to represent type: " + e->ty->toString());
                return {};
            }

            typeVec.push_back(type.value());
        }
    }

    // Create an array ref of our parameter types
    ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);
    // Determine if the function is variadic
    bool isVariadic = ctx->variadic || ctx->ELLIPSIS();

    // Generate the return type or set it to be Void if PROC
    std::optional<llvm::Type *> retOpt = ctx->ty ? CodegenVisitor::llvmTypeFor(ctx->ty) : VoidTy;

    // If we fail to generate a return, then throw an error.
    if (!retOpt)
    {
        errorHandler.addCodegenError(ctx->ty->getStart(), "Could not generate code for type: " + ctx->ty->toString());
        return {};
    }

    // Create the function definition
    FunctionType *fnType = FunctionType::get(
        retOpt.value(),
        paramRef,
        isVariadic);

    Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitFuncDef(WPLParser::FuncDefContext *ctx)
{
    return CodegenVisitor::visitInvokeable(ctx);
}

std::optional<Value *> CodegenVisitor::TvisitProcDef(WPLParser::ProcDefContext *ctx)
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

    Symbol * varSym = varSymOpt.value();

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
    builder->CreateStore(exprVal.value(), val.value());

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

            Symbol * varSymbol = varSymbolOpt.value(); 

            // Get the type of the symbol
            llvm::Type *ty = varSymbol->type->getLLVMType(module->getContext());

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
                // As this is a local var we can just create an allocation for it
                llvm::AllocaInst *v = builder->CreateAlloca(ty, 0, var->getText());
                varSymbol->val = v;

                // Similarly, if we have an expression for the local var, we can store it. Otherwise, we can leave it undefined.
                if (e->ex)
                    builder->CreateStore(exVal.value(), v);
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
                WPLParser::BlockContext * blkCtx = blkStmtCtx->block(); 
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
