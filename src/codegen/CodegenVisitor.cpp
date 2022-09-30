#include "CodegenVisitor.h"

std::optional<Value *> CodegenVisitor::TvisitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    for (auto e : ctx->extens)
    {
        e->accept(this);
    }

    // FIXME: SURROUND IN IF
    for (auto e : ctx->stmts)
    {
        // Generate code for statement
        e->accept(this);
    }

    /*
     * Create main to invoke program
     */

    // FIXME: SHOULD WE DISALLOW MAIN?

    FunctionType *mainFuncType = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main", module);

    // Create block to attach to main
    BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", mainFunc);
    builder->SetInsertPoint(bBlk);

    // FIXME: WE SEGFAULT IF NOT FOUND!!!

    llvm::Function *progFn = module->getFunction("program");
    builder->CreateRet(builder->CreateCall(progFn, {}));

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

    // Create the expression pointer
    llvm::AllocaInst *arrayPtr = props->getBinding(ctx)->val;
    auto ptr = builder->CreateGEP(arrayPtr, {Int32Zero, index.value()});
    Value *val = builder->CreateLoad(ptr->getType()->getPointerElementType(), ptr);
    return val;
}

std::optional<Value *> CodegenVisitor::TvisitArrayOrVar(WPLParser::ArrayOrVarContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitArrayOrVar");
    return {};
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
    // FIXME: UNESCAPE STRINg
    std::string full(ctx->s->getText());
    std::string actual = full.substr(1, full.length() - 2);

    // std::regex reg("\\\\(.)");
    // std::string out = regex_replace(actual, reg, R"($1)");

    // FIXME: MAKE THIS WORK!!!
    std::regex basic("\\\\n");
    std::string out = regex_replace(actual, basic, "\n");

    // StringRef ref = actual;
    Value *strVal = builder->CreateGlobalStringPtr(out); // For some reason, I can't return this directly...

    return strVal;
}

// FIXME: SHOULD WE ALLOW INTS IN VARIABLE NAMES? PROBABLY

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

    // FIXME: REVIEW
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
    // FIXME: VERIFY GOOD ENOUGH! PROBS WONT WORK ON STRINGS!!!
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
    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

    if (!lhs || !rhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
        return {};
    }

    Value *IR = builder->CreateAnd(lhs.value(), rhs.value());
    Value *v = builder->CreateZExtOrTrunc(IR, Int1Ty);
    return v;
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
    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

    if (!lhs || !rhs)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
        return {};
    }

    Value *IR = builder->CreateOr(lhs.value(), rhs.value());
    Value *v = builder->CreateZExtOrTrunc(IR, Int1Ty);
    return v;
}

// Passthrough to TvisitInvocation
std::optional<Value *> CodegenVisitor::TvisitCallExpr(WPLParser::CallExprContext *ctx) { return this->TvisitInvocation(ctx->call); }

std::optional<Value *> CodegenVisitor::TvisitVariableExpr(WPLParser::VariableExprContext *ctx)
{
    // FIXME: should probably methodize...
    std::string id = ctx->v->getText();
    Symbol *sym = props->getBinding(ctx);

    if (!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Undefined variable access: " + id);
        return {};
    }

    // FIXME: ADD TYPES
    llvm::Type *type = sym->type->getLLVMType(module->getContext());
    if (!type)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unable to find type for variable: " + ctx->getText());
    }

    if (!sym->val)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unable to find allocation for variable: " + ctx->getText());
    }
    // std::cout << "CREATE LOAD FOR: " << id << " " << sym->toString() << " WITH " << type << std::endl;
    Value *v = builder->CreateLoad(type, sym->val, id);
    return v;
}

