#include "CodegenVisitor.h"

std::any CodegenVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    // External functions --- Temporary thing
    // auto printf_prototype = FunctionType::get(i8p, true);
    // auto printf_fn = Function::Create(printf_prototype, Function::ExternalLinkage, "printf", module);
    // FunctionCallee printExpr(printf_prototype, printf_fn);

    // Main function --- FIXME: WE DON'T DO THIS IN FINAL VERSION!!!
    // FunctionType *mainFuncType = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
    // Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main", module);

    // // Create block to attach to main
    // BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", mainFunc);
    // builder->SetInsertPoint(bBlk);

    for (auto e : ctx->extens)
    {
        e->accept(this);
    }

    // FIXME: SURROUND IN IF
    for (auto e : ctx->stmts)
    {
        // Generate code for statement
        //  Value *result = std::any_cast<Value *>(e->accept(this));
        e->accept(this);

        // Log expression for debugging purposes
        // auto txt = e->getText();
        StringRef formatRef = "Processed %s\n";
        auto gFormat = builder->CreateGlobalStringPtr(formatRef, "fmtStr");
        // StringRef ref = txt;
        // auto fmt = builder->CreateGlobalStringPtr(ref, "exprStr");
        // builder->CreateCall(printf_fn, {gFormat, fmt});
    }

    // builder->CreateRet(Int32Zero);

    /*
     * Create main to invoke program
     */

    // FIXME: SHOULD WE DISALLOW MAIN?

    FunctionType *mainFuncType = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main", module);

    // Create block to attach to main
    BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", mainFunc);
    builder->SetInsertPoint(bBlk);

    // auto progType = FunctionType::get(Int32Ty, true);
    // auto progFn = Function::Create(progType, Function::ExternalLinkage, "program", module);
    // FunctionCallee progCall(progType, progFn);

    // builder->CreateRet(
    //     builder->CreateCall(progFn, {})
    // );

    // FIXME: WE SEGFAULT IF NOT FOUND!!!

    llvm::Function *progFn = module->getFunction("program");
    builder->CreateRet(
        builder->CreateCall(progFn, {}));

    return nullptr; // FIXME: DANGER!!
}

std::any CodegenVisitor::visitInvocation(WPLParser::InvocationContext *ctx)
{
    // FIXME: IMPL VARIADIC - should be ok on this side...
    std::vector<llvm::Value *> args;
    std::cout << "75!" << std::endl;
    for (auto e : ctx->args)
    {
        Value *val = std::any_cast<Value *>(e->accept(this));
        args.push_back(val);
    }
    std::cout << "81!" << std::endl;
    ArrayRef<Value *> ref = ArrayRef(args);

    llvm::Function *call = module->getFunction(ctx->VARIABLE()->getText());

    Value *val = builder->CreateCall(call, ref); // Needs to be separate line because, C++
    return val;
}

std::any CodegenVisitor::visitArrayAccess(WPLParser::ArrayAccessContext *ctx)
{
    Value *index = std::any_cast<Value *>(ctx->index->accept(this));
    llvm::AllocaInst *arrayPtr = props->getBinding(ctx)->val;
    auto ptr = builder->CreateGEP(arrayPtr, {Int32Zero, index});
    Value *val = builder->CreateLoad(ptr->getType()->getPointerElementType(), ptr);
    return val;
}

std::any CodegenVisitor::visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitArrayOrVar");
    return nullptr;
}

std::any CodegenVisitor::visitIConstExpr(WPLParser::IConstExprContext *ctx)
{
    int i = std::stoi(ctx->i->getText());
    Value *v = builder->getInt32(i);
    return v;
}

std::any CodegenVisitor::visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx)
{
    return ctx->arrayAccess()->accept(this);
}

std::any CodegenVisitor::visitSConstExpr(WPLParser::SConstExprContext *ctx)
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

std::any CodegenVisitor::visitUnaryExpr(WPLParser::UnaryExprContext *ctx)
{
    switch (ctx->op->getType())
    {
    case WPLParser::MINUS:
    {
        Value *innerVal = std::any_cast<Value *>(ctx->ex->accept(this));
        Value *v = builder->CreateNSWSub(builder->getInt32(0), innerVal);
        return v;
    }

    // FIXME: REVIEW
    case WPLParser::NOT:
    {
        Value *v = std::any_cast<Value *>(ctx->ex->accept(this));
        v = builder->CreateNot(v);
        return v;
    }
    }

    errorHandler.addCodegenError(ctx->getStart(), "Unknown unary operator: " + ctx->op->getText());

    return nullptr;
}

