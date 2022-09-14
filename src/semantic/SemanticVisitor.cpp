#include "SemanticVisitor.h"

std::any SemanticVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    // Enter initial scope
    stmgr->enterScope();

    for (auto e : ctx->stmts)
    {
        e->accept(this);
    }

    return SymbolType::UNDEFINED;
}
//     std::any visitInvocation(WPLParser::InvocationContext *ctx) override;
//     std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx) override;
//     std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx) override;
std::any SemanticVisitor::visitIConstExpr(WPLParser::IConstExprContext *ctx)
{
    return SymbolType::INT;
}

std::any SemanticVisitor::visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx) { return ctx->arrayAccess()->accept(this); }

std::any SemanticVisitor::visitSConstExpr(WPLParser::SConstExprContext *ctx)
{
    return SymbolType::STR;
}

std::any SemanticVisitor::visitUnaryExpr(WPLParser::UnaryExprContext *ctx)
{
    SymbolType innerType = std::any_cast<SymbolType>(ctx->ex->accept(this));

    switch (ctx->op->getType())
    {
    case WPLParser::MINUS:
        if (innerType != SymbolType::INT)
        {
            errorHandler.addSemanticError(ctx->getStart(), "INT expected in unary minus, but got " + Symbol::getStringFor(innerType));
            return SymbolType::UNDEFINED;
        }
        break;
    case WPLParser::NOT:
        if (innerType != SymbolType::BOOL)
        {
            errorHandler.addSemanticError(ctx->getStart(), "BOOL expected in unary not, but got " + Symbol::getStringFor(innerType));
            return SymbolType::UNDEFINED;
        }
        break;
    }

    return innerType;
}

std::any SemanticVisitor::visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx)
{
    // Based on starter
    SymbolType type = INT;
    auto left = std::any_cast<SymbolType>(ctx->left->accept(this));
    if (left != SymbolType::INT)
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + Symbol::getStringFor(left));
        type = SymbolType::UNDEFINED;
    }
    auto right = std::any_cast<SymbolType>(ctx->right->accept(this));
    if (right != SymbolType::INT)
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + Symbol::getStringFor(right));
        type = SymbolType::UNDEFINED;
    }
    return type;
}

std::any SemanticVisitor::visitEqExpr(WPLParser::EqExprContext *ctx)
{
    // FIXME: do better!
    SymbolType result = SymbolType::BOOL;
    auto right = std::any_cast<SymbolType>(ctx->right->accept(this));
    auto left = std::any_cast<SymbolType>(ctx->left->accept(this));
    if (right != left)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Both sides of '=' must have the same type");
        result = SymbolType::UNDEFINED;
    }
    return result;
}
std::any SemanticVisitor::visitLogAndExpr(WPLParser::LogAndExprContext *ctx)
{
    // Based on starter //FIXME: do better!
    SymbolType type = BOOL;
    auto left = std::any_cast<SymbolType>(ctx->left->accept(this));
    if (left != SymbolType::BOOL)
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + Symbol::getStringFor(left));
        type = SymbolType::UNDEFINED;
    }
    auto right = std::any_cast<SymbolType>(ctx->right->accept(this));
    if (right != SymbolType::BOOL)
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL right expression expected, but was " + Symbol::getStringFor(right));
        type = SymbolType::UNDEFINED;
    }
    return type;
}
std::any SemanticVisitor::visitLogOrExpr(WPLParser::LogOrExprContext *ctx)
{
    // Based on starter //FIXME: do better!
    SymbolType type = BOOL;
    auto left = std::any_cast<SymbolType>(ctx->left->accept(this));
    if (left != SymbolType::BOOL)
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + Symbol::getStringFor(left));
        type = SymbolType::UNDEFINED;
    }
    auto right = std::any_cast<SymbolType>(ctx->right->accept(this));
    if (right != SymbolType::BOOL)
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL right expression expected, but was " + Symbol::getStringFor(right));
        type = SymbolType::UNDEFINED;
    }
    return type;
}

std::any SemanticVisitor::visitCallExpr(WPLParser::CallExprContext *ctx)
{
    return ctx->call->accept(this);
}

std::any SemanticVisitor::visitVariableExpr(WPLParser::VariableExprContext *ctx)
{
    // Based on starter
    std::string id = ctx->v->getText();

    std::optional<Symbol *> opt = stmgr->lookup(id);

    if (!opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Undefined variable in expression: " + id);
        return SymbolType::UNDEFINED;
    }

    Symbol *symbol = opt.value();

    bindings->bind(ctx, symbol);
    return symbol->type;
}
//     std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx) override;
std::any SemanticVisitor::visitParenExpr(WPLParser::ParenExprContext *ctx)
{
    return ctx->ex->accept(this);
}

