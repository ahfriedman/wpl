#include "CodegenVisitor.h"

std::any CodegenVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    // External functions --- Temporary thing
    auto printf_prototype = FunctionType::get(i8p, true);
    auto printf_fn = Function::Create(printf_prototype, Function::ExternalLinkage, "printf", module);
    FunctionCallee printExpr(printf_prototype, printf_fn);

    // Main function --- FIXME: WE DON'T DO THIS IN FINAL VERSION!!!
    FunctionType *mainFuncType = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
    Function *mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main", module);

    // Create block to attach to main
    BasicBlock *bBlk = BasicBlock::Create(module->getContext(), "entry", mainFunc);
    builder->SetInsertPoint(bBlk);

    // FIXME: PROCESS EXTERNS!!!!
    for (auto e : ctx->stmts)
    {
        // Generate code for statement
        //  Value *result = std::any_cast<Value *>(e->accept(this));
        e->accept(this);

        // Log expression for debugging purposes
        auto txt = e->getText();
        StringRef formatRef = "Processed %s\n";
        auto gFormat = builder->CreateGlobalStringPtr(formatRef, "fmtStr");
        StringRef ref = txt;
        auto fmt = builder->CreateGlobalStringPtr(ref, "exprStr");
        builder->CreateCall(printf_fn, {gFormat, fmt});
    }

    builder->CreateRet(Int32Zero);
    return nullptr; // FIXME: DANGER!!
}

std::any CodegenVisitor::visitInvocation(WPLParser::InvocationContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitInvocation");
    return nullptr;
}

std::any CodegenVisitor::visitArrayAccess(WPLParser::ArrayAccessContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitArrayAccess");
    return nullptr;
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
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitSConstExpr");
    return nullptr;
}

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
        v = builder->CreateZExtOrTrunc(v, CodegenVisitor::Int1Ty);
        v = builder->CreateXor(v, Int32One);
        v = builder->CreateZExtOrTrunc(v, CodegenVisitor::Int32Ty);
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
    //FIXME: should probably methodize...
    std::string id = ctx->v->getText(); 
    Symbol * sym = props->getBinding(ctx); 

    if(!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Undefined variable access: " + id);
        return nullptr; //FIXME: these nulptrs are causing bad anycasts.
    }

    //FIXME: ADD TYPES
    Value * v = builder->CreateLoad(CodegenVisitor::Int32Ty, sym->val, id);

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

std::any CodegenVisitor::visitBlock(WPLParser::BlockContext *ctx)
{
    // errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitBlock");
    return nullptr;
}

std::any CodegenVisitor::visitCondition(WPLParser::ConditionContext *ctx)
{
    //Based on https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl05.html
    //FIXME: VERIFY, This might not actually get a boolean well enough....
    //lvm::ConstantInt::getSigned((llvm::Type::getInt1Ty(*context)),
                                    //   expr.val);
    //FIXME: DO BETTER, SHOULD PROBABLY JUST USE 1 BIT TYPES
    // return ConstantInt::get(Int1Ty, ctx->ex->accept(this));
    return ctx->ex->accept(this); 
}

std::any CodegenVisitor::visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitSelectAlternative");
    return nullptr;
}

std::any CodegenVisitor::visitParameterList(WPLParser::ParameterListContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitParameter");
    return nullptr;
}

std::any CodegenVisitor::visitParameter(WPLParser::ParameterContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitParameterList");
    return nullptr;
}

std::any CodegenVisitor::visitExternStatement(WPLParser::ExternStatementContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitExternStatement");
    return nullptr;
}

std::any CodegenVisitor::visitFuncDef(WPLParser::FuncDefContext *ctx)
{
    
    std::vector<Type*> typeVec ; 

    Symbol * sym = props->getBinding(ctx); 
    if(!sym)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Unbound function: " + ctx->name->getText());
        return nullptr; 
    }

    const TypeInvoke * inv = dynamic_cast<const TypeInvoke*>(sym->type);

    for(const Type * t : inv->getParamTypes())
    {
        typeVec.push_back(t); 
    }

    ArrayRef<Type*> paramRef = ArrayRef(typeVec); 

    FunctionType * fnType = FunctionType::get(
        ctx->ty->accept(this), //FIXME: DO BETTER

    )
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitFuncDef");
    return nullptr;
}

std::any CodegenVisitor::visitProcDef(WPLParser::ProcDefContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitProcDef");
    return nullptr;
}