std::any CodegenVisitor::visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx)
{
    Value *lhs = std::any_cast<Value *>(ctx->left->accept(this));
    Value *rhs = std::any_cast<Value *>(ctx->right->accept(this));

    switch (ctx->op->getType())
    {
    case WPLParser::PLUS:
        return builder->CreateNSWAdd(lhs, rhs);
    case WPLParser::MINUS:
        return builder->CreateNSWSub(lhs, rhs);
    case WPLParser::MULTIPLY:
        return builder->CreateNSWMul(lhs, rhs);
    case WPLParser::DIVIDE:
        return builder->CreateSDiv(lhs, rhs);
    }

    errorHandler.addCodegenError(ctx->getStart(), "Unknown arith op: " + ctx->op->getText());
    return nullptr;
}

std::any CodegenVisitor::visitEqExpr(WPLParser::EqExprContext *ctx)
{
    // FIXME: VERIFY GOOD ENOUGH! PROBS WONT WORK ON STRINGS!!!
    Value *lhs = std::any_cast<Value *>(ctx->left->accept(this));
    Value *rhs = std::any_cast<Value *>(ctx->right->accept(this));

    switch (ctx->op->getType())
    {
    case WPLParser::EQUAL:
    {
        Value *v1 = builder->CreateICmpEQ(lhs, rhs);
        Value *v = builder->CreateZExtOrTrunc(v1, Int32Ty);
        return v;
    }

    case WPLParser::NOT_EQUAL:
    {
        Value *v1 = builder->CreateICmpNE(lhs, rhs);
        Value *v = builder->CreateZExtOrTrunc(v1, Int32Ty);
        return v;
    }
    }

    errorHandler.addCodegenError(ctx->getStart(), "Unknown equality operator: " + ctx->op->getText());
    return nullptr;
}

std::any CodegenVisitor::visitLogAndExpr(WPLParser::LogAndExprContext *ctx)
{
    Value *lhs = std::any_cast<Value *>(ctx->left->accept(this));
    Value *rhs = std::any_cast<Value *>(ctx->right->accept(this));

    Value *IR = builder->CreateAnd(lhs, rhs);
    Value *v = builder->CreateZExtOrTrunc(IR, Int1Ty);
    return v; // FIXME: VERIFY!!!
}

std::any CodegenVisitor::visitLogOrExpr(WPLParser::LogOrExprContext *ctx)
{
    Value *lhs = std::any_cast<Value *>(ctx->left->accept(this));
    Value *rhs = std::any_cast<Value *>(ctx->right->accept(this));

    Value *IR = builder->CreateOr(lhs, rhs);
    Value *v = builder->CreateZExtOrTrunc(IR, Int1Ty);
    return v; // FIXME: VERIFY!!!
}

std::any CodegenVisitor::visitCallExpr(WPLParser::CallExprContext *ctx)
{
    return ctx->call->accept(this);
}

std::any CodegenVisitor::visitVariableExpr(WPLParser::VariableExprContext *ctx)
{
    // FIXME: should probably methodize...
    std::string id = ctx->v->getText();
    Symbol *sym = props->getBinding(ctx);

    if (!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Undefined variable access: " + id);
        return nullptr; // FIXME: these nulptrs are causing bad anycasts.
    }

    // FIXME: ADD TYPES
    Value *v = builder->CreateLoad(sym->type->getLLVMType(module->getContext()), sym->val, id);

    return v;
}

std::any CodegenVisitor::visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitFieldAccessExpr");
    return nullptr;
}

std::any CodegenVisitor::visitParenExpr(WPLParser::ParenExprContext *ctx)
{
    return ctx->ex->accept(this); // FIXME: VERIFY GOOD ENOUGH
}

std::any CodegenVisitor::visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx)
{
    Value *lhs = std::any_cast<Value *>(ctx->left->accept(this));
    Value *rhs = std::any_cast<Value *>(ctx->right->accept(this));

    Value *v1;

    switch (ctx->op->getType())
    {
    case WPLParser::LESS:
        v1 = builder->CreateICmpSLT(lhs, rhs);
        break;
    case WPLParser::LESS_EQ:
        v1 = builder->CreateICmpSLE(lhs, rhs); // FIXME: VERIFY
        break;
    case WPLParser::GREATER:
        v1 = builder->CreateICmpSGT(lhs, rhs);
        break;
    case WPLParser::GREATER_EQ:
        v1 = builder->CreateICmpSGE(lhs, rhs); // FIXME: VERIFY
        break;

    default:
        errorHandler.addCodegenError(ctx->getStart(), "Unknown rel operator: " + ctx->op->getText());
        return nullptr;
    }

    Value *v = builder->CreateZExtOrTrunc(v1, Int1Ty);
    return v;
}