std::optional<Value *> CodegenVisitor::TvisitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx)
{

    // This is ONLY array length for now...
    Symbol *sym = props->getBinding(ctx->ex); // FIXME: DO WE NEED TO VISIT? -> I guess not currently b/c all type stuff, but probably should and/or remove and change it to VAR?

    if (!sym || !sym->val || !sym->type)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized array access symbol.");
        return {};
    }

    if (const TypeArray *ar = dynamic_cast<const TypeArray *>(sym->type))
    {
        Value *v = builder->getInt32(ar->getLength());

        return v;
    }

    errorHandler.addCodegenError(ctx->getStart(), "Given non-array type in TvisitFieldAccessExpr!");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitParenExpr(WPLParser::ParenExprContext *ctx)
{
    return std::any_cast<std::optional<Value *>>(ctx->ex->accept(this)); // FIXME: VERIFY GOOD ENOUGH
}

std::optional<Value *> CodegenVisitor::TvisitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx)
{
    std::optional<Value *> lhs = std::any_cast<std::optional<Value *>>(ctx->left->accept(this));
    std::optional<Value *> rhs = std::any_cast<std::optional<Value *>>(ctx->right->accept(this));

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
        v1 = builder->CreateICmpSLE(lhs.value(), rhs.value()); // FIXME: VERIFY
        break;
    case WPLParser::GREATER:
        v1 = builder->CreateICmpSGT(lhs.value(), rhs.value());
        break;
    case WPLParser::GREATER_EQ:
        v1 = builder->CreateICmpSGE(lhs.value(), rhs.value()); // FIXME: VERIFY
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
    // Based on https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl05.html
    // FIXME: VERIFY, This might not actually get a boolean well enough....
    return std::any_cast<std::optional<Value *>>(ctx->ex->accept(this));
}

std::optional<Value *> CodegenVisitor::TvisitSelectAlternative(WPLParser::SelectAlternativeContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitSelectAlternative");
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitExternStatement(WPLParser::ExternStatementContext *ctx)
{
    std::vector<llvm::Type *> typeVec;

    if (ctx->paramList)
    {
        for (auto e : ctx->paramList->params)
        {
            llvm::Type *type = CodegenVisitor::llvmTypeFor(e->ty);
            typeVec.push_back(type);
        }
    }

    ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);
    bool isVariadic = ctx->variadic || ctx->ELLIPSIS();

    FunctionType *fnType = FunctionType::get(
        CodegenVisitor::llvmTypeFor(ctx->ty),
        paramRef,
        isVariadic);

    Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);
    return {};
}