std::any SemanticVisitor::visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx)
{
    // Based on starter //FIXME: do better!
    SymbolType type = BOOL;
    auto left = std::any_cast<SymbolType>(ctx->left->accept(this));
    if (left != SymbolType::INT)
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + Symbol::getStringFor(left));
        type = SymbolType::UNDEFINED;
    }
    auto right = std::any_cast<SymbolType>(ctx->right->accept(this));
    if (right != SymbolType::INT)
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + Symbol::getStringFor(right));
        type = SymbolType::UNDEFINED;
    }
    return type;
}

std::any SemanticVisitor::visitBConstExpr(WPLParser::BConstExprContext *ctx)
{
    return SymbolType::BOOL;
}

std::any SemanticVisitor::visitBlock(WPLParser::BlockContext *ctx)
{
    // FIXME: Probably have to do better with things like type inference!!!
    stmgr->enterScope();

    for (auto e : ctx->stmts)
    {
        e->accept(this);
    }

    stmgr->exitScope();

    return SymbolType::UNDEFINED;
}
std::any SemanticVisitor::visitCondition(WPLParser::ConditionContext *ctx)
{
    SymbolType conditionType = std::any_cast<SymbolType>(ctx->ex);

    if (conditionType != SymbolType::BOOL)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Condition expected BOOL, but was given " + Symbol::getStringFor(conditionType));
    }

    return UNDEFINED;
}
//     std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override;
//     std::any visitParameterList(WPLParser::ParameterListContext *ctx) override;
//     std::any visitParameter(WPLParser::ParameterContext *ctx) override;
//     std::any visitAssignment(WPLParser::AssignmentContext *ctx) override;
//     std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override;
//     std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override;
//     std::any visitProcDef(WPLParser::ProcDefContext *ctx) override;
std::any SemanticVisitor::visitAssignStatement(WPLParser::AssignStatementContext *ctx)
{
    // This one is the update one!
    SymbolType exprType = std::any_cast<SymbolType>(ctx->ex->accept(this));

    // FIXME: DO BETTER W/ Type Inference & such
    if (exprType == UNDEFINED)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Expression evaluates to an UNDEFINED type");
    }

    std::string varId = ctx->to->getText();

    std::optional<Symbol *> opt = stmgr->lookup(varId);

    if (opt)
    {
        Symbol *symbol = opt.value();

        if (symbol->type != exprType)
        {
            errorHandler.addSemanticError(ctx->getStart(), "Assignment statement expected " + Symbol::getStringFor(symbol->type) + " but got " + Symbol::getStringFor(exprType));
        }
    }

    return UNDEFINED; // FIXME: VERIFY
}
//     std::any visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx) override;
std::any SemanticVisitor::visitLoopStatement(WPLParser::LoopStatementContext *ctx)
{
    ctx->check->accept(this);
    ctx->block()->accept(this);

    return SymbolType::UNDEFINED;
}

std::any SemanticVisitor::visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx)
{
    //FIXME:Type inference!!!
    ctx->check->accept(this); 

    ctx->trueBlk->accept(this); 

    if(ctx->falseBlk) {
        ctx->falseBlk->accept(this); 
    }

    return SymbolType::UNDEFINED; 
}

//     std::any visitSelectStatement(WPLParser::SelectStatementContext *ctx) override;
std::any SemanticVisitor::visitCallStatement(WPLParser::CallStatementContext *ctx)
{
    return ctx->call->accept(this);
}
//     std::any visitReturnStatement(WPLParser::ReturnStatementContext *ctx) override;

std::any SemanticVisitor::visitBlockStatement(WPLParser::BlockStatementContext *ctx)
{
    return ctx->block()->accept(this);
}

std::any SemanticVisitor::visitTypeOrVar(WPLParser::TypeOrVarContext *ctx)
{
    if (!(ctx->type()))
    {
        errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED: var"); // FIXME: TYPE INFERENC
        return SymbolType::UNDEFINED;
    }

    return ctx->type()->accept(this);
}

std::any SemanticVisitor::visitType(WPLParser::TypeContext *ctx)
{
    if (ctx->len)
    {
        // FIXME: HANDLE BETTER
        errorHandler.addSemanticError(ctx->getStart(), "Arrays currently not supported");
        return UNDEFINED;
    }

    if (ctx->TYPE_INT())
        return SymbolType::INT;
    if (ctx->TYPE_BOOL())
        return SymbolType::BOOL;
    if (ctx->TYPE_STR())
        return SymbolType::STR;

    errorHandler.addSemanticError(ctx->getStart(), "Unknown type: " + ctx->getText());
    return SymbolType::UNDEFINED;
}

std::any SemanticVisitor::visitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    return SymbolType::BOOL;
}