std::any CodegenVisitor::visitBConstExpr(WPLParser::BConstExprContext *ctx)
{
    return ctx->booleanConst()->accept(this);
}

std::any CodegenVisitor::visitCondition(WPLParser::ConditionContext *ctx)
{
    // Based on https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl05.html
    // FIXME: VERIFY, This might not actually get a boolean well enough....
    return ctx->ex->accept(this);
}

std::any CodegenVisitor::visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitSelectAlternative");
    return nullptr;
}

std::any CodegenVisitor::visitExternStatement(WPLParser::ExternStatementContext *ctx)
{
    // FIXME: VARIADIC - Should be OK here now...
    std::vector<llvm::Type *> typeVec;

    if (ctx->paramList)
    {
        for (auto e : ctx->paramList->params)
        {
            llvm::Type *type = std::any_cast<llvm::Type *>(e->ty->accept(this));
            typeVec.push_back(type);
        }
    }
    std::cout << "HERE" << std::endl;

    ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);
    bool isVariadic = ctx->variadic || ctx->ELLIPSIS();

    std::cout << "HERE2" << std::endl;

    FunctionType *fnType = FunctionType::get(
        std::any_cast<llvm::Type *>(ctx->ty->accept(this)), // Int32Ty, //ctx->ty->accept(this), // FIXME: DO BETTER
        paramRef,
        isVariadic);
    std::cout << "HERE3" << std::endl;
    // FIXME: VERIFY CORRECT!
    Function *fn = Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);
    std::cout << "HERE4" << std::endl;
    // errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitExternStatement");
    return nullptr;
}

std::any CodegenVisitor::visitFuncDef(WPLParser::FuncDefContext *ctx)
{

    std::vector<llvm::Type *> typeVec;

    Symbol *sym = props->getBinding(ctx);
    if (!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unbound function: " + ctx->name->getText());
        return nullptr;
    }

    const TypeInvoke *inv = dynamic_cast<const TypeInvoke *>(sym->type);

    /*
        for(const Type * t : inv->getParamTypes())
        {
            //FIXME: NEED A WAY TO get the type!!!
            typeVec.push_back(t);
        }
    */

    if (ctx->paramList)
    {
        for (auto e : ctx->paramList->params)
        {
            llvm::Type *type = std::any_cast<llvm::Type *>(e->ty->accept(this));
            typeVec.push_back(type);
        }
    }
    ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);

    FunctionType *fnType = FunctionType::get(
        std::any_cast<llvm::Type *>(ctx->ty->accept(this)), // Int32Ty, //ctx->ty->accept(this), // FIXME: DO BETTER
        paramRef,
        false);

    // FIXME: VERIFY CORRECT!
    Function *fn = Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);

    // Create block
    BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", fn); // FIXME: USING ENTRY MAY BE AN ISSUE?
    builder->SetInsertPoint(bBlk);


    std::any last = nullptr; 
    for (auto e : ctx->block()->stmts)
    {
        last = e->accept(this);
    }

    if(ctx->block()->stmts.size() > 0 && dynamic_cast<WPLParser::ReturnStatementContext*>(ctx->block()->stmts.at(ctx->block()->stmts.size() - 1)))
    {
        // builder->CreateRet(
        //     std::any_cast<Value *>(last)
        // ); 
    }

    // FIXME: VERIFY ENOUGH, NOTHING FOLLOWING, ETC. THIS IS PROBS WRONG!

    return nullptr;
}