std::optional<Value *> CodegenVisitor::TvisitFuncDef(WPLParser::FuncDefContext *ctx)
{
    std::vector<llvm::Type *> typeVec;

    Symbol *sym = props->getBinding(ctx);
    if (!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unbound function: " + ctx->name->getText());
        return {};
    }

    const TypeInvoke *inv = dynamic_cast<const TypeInvoke *>(sym->type);

    if (ctx->paramList)
    {
        for (auto e : ctx->paramList->params)
        {
            llvm::Type *type = CodegenVisitor::llvmTypeFor(e->ty);
            typeVec.push_back(type);
        }
    }
    ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);

    FunctionType *fnType = FunctionType::get(
        CodegenVisitor::llvmTypeFor(ctx->ty),
        paramRef,
        false);

    Function *fn = Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);

    // Create block
    BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", fn); // FIXME: USING ENTRY MAY BE AN ISSUE?
    builder->SetInsertPoint(bBlk);

    std::cout << "438" << std::endl;

    // FIXME: NEED TO DO THIS FOR PROCs AS WELL!!!
    for (auto &arg : fn->args())
    {
        int argNumber = arg.getArgNo();
        llvm::Type *type = typeVec.at(argNumber);

        std::string argName = ctx->paramList->params.at(argNumber)->getText();

        llvm::AllocaInst *v = builder->CreateAlloca(type, 0, argName);
        Symbol *sym = props->getBinding(ctx->paramList->params.at(argNumber));

        if (!sym)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Unable to generate parameter for function: " + argName);
        }
        else
        {
            sym->val = v;

            builder->CreateStore(&arg, v);
        }
    }

    std::cout << "453" << std::endl;

    std::any last = nullptr;
    for (auto e : ctx->block()->stmts)
    {
        last = e->accept(this);
    }

    if (ctx->block()->stmts.size() > 0 && dynamic_cast<WPLParser::ReturnStatementContext *>(ctx->block()->stmts.at(ctx->block()->stmts.size() - 1)))
    {
        // builder->CreateRet(
        //     std::any_cast<Value *>(last)
        // );
    }

    // FIXME: VERIFY ENOUGH, NOTHING FOLLOWING, ETC. THIS IS PROBS WRONG!

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitProcDef(WPLParser::ProcDefContext *ctx)
{
    // FIXME: VERIFY + METHODIZE
    std::vector<llvm::Type *> typeVec;

    Symbol *sym = props->getBinding(ctx);
    if (!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unbound function: " + ctx->name->getText());
        return {};
    }

    const TypeInvoke *inv = dynamic_cast<const TypeInvoke *>(sym->type);

    if (ctx->paramList)
    {
        for (auto e : ctx->paramList->params)
        {
            llvm::Type *type = this->llvmTypeFor(e->ty);
            typeVec.push_back(type);
        }
    }
    ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);

    FunctionType *fnType = FunctionType::get(
        VoidTy,
        paramRef,
        false);

    Function *fn = Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);

    // Create block
    BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", fn); // FIXME: USING ENTRY MAY BE AN ISSUE?
    builder->SetInsertPoint(bBlk);

    // FIXME: NEED TO DO THIS FOR PROCs AS WELL!!!
    for (auto &arg : fn->args())
    {
        int argNumber = arg.getArgNo();
        llvm::Type *type = typeVec.at(argNumber);

        std::string argName = ctx->paramList->params.at(argNumber)->getText();

        llvm::AllocaInst *v = builder->CreateAlloca(type, 0, argName);
        Symbol *sym = props->getBinding(ctx->paramList->params.at(argNumber));

        if (!sym)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Unable to generate parameter for function: " + argName);
        }
        else
        {
            sym->val = v;

            builder->CreateStore(&arg, v);
        }
    }

    for (auto e : ctx->block()->stmts)
    {
        e->accept(this);
    }

    // FIXME: VERIFY ENOUGH, NOTHING FOLLOWING, ETC. THIS IS PROBS WRONG!

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitAssignStatement(WPLParser::AssignStatementContext *ctx)
{
    // FIXME: Might not work perfectly due to no arrays/vars yet.... or strings...
    std::optional<Value *> exprVal = std::any_cast<std::optional<Value *>>(ctx->ex->accept(this));

    if (!exprVal)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->ex->getText());
        return {};
    }

    Symbol *varSym = props->getBinding(ctx->to);

    if (varSym == nullptr)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Incorrectly processed variable in assignment: " + ctx->to->getText());
        return {};
    }
    // Shouldn't need this in the end....
    if (varSym->val == nullptr)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized variable in assignment: " + ctx->to->getText() + "@" + varSym->identifier);
        std::cout << "IMPROP VAR @ " << varSym << std::endl;
        return {};
    }

    Value *val = varSym->val;

    if (!ctx->to->VARIABLE())
    {
        // Dealing with an array //FIXME: REFACTOR!!!
        std::optional<Value *> index = std::any_cast<std::optional<Value *>>(ctx->to->array->index->accept(this));

        if (!index)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->to->getText());
        }

        Value *built = builder->CreateGEP(val, {Int32Zero, index.value()});

        std::cout << "551 " << built << std::endl;

        val = built; // builder->CreateGEP(val, {Int32Zero, index.value()});
    }

    builder->CreateStore(exprVal.value(), val);

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    // FIXME: MODIFY TO DO THINGS BY TYPE!!!!

    for (auto e : ctx->assignments)
    {
        // FIXME: DOESNT WORK WHEN NO VALUE!!!
        std::optional<Value *> exVal = (e->ex) ? std::any_cast<std::optional<Value *>>(e->ex->accept(this)) : std::nullopt; // builder->getInt32(0);

        if ((e->ex) && !exVal)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + e->ex->getText());
        }

        for (auto var : e->VARIABLE())
        {
            Symbol *varSymbol = props->getBinding(var);

            if (!varSymbol)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Issue creating variable: " + var->getText());
                return {};
            }

            std::cout << "493 4 " << ctx->getText() << !!varSymbol->type << std::endl;

            llvm::Type *ty = varSymbol->type->getLLVMType(module->getContext());
            std::cout << "495 4 " << ctx->getText() << std::endl;
            llvm::AllocaInst *v = builder->CreateAlloca(ty, 0, var->getText());

            std::cout << "Set Val for: " << varSymbol->identifier << "(" << var->getText() << ") @" << varSymbol << std::endl;

            std::cout << " META: " << v->isArrayAllocation() << " AND " << v->getArraySize() << std::endl;
            varSymbol->val = v;

            if (e->ex)
                builder->CreateStore(exVal.value(), v);
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

    BasicBlock *loopBlk = BasicBlock::Create(module->getContext(), "loop");
    BasicBlock *restBlk = BasicBlock::Create(module->getContext(), "rest");

    builder->CreateCondBr(check.value(), loopBlk, restBlk);

    // Need to add here otherwise we will overwrite it
    parent->getBasicBlockList().push_back(loopBlk);

    // In the loop block
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
    loopBlk = builder->GetInsertBlock(); // FIXME: REVIEW
    builder->CreateCondBr(check.value(), loopBlk, restBlk);

    // FIXME: ALLOW _ IN NAMES?
    //  Out of Loop

    parent->getBasicBlockList().push_back(restBlk);
    builder->SetInsertPoint(restBlk);

    return {};
}

