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
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitArrayAccess(WPLParser::ArrayAccessContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
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
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitSConstExpr(WPLParser::SConstExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
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

    errorHandler.addSemanticError(ctx->getStart(), "Unknown unary operator: " + ctx->op->getText());

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

    errorHandler.addSemanticError(ctx->getStart(), "Unknown arith op: " + ctx->op->getText());
    return nullptr; 
}

std::any CodegenVisitor::visitEqExpr(WPLParser::EqExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitLogAndExpr(WPLParser::LogAndExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitLogOrExpr(WPLParser::LogOrExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitCallExpr(WPLParser::CallExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitVariableExpr(WPLParser::VariableExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitParenExpr(WPLParser::ParenExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}

std::any CodegenVisitor::visitBConstExpr(WPLParser::BConstExprContext *ctx)
{
   return ctx->booleanConst()->accept(this); 
}

std::any CodegenVisitor::visitBlock(WPLParser::BlockContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitCondition(WPLParser::ConditionContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitParameterList(WPLParser::ParameterListContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitParameter(WPLParser::ParameterContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitAssignment(WPLParser::AssignmentContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitExternStatement(WPLParser::ExternStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitFuncDef(WPLParser::FuncDefContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitProcDef(WPLParser::ProcDefContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitAssignStatement(WPLParser::AssignStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitLoopStatement(WPLParser::LoopStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitSelectStatement(WPLParser::SelectStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitCallStatement(WPLParser::CallStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitReturnStatement(WPLParser::ReturnStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitBlockStatement(WPLParser::BlockStatementContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitTypeOrVar(WPLParser::TypeOrVarContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitType(WPLParser::TypeContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED");
    return nullptr;
}
std::any CodegenVisitor::visitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    Value *v = builder->getInt32(ctx->TRUE() ? 1 : 0);

    return v;
}