std::any CodegenVisitor::visitProcDef(WPLParser::ProcDefContext *ctx)
{
    // FIXME: VERIFY + METHODIZE
    std::vector<llvm::Type *> typeVec;

    Symbol *sym = props->getBinding(ctx);
    if (!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unbound function: " + ctx->name->getText());
        return nullptr;
    }

    const TypeInvoke *inv = dynamic_cast<const TypeInvoke *>(sym->type);

    /*
        for(const Type * t : inv->getParamTypes())
        {
            //FIXME: NEED A WAY TO get the type!!!
            typeVec.push_back(t);
        }
    */

    if (ctx->paramList)
    {
        for (auto e : ctx->paramList->params)
        {
            llvm::Type *type = std::any_cast<llvm::Type *>(e->ty->accept(this));
            typeVec.push_back(type);
        }
    }
    ArrayRef<llvm::Type *> paramRef = ArrayRef(typeVec);

    FunctionType *fnType = FunctionType::get(
        VoidTy,
        paramRef,
        false);

    // FIXME: VERIFY CORRECT!
    Function *fn = Function::Create(fnType, GlobalValue::ExternalLinkage, ctx->name->getText(), module);

    // Create block
    BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", fn); // FIXME: USING ENTRY MAY BE AN ISSUE?
    builder->SetInsertPoint(bBlk);

    // std::any last = nullptr; 

    for (auto e : ctx->block()->stmts)
    {
        e->accept(this);
    }

    if(ctx->block()->stmts.size() > 0 && dynamic_cast<WPLParser::ReturnStatementContext*>(ctx->block()->stmts.at(ctx->block()->stmts.size() - 1)))
    {
        // builder->CreateRetVoid(); 
    }

    // FIXME: VERIFY ENOUGH, NOTHING FOLLOWING, ETC. THIS IS PROBS WRONG!

    return nullptr;
}

std::any CodegenVisitor::visitAssignStatement(WPLParser::AssignStatementContext *ctx)
{
    // FIXME: Might not work perfectly due to no arrays/vars yet.... or strings...
    Value *exprVal = std::any_cast<Value *>(ctx->ex->accept(this));

    // if (ctx->to->VARIABLE())
    // {
    Symbol *varSym = props->getBinding(ctx->to);

    if (varSym == nullptr)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Incorrectly processed variable in assignment: " + ctx->to->getText());
        return nullptr;
    }
    // Shouldn't need this in the end....
    if (varSym->val == nullptr)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized variable in assignment: " + ctx->to->getText() + "@" + varSym->identifier);
        std::cout << "IMPROP VAR @ " << &varSym << std::endl;
        return nullptr;
    }

    Value *val = varSym->val;

    if (!ctx->to->VARIABLE())
    {
        // Dealing with an array //FIXME: REFACTOR!!!
        Value *index = std::any_cast<Value *>(ctx->to->array->index->accept(this));
        val = builder->CreateGEP(val, {Int32Zero, index});
        // val = std::any_cast<Value *>(ctx->to->array->accept(this));
    }

    builder->CreateStore(exprVal, val);
    // }
    // else
    // {
    // Array access
    // FIXME: DO BETER

    // }
    return nullptr;
}

std::any CodegenVisitor::visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    // FIXME: MODIFY TO DO THINGS BY TYPE!!!!

    for (auto e : ctx->assignments)
    {
        // FIXME: DOESNT WORK WHEN NO VALUE!!!
        Value *exVal = (e->ex) ? std::any_cast<Value *>(e->ex->accept(this)) : nullptr; // builder->getInt32(0);
        for (auto var : e->VARIABLE())
        {
            Symbol *varSymbol = props->getBinding(var);

            if (!varSymbol)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Issue creating variable: " + var->getText());
                return nullptr; // FIXME: DO BETTER
            }

            std::cout << "493 4 " << ctx->getText() << std::endl;
            // FIXME: THIS WILL NOT WORK FOR VAR!!! (may have multiple types!)
            llvm::Type *ty = std::any_cast<llvm::Type *>(ctx->ty->accept(this));
            std::cout << "495 4 " << ctx->getText() << std::endl;
            llvm::AllocaInst *v = builder->CreateAlloca(ty, 0, var->getText());

            std::cout << "Set Val for: " << varSymbol->identifier << "(" << var->getText() << ") @" << &varSymbol << std::endl;
            varSymbol->val = v;

            if (exVal)
                builder->CreateStore(exVal, v);
        }
    }
    // FIXME: ENSURE CORRECT!!!
    return nullptr;
}