// FIXME: EXTERNS THAT ARE JUST ...!!!!
// FIXME: EXTERNS CANT HAVE A SPACE?

std::optional<Value *> CodegenVisitor::TvisitConditionalStatement(WPLParser::ConditionalStatementContext *ctx)
{
    // FIXME: THIS MIGHT NOT WORK OUTSIDE A FUNCTION
    std::optional<Value *> cond = this->TvisitCondition(ctx->check); // std::any_cast<Value *>(ctx->check->accept(this));

    if (!cond)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->check->getText());
        return {};
    }

    auto parentFn = builder->GetInsertBlock()->getParent();

    BasicBlock *thenBlk = BasicBlock::Create(module->getContext(), "then", parentFn);
    BasicBlock *elseBlk = BasicBlock::Create(module->getContext(), "else");

    BasicBlock *restBlk = ctx->falseBlk ? BasicBlock::Create(module->getContext(), "ifcont")
                                        : elseBlk;

    // FIXME: WHAT IF NO ELSE BLOCK??

    builder->CreateCondBr(cond.value(), thenBlk, elseBlk);

    // Then block
    // parentFn->getBasicBlockList().push_back(thenBlk);
    builder->SetInsertPoint(thenBlk);
    std::any lastTrue = nullptr;
    for (auto e : ctx->trueBlk->stmts)
    {
        // FIXME: UNSAFE WITH RETUNING NULL! // FIXME: STILL NEED TO CAST??
        lastTrue = e->accept(this); // std::any_cast<Value*>(e->accept(this));
    }

    if (!CodegenVisitor::blockEndsInReturn(ctx->trueBlk))
    {
        builder->CreateBr(restBlk);
    }

    thenBlk = builder->GetInsertBlock(); // REVIEW

    // Else block  //FIXME: THIS TREATS IT AS REQUIRED. SHOULD WE DO SOMETHING AB THIS?
    parentFn->getBasicBlockList().push_back(elseBlk);
    builder->SetInsertPoint(elseBlk);

    if (ctx->falseBlk)
    {
        std::any lastFalse = nullptr;
        for (auto e : ctx->falseBlk->stmts)
        {
            // FIXME: UNSAFE W/ RETURNING NULL;
            lastFalse = e->accept(this); // std::any_cast<Value*>(e->accept(this));
        }

        if (!CodegenVisitor::blockEndsInReturn(ctx->falseBlk))
        {
            builder->CreateBr(restBlk);
        }

        elseBlk = builder->GetInsertBlock();

        // Merge back in
        parentFn->getBasicBlockList().push_back(restBlk);
        builder->SetInsertPoint(restBlk);
    }

    return {};
}