std::any CodegenVisitor::visitAssignStatement(WPLParser::AssignStatementContext *ctx)
{
    //FIXME: Might not work perfectly due to no arrays/vars yet.... or strings...
    Value * exprVal = std::any_cast<Value *>(ctx->ex->accept(this));
    Symbol * varSym = props->getBinding(ctx->to);

    if(varSym == nullptr)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Incorrectly processed variable in assignment: " + ctx->to->getText());
        return nullptr; 
    }
    //Shouldn't need this in the end....
    if(varSym->val == nullptr)
    {
        errorHandler.addCodegenError(ctx->getStart(), "Improperly initialized variable in assignment: " + ctx->to->getText());
        return nullptr; 
    }

    builder->CreateStore(exprVal, varSym->val);

    // errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitAssignStatement");
    return nullptr;
}

std::any CodegenVisitor::visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    //FIXME: MODIFY TO DO THINGS BY TYPE!!!!
    
    for(auto e : ctx->assignments)
    {
        Value * exVal = std::any_cast<Value *>(e->ex->accept(this));

        for(auto var : e->VARIABLE())
        {
            Symbol *varSymbol = props->getBinding(var);

            if(!varSymbol)
            {
                errorHandler.addCodegenError(ctx->getStart(), "Issue creating variable: " + var->getText());
                return nullptr; //FIXME: DO BETTER
            }

            Value * v = builder->CreateAlloca(Int32Ty, 0, var->getText()); 
            varSymbol->val = v; 

            builder->CreateStore(exVal, v);

        }
    }
    return nullptr;
}

std::any CodegenVisitor::visitLoopStatement(WPLParser::LoopStatementContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitVarDeclStatement");
    return nullptr;
}

std::any CodegenVisitor::visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx)
{
    //FIXME: THIS MIGHT NOT WORK OUTSIDE A FUNCTION
    Value *  cond = std::any_cast<Value *>(ctx->check->accept(this)); 

    auto parentFn = builder->GetInsertBlock()->getParent(); 

    BasicBlock * thenBlk = BasicBlock::Create(module->getContext(), "then", parentFn);
    BasicBlock * elseBlk = BasicBlock::Create(module->getContext(), "else");
    BasicBlock * restBlk = BasicBlock::Create(module->getContext(), "ifcont");

    //FIXME: WHAT IF NO ELSE BLOCK??

    //Builder.CreateCondBr(CondV, ThenBB, ElseBB);
    builder->CreateCondBr(cond, thenBlk, elseBlk);

    //Then block 
    builder->SetInsertPoint(thenBlk); 
    ctx->trueBlk->accept(this); //FIXME: DO BETTER, ALSO CHECK THE DOUBLING UP OF BLOCKS! & MAYBE DO A NULL CHECK!
    //FIXME: HOW WILL THIS WORK WITH RETURNS??
    builder->CreateBr(restBlk);

    thenBlk = builder ->GetInsertBlock(); //REVIEW

    //Else block  //FIXME: THIS TREATS IT AS REQUIRED. SHOULD WE DO SOMETHING AB THIS?
    parentFn->getBasicBlockList().push_back(elseBlk); 
    builder->SetInsertPoint(elseBlk);
    ctx->falseBlk->accept(this); //FIXME: SEE ABOVE COMMENTS
    builder->CreateBr(restBlk); 

    elseBlk = builder->GetInsertBlock(); 


    //Merge back in 
    parentFn->getBasicBlockList().push_back(restBlk); 
    builder->SetInsertPoint(restBlk);
    
    //FIXME: ADD PHI STUFF SO RETURNS WORK? 

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
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitReturnStatement");
    return nullptr;
}

std::any CodegenVisitor::visitBlockStatement(WPLParser::BlockStatementContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED - visitBlockStatement");
    return nullptr;
}

std::any CodegenVisitor::visitType(WPLParser::TypeContext *ctx)
{
    // FIXME: VERIFY!
    if (ctx->TYPE_INT())
        return Int32Ty;
    if (ctx->TYPE_BOOL())
        return Int1Ty; // FIXME: DO BETTER

    errorHandler.addCodegenError(ctx->getStart(), "UNIMPLEMENTED TYPE: " + ctx->getText());
    return nullptr;
}

std::any CodegenVisitor::visitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    // Value *v = builder->getInt32(ctx->TRUE() ? 1 : 0);

    // return v;
    return ctx->TRUE() ? builder->getTrue() : builder->getFalse(); 
}


/*
 *
 * UNUSED VISITORS
 * ===============
 *
 * These are visitors which should NEVER be seen during the compilation process.  
 * 
 */

//FIXME: maybe these should be meta/compiler errors
std::any CodegenVisitor::visitTypeOrVar(WPLParser::TypeOrVarContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Unknown Error: Type information should be collected prior to codegen!");
    return nullptr; 
}

std::any CodegenVisitor::visitAssignment(WPLParser::AssignmentContext *ctx)
{
    errorHandler.addCodegenError(ctx->getStart(), "Assignment fragment should never be visited directly during codegen!");
    return nullptr;
}