std::any CodegenVisitor::visitLoopStatement(WPLParser::LoopStatementContext *ctx)
{
    // Very similar to conditionals

    Value *check = std::any_cast<Value *>(ctx->check->accept(this));

    auto parent = builder->GetInsertBlock()->getParent();

    BasicBlock *loopBlk = BasicBlock::Create(module->getContext(), "loop");
    BasicBlock *restBlk = BasicBlock::Create(module->getContext(), "rest");

    builder->CreateCondBr(check, loopBlk, restBlk);

    // Need to add here otherwise we will overwrite it
    parent->getBasicBlockList().push_back(loopBlk);

    // In the loop block
    builder->SetInsertPoint(loopBlk);
    for (auto e : ctx->block()->stmts)
    {
        e->accept(this);
    }
    // Re-calculate the loop condition
    check = std::any_cast<Value *>(ctx->check->accept(this));
    // Check if we need to loop back again...
    loopBlk = builder->GetInsertBlock(); // FIXME: REVIEW
    builder->CreateCondBr(check, loopBlk, restBlk);

    // FIXME: ALLOW _ IN NAMES?
    //  Out of Loop

    parent->getBasicBlockList().push_back(restBlk);
    builder->SetInsertPoint(restBlk);

    // FIXME: VERIFY!!!
    return nullptr;
}

// FIXME: EXTERNS THAT ARE JUST ...!!!!
// FIXME: EXTERNS CANT HAVE A SPACE?

std::any CodegenVisitor::visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx)
{
    // FIXME: THIS MIGHT NOT WORK OUTSIDE A FUNCTION
    Value *cond = std::any_cast<Value *>(ctx->check->accept(this));

    auto parentFn = builder->GetInsertBlock()->getParent();

    BasicBlock *thenBlk = BasicBlock::Create(module->getContext(), "then", parentFn);
    BasicBlock *elseBlk = BasicBlock::Create(module->getContext(), "else");
    BasicBlock *restBlk = BasicBlock::Create(module->getContext(), "ifcont");

    // FIXME: WHAT IF NO ELSE BLOCK??

    // Builder.CreateCondBr(CondV, ThenBB, ElseBB);
    builder->CreateCondBr(cond, thenBlk, elseBlk);

    // Then block
    // parentFn->getBasicBlockList().push_back(thenBlk);
    builder->SetInsertPoint(thenBlk);
    std::any lastTrue = nullptr; 
    for (auto e : ctx->trueBlk->stmts)
    {
        //FIXME: UNSAFE WITH RETUNING NULL!
        lastTrue = e->accept(this);//std::any_cast<Value*>(e->accept(this));
    }
    // ctx->trueBlk->accept(this); // FIXME: DO BETTER, ALSO CHECK THE DOUBLING UP OF BLOCKS! & MAYBE DO A NULL CHECK!
    // FIXME: HOW WILL THIS WORK WITH RETURNS??
    if(!CodegenVisitor::blockEndsInReturn(ctx->trueBlk))
    {
        builder->CreateBr(restBlk);
    }
    
    thenBlk = builder->GetInsertBlock(); // REVIEW

    // Else block  //FIXME: THIS TREATS IT AS REQUIRED. SHOULD WE DO SOMETHING AB THIS?
    parentFn->getBasicBlockList().push_back(elseBlk);
    builder->SetInsertPoint(elseBlk);
    std::any lastFalse = nullptr; 
    for (auto e : ctx->falseBlk->stmts)
    {
        //FIXME: UNSAFE W/ RETURNING NULL; 
        lastFalse = e->accept(this);//std::any_cast<Value*>(e->accept(this));
    }
    // ctx->falseBlk->accept(this); // FIXME: SEE ABOVE COMMENTS

    if(!CodegenVisitor::blockEndsInReturn(ctx->falseBlk))
    {
        builder->CreateBr(restBlk);
    }

    elseBlk = builder->GetInsertBlock();

    // Merge back in
    parentFn->getBasicBlockList().push_back(restBlk);
    builder->SetInsertPoint(restBlk);


    std::vector<std::pair<Value*, BasicBlock*>> phis; 
std::cout << "625" << std::endl; 
    //FIXME: MAKE SURE NOTHING CAN EXIST AFTER A RET!!!
    if(ctx->trueBlk->stmts.size() != 0 && dynamic_cast<WPLParser::ReturnStatementContext*>(ctx->trueBlk->stmts.at(ctx->trueBlk->stmts.size() - 1)))
    {
        //FIXME: CHECK FOR NULL?
        Value * tRet = std::any_cast<Value *>(lastTrue);
        phis.push_back({tRet, thenBlk});
    }
std::cout << "633" << std::endl; 
    if(ctx->falseBlk->stmts.size() != 0 && dynamic_cast<WPLParser::ReturnStatementContext*>(ctx->falseBlk->stmts.at(ctx->falseBlk->stmts.size() - 1)))
    {
        Value * fRet = std::any_cast<Value*>(lastFalse);
        phis.push_back({fRet, elseBlk});
    }
    std::cout << "639" << std::endl; 
    //FIXME: NEED TO DO THIS!!
    // PHINode *PN =
    //     Builder.CreatePHI(Type::getDoubleTy(TheContext), 2, "iftmp");

    // PN->addIncoming(ThenV, ThenBB);
    // PN->addIncoming(ElseV, ElseBB);

    std::cout << "NUM PHIS: " << phis.size() << std::endl; 
    // if(phis.size() > 0)
    // {
    //     //FIXME: NEED TO USE CORRECT TYPE!!!
    //     llvm::PHINode * p = builder->CreatePHI(Int32Ty, phis.size());

    //     for(auto a : phis)
    //     {
    //         p->addIncoming(a.first, a.second);
    //     }

    //     return p; 
    // }

    // llvm::PHINode * p = builder->CreatePHI(Int32Ty, 2, "iftmp");
    // p->addIncoming()
    // FIXME: ADD PHI STUFF SO RETURNS WORK?

    // errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitConditionalStatement");
    return nullptr;
}