std::optional<Value *> CodegenVisitor::TvisitSelectStatement(WPLParser::SelectStatementContext *ctx)
{
    // FIXME: WILL NEED TO CHECK FOR RETURNS AND RETURNS IN BLOCKS!!!

    auto origParent = builder->GetInsertBlock()->getParent();
    BasicBlock *mergeBlk = BasicBlock::Create(module->getContext(), "ifcont");

    for (unsigned long i = 0; i < ctx->cases.size(); i++) // WPLParser::SelectAlternativeContext *evalCase : ctx->cases)
    {
        WPLParser::SelectAlternativeContext *evalCase = ctx->cases.at(i);

        std::any anyCheck = evalCase->check->accept(this);

        if (std::optional<Value *> optVal = std::any_cast<std::optional<Value *>>(anyCheck))
        {
            if (!optVal)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + evalCase->getText());
                return {};
            }

            Value *val = optVal.value();

            bool isLast = i == ctx->cases.size() - 1;

            auto parent = builder->GetInsertBlock()->getParent();

            BasicBlock *thenBlk = BasicBlock::Create(module->getContext(), "then", parent);
            BasicBlock *elseBlk = isLast ? mergeBlk : BasicBlock::Create(module->getContext(), "else");

            builder->CreateCondBr(val, thenBlk, elseBlk);

            /*
             *
             * THEN BLOCK
             *
             */
            builder->SetInsertPoint(thenBlk);
            std::any thenAny = evalCase->eval->accept(this);
            if (std::optional<Value *> thenOpt = std::any_cast<std::optional<Value *>>(thenAny))
            {
                if (!thenOpt)
                {
                    errorHandler.addCodegenError(evalCase->getStart(), "Failed to generate code for case: " + evalCase->eval->getText());
                    return {};
                }
                Value *thenVal = thenOpt.value();

                if (WPLParser::BlockContext * blkCtx = dynamic_cast<WPLParser::BlockContext*>(evalCase->eval))
                {
                    if(!CodegenVisitor::blockEndsInReturn(blkCtx))
                    {
                        builder->CreateBr(mergeBlk);
                    }
                    //if it ends in a return, we're good!
                }
                else if(WPLParser::ReturnStatementContext* retCtx = dynamic_cast<WPLParser::ReturnStatementContext*>(evalCase->eval))
                {
                    //Similarly, we don't need to generate the branch
                }
                else 
                {
                    builder->CreateBr(mergeBlk);
                }

                thenBlk = builder->GetInsertBlock(); 

                /*
                 *
                 * Else Block!
                 * 
                 */
                if(!isLast)
                {
                    parent->getBasicBlockList().push_back(elseBlk);
                    builder->SetInsertPoint(elseBlk);
                }
                
            }
            else
            {
                errorHandler.addCodegenError(evalCase->getStart(), "Failed to generate code for case: " + evalCase->eval->getText());
                return {};
            }

            // FIXME: just do recursivley
        }
    }
    
    //FIXME: actually, could we do this in a way where we remove the is last check and put it there? 
    origParent->getBasicBlockList().push_back(mergeBlk);
    builder->SetInsertPoint(mergeBlk);
    
    return {};
}

// Passthrough function
std::optional<Value *> CodegenVisitor::TvisitCallStatement(WPLParser::CallStatementContext *ctx) { return this->TvisitInvocation(ctx->call); }

std::optional<Value *> CodegenVisitor::TvisitReturnStatement(WPLParser::ReturnStatementContext *ctx)
{
    if (ctx->expression())
    {

        std::optional<Value *> inner = std::any_cast<std::optional<Value *>>(ctx->expression()->accept(this)); // FIXME: UNSAFE W/ ERORS

        if (!inner)
        {
            errorHandler.addCodegenError(ctx->getStart(), "Failed to generate code for: " + ctx->getText());
            return {};
        }

        Value *v = builder->CreateRet(inner.value());

        // FIXME: ENSURE NO FOLLOWING CODE

        return v;
    }
    Value *v = builder->CreateRetVoid();
    // FIXME: ENSURE NO FOLLOWING CODE, ENSURE CORRECT!!
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

// FIXME: maybe these should be meta/compiler errors
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