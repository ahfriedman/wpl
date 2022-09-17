#include "SemanticVisitor.h"

std::any SemanticVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    // Enter initial scope
    stmgr->enterScope();

    for (auto e : ctx->stmts)
    {
        e->accept(this);
    }

    return Types::UNDEFINED; 
}
//     std::any visitInvocation(WPLParser::InvocationContext *ctx) override;
//     std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx) override;
//     std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx) override;
std::any SemanticVisitor::visitIConstExpr(WPLParser::IConstExprContext *ctx)
{
    return Types::INT;
}

std::any SemanticVisitor::visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx) { return ctx->arrayAccess()->accept(this); }

std::any SemanticVisitor::visitSConstExpr(WPLParser::SConstExprContext *ctx)
{
    return Types::STR;
}

std::any SemanticVisitor::visitUnaryExpr(WPLParser::UnaryExprContext *ctx)
{
    Type* innerType = std::any_cast<Type*>(ctx->ex->accept(this));

    switch (ctx->op->getType())
    {
    case WPLParser::MINUS:
        if (innerType->isNot(Types::INT))
        {
            errorHandler.addSemanticError(ctx->getStart(), "INT expected in unary minus, but got " + innerType->toString());
            return Types::UNDEFINED;
        }
        break;
    case WPLParser::NOT:
        if (innerType->isNot(Types::BOOL))
        {
            errorHandler.addSemanticError(ctx->getStart(), "BOOL expected in unary not, but got " + innerType->toString());
            return Types::UNDEFINED;
        }
        break;
    }

    return innerType;
}

std::any SemanticVisitor::visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx)
{
    // Based on starter
    bool valid = true; 
    auto left = std::any_cast<Type*>(ctx->left->accept(this));
    if (left->isNot(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + left->toString());
        valid = false; 
    }
    auto right = std::any_cast<Type*>(ctx->right->accept(this));
    if (right->isNot(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + right->toString());
        valid = false; 
    }
    return (valid) ? Types::INT : Types::UNDEFINED;
}

std::any SemanticVisitor::visitEqExpr(WPLParser::EqExprContext *ctx)
{
    // FIXME: do better!
    auto right = std::any_cast<Type*>(ctx->right->accept(this));
    auto left = std::any_cast<Type*>(ctx->left->accept(this));
    if (right->isNot(left))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Both sides of '=' must have the same type");
        return Types::UNDEFINED; 
    }
    return Types::BOOL;
}
std::any SemanticVisitor::visitLogAndExpr(WPLParser::LogAndExprContext *ctx)
{
    // Based on starter //FIXME: do better!
    bool valid = true; 
    auto left = std::any_cast<Type*>(ctx->left->accept(this));
    if (left->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + left->toString());
        valid = false; 
    }
    auto right = std::any_cast<Type*>(ctx->right->accept(this));
    if (right->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL right expression expected, but was " + right->toString());
        valid = false; 
    }
    return (valid) ? Types::BOOL : Types::UNDEFINED;
}
std::any SemanticVisitor::visitLogOrExpr(WPLParser::LogOrExprContext *ctx)
{
    // Based on starter //FIXME: do better!
    bool valid = true; 

    auto left = std::any_cast<Type*>(ctx->left->accept(this));
    if (left->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + left->toString());
        valid = false; 
    }
    auto right = std::any_cast<Type*>(ctx->right->accept(this));
    if (right->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL right expression expected, but was " + right->toString());
        valid = false; 
    }
    return valid ? Types::BOOL : Types::UNDEFINED;
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
        return Types::UNDEFINED;
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
    bool valid = true; 
    auto left = std::any_cast<Type*>(ctx->left->accept(this));
    if (left->isNot(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + left->toString());
        valid = false; 
    }
    auto right = std::any_cast<Type*>(ctx->right->accept(this));
    if (right->isNot(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + right->toString());
        valid = false; 
    }
    return valid ? Types::BOOL : Types::UNDEFINED;
}

std::any SemanticVisitor::visitBConstExpr(WPLParser::BConstExprContext *ctx)
{
    return Types::BOOL;
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

    return Types::UNDEFINED;
}
std::any SemanticVisitor::visitCondition(WPLParser::ConditionContext *ctx)
{
    auto conditionType = std::any_cast<Type*>(ctx->ex);

    if (conditionType->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Condition expected BOOL, but was given " + conditionType->toString());
    }

    return Types::UNDEFINED;
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
    auto exprType = std::any_cast<Type*>(ctx->ex->accept(this));

    // FIXME: DO BETTER W/ Type Inference & such
    // if (exprType == Types::UNDEFINED)
    // {
    //     errorHandler.addSemanticError(ctx->getStart(), "Expression evaluates to an Types::UNDEFINED type");
    // }

    std::string varId = ctx->to->getText();

    std::optional<Symbol *> opt = stmgr->lookup(varId);

    if (opt)
    {
        Symbol *symbol = opt.value();

        if (symbol->type->isNot(exprType))
        {
            errorHandler.addSemanticError(ctx->getStart(), "Assignment statement expected " + symbol->type->toString() + " but got " + exprType->toString());
        }
    }

    return Types::UNDEFINED; // FIXME: VERIFY
}

std::any SemanticVisitor::visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    //FIXME: need lookup in current scope!!!
    
    //FIXME: make sure this lookup checks undefined!!!
    auto assignType = std::any_cast<Type*>(ctx->typeOrVar());

    for(auto e : ctx->assignments) {
        auto exprType = std::any_cast<Type*>(e->ex->accept(this));

        if(assignType->isNot(exprType))
        {
            errorHandler.addSemanticError(e->getStart(), "Expression of type " + exprType->toString() + " cannot be assigned to " + assignType->toString());
        }

        for(auto var : e->v) 
        {
            std::string id = var->getText(); 

            std::optional<Symbol*> symOpt = stmgr->lookup(id);

            if(symOpt) {
                errorHandler.addSemanticError(e->getStart(), "Redeclaration of " + id);
            }
            else {
                Symbol* symbol = new Symbol(id, exprType); //Done with exprType for later inferencing purposes
                stmgr->addSymbol(symbol);
                // bindings->bind() //FIXME: What to do about bindings????
            }
        }
    }

    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitLoopStatement(WPLParser::LoopStatementContext *ctx)
{
    ctx->check->accept(this);
    ctx->block()->accept(this);

    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx)
{
    //FIXME:Type inference!!!
    ctx->check->accept(this); 

    ctx->trueBlk->accept(this); 

    if(ctx->falseBlk) {
        ctx->falseBlk->accept(this); 
    }

    return Types::UNDEFINED; 
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
        return Types::UNDEFINED;
    }

    return ctx->type()->accept(this);
}

std::any SemanticVisitor::visitType(WPLParser::TypeContext *ctx)
{
    if (ctx->len)
    {
        // FIXME: HANDLE BETTER
        errorHandler.addSemanticError(ctx->getStart(), "Arrays currently not supported");
        return Types::UNDEFINED;
    }

    if (ctx->TYPE_INT())
        return Types::INT;
    if (ctx->TYPE_BOOL())
        return Types::BOOL;
    if (ctx->TYPE_STR())
        return Types::STR;

    errorHandler.addSemanticError(ctx->getStart(), "Unknown type: " + ctx->getText());
    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    return Types::BOOL;
}