std::any CodegenVisitor::visitSelectStatement(WPLParser::SelectStatementContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitSelectStatement");
    return nullptr;
}

std::any CodegenVisitor::visitCallStatement(WPLParser::CallStatementContext *ctx)
{
    return ctx->call->accept(this);
}

std::any CodegenVisitor::visitReturnStatement(WPLParser::ReturnStatementContext *ctx)
{
    if (ctx->expression())
    {
        Value * v = builder->CreateRet(
         std::any_cast<Value *>(ctx->expression()->accept(this)) // FIXME: UNSAFE W/ ERORS
        );
        // FIXME: ENSURE NO FOLLOWING CODE

        return v;
    }
    Value * v = builder->CreateRetVoid();
    // FIXME: ENSURE NO FOLLOWING CODE, ENSURE CORRECT!!
    return v;
    // return nullptr; //FIXME: DO BETTER?
}

std::any CodegenVisitor::visitType(WPLParser::TypeContext *ctx)
{
    llvm::Type *ty;
    bool valid = false;

    // FIXME: VERIFY!
    if (ctx->TYPE_INT())
    {
        ty = Int32Ty;
        valid = true;
    }
    else if (ctx->TYPE_BOOL())
    {
        ty = Int1Ty;
        valid = true;
    }
    else if (ctx->TYPE_STR())
    {
        ty = i8p;
        valid = true;
    }

    if (!valid)
    {
        errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED TYPE: " + ctx->getText());
        return nullptr;
    }

    if (ctx->len)
    {
        // FIXME: ENSURE POSITIVE
        uint64_t len = (uint64_t)std::stoi(ctx->len->getText());
        llvm::Type *arr = ArrayType::get(ty, len); // new llvm::Type::ArrayType(ty, len);

        return arr;
    }

    return ty;
}

std::any CodegenVisitor::visitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    Value * val = ctx->TRUE() ? builder->getTrue() : builder->getFalse();
    return val; 
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
std::any CodegenVisitor::visitTypeOrVar(WPLParser::TypeOrVarContext *ctx)
{
    std::cout << "674 -- " << !!(ctx->type()) << " " << !!props->getBinding(ctx) << std::endl;
    if (ctx->type())
    {
        return ctx->type()->accept(this);
    }

    
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED: TYPE INFERENCE");
    return nullptr;
}

std::any CodegenVisitor::visitAssignment(WPLParser::AssignmentContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Assignment fragment should never be visited directly during codegen!");
    return nullptr;
}

std::any CodegenVisitor::visitParameterList(WPLParser::ParameterListContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown error: Codegen should not have to visits parameter list!");
    return nullptr;
}

std::any CodegenVisitor::visitParameter(WPLParser::ParameterContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown error: Codegen should not have to visit parameter!");
    return nullptr;
}

std::any CodegenVisitor::visitBlock(WPLParser::BlockContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown error: Codegen should not directly visit block!");
    return nullptr;
}

std::any CodegenVisitor::visitBlockStatement(WPLParser::BlockStatementContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown error: Codegen should not directly visit block statement!");
    return nullptr;
}