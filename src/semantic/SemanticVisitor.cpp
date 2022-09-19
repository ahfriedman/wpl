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

std::any SemanticVisitor::visitInvocation(WPLParser::InvocationContext *ctx)
{
    // FIXME: should probably make it so that InvokableTypes use BOT instead of optionals...
    // FIXME: Implemented variadic fns

    std::string name = ctx->VARIABLE()->getText();

    std::optional<Symbol *> opt = stmgr->lookup(name);

    if (!opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot invoke undefined function: " + name);
        return Types::UNDEFINED;
    }

    Symbol *sym = opt.value();

    if (const TypeInvoke *invokeable = dynamic_cast<const TypeInvoke *>(sym->type))
    {
        std::vector<const Type *> fnParams = invokeable->getParamTypes();

        if (fnParams.size() != ctx->args.size())
        {
            std::ostringstream errorMsg;
            errorMsg << "Invocation of " << name << " expected " << fnParams.size() << " argument(s), but got " << ctx->args.size();
            errorHandler.addSemanticError(ctx->getStart(), errorMsg.str());
            return Types::UNDEFINED;
        }

        for (unsigned int i = 0; i < fnParams.size(); i++)
        {
            const Type *providedType = std::any_cast<const Type *>(ctx->args.at(i)->accept(this));
            const Type *expectedType = fnParams.at(i);

            if (providedType->isNot(expectedType))
            {
                std::ostringstream errorMsg;
                errorMsg << "Argument " << i << " provided to " << name << " expected " << expectedType->toString() << " but got " << providedType->toString();

                errorHandler.addSemanticError(ctx->getStart(), errorMsg.str());
            }
        }

        return invokeable->getReturnType().has_value() ? invokeable->getReturnType().value() : Types::UNDEFINED;
    }

    errorHandler.addSemanticError(ctx->getStart(), "Can only invoke PROC and FUNC, not " + name + " : " + sym->type->toString());
    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitArrayAccess(WPLParser::ArrayAccessContext *ctx)
{
    std::string name = ctx->var->toString();

    const Type *exprType = std::any_cast<const Type *>(ctx->index->accept(this));
    if (exprType->isNot(Types::INT)) // FIXME: maybe have to flip these..... after all, this would allow a TOP through!
    {
        errorHandler.addSemanticError(ctx->getStart(), "Array access index expected type INT but got " + exprType->toString());
    }

    std::optional<Symbol *> opt = stmgr->lookup(name);

    if (!opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot access value from undefined array: " + name);
    }
    else
    {
        Symbol *sym = opt.value();

        if (const TypeArray *arr = dynamic_cast<const TypeArray *>(sym->type))
        {
            return arr->getValueType();
        }
        else
        {
            errorHandler.addSemanticError(ctx->getStart(), "Cannot use array access on non-array expression " + name + " : " + sym->type->toString());
        }
    }
    // FIXME: when should and shouldn't we return the wrong type intenionally?

    return Types::UNDEFINED;
}

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
    const Type *innerType = std::any_cast<const Type *>(ctx->ex->accept(this));

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
    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (left->isNot(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + left->toString());
        valid = false;
    }
    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
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
    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
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
    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (left->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + left->toString());
        valid = false;
    }
    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
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

    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (left->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + left->toString());
        valid = false;
    }
    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
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
    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (left->isNot(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + left->toString());
        valid = false;
    }
    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
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
    auto conditionType = std::any_cast<const Type *>(ctx->ex);

    if (conditionType->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Condition expected BOOL, but was given " + conditionType->toString());
    }

    return Types::UNDEFINED;
}
//     std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override;
std::any SemanticVisitor::visitParameterList(WPLParser::ParameterListContext *ctx)
{
    std::cout << "STAR PARAMLIST" << std::endl;
    std::vector<const Type *> params;

    for (auto param : ctx->params)
    {
        const Type *type = std::any_cast<const Type *>(param->accept(this));
        params.push_back(type);
    }

    std::cout << "END PARAMLIST" << std::endl;

    const Type *type = new TypeInvoke(params); // Needs to be two separate lines for sake of const?
    return type;
}

std::any SemanticVisitor::visitParameter(WPLParser::ParameterContext *ctx)
{
    return ctx->ty->accept(this);
}
//     std::any visitAssignment(WPLParser::AssignmentContext *ctx) override;
//     std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override;
//     std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override;
std::any SemanticVisitor::visitProcDef(WPLParser::ProcDefContext *ctx)
{
    std::cout << "PROC DEF" << std::endl;
    std::string procId = ctx->name->getText();

    // FIXME: NEEDS TO BE LOCAL SCOPE ONLY AND THEN NEED TO COMPARE TYPES
    std::optional<Symbol *> opt = stmgr->lookup(procId);

    // FIXME: DO BETTER, NEED OTHER ORDERING TO CATCH ALL ERRORS
    if (opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unsupported redeclaration of " + procId);
        return Types::UNDEFINED;
    }

    // FIXME: test breaking params somehow!! like using something thats not a type!!!!
    std::cout << "239" << std::endl;
    std::cout << typeid(ctx->paramList->accept(this)).name() << std::endl;
    const Type *procType = (ctx->paramList) ? std::any_cast<const Type *>(ctx->paramList->accept(this))
                                            : new TypeInvoke();
    std::cout << "242" << std::endl;
    Symbol *procSymbol = new Symbol(procId, procType);

    stmgr->addSymbol(procSymbol);

    stmgr->enterScope(); // FIXME: we double up on scope entrances here. Is that ok?

    // FIXME: we double up work here b/c we essentially get the type twice....
    if (ctx->paramList)
    {
        for (auto param : ctx->paramList->params)
        {
            const Type *paramType = std::any_cast<const Type *>(param->ty->accept(this));
            Symbol *paramSymbol = new Symbol(param->name->getText(), paramType);

            stmgr->addSymbol(paramSymbol);
        }
    }

    ctx->block()->accept(this);

    // Double scope for params.... should maybe make this a function....
    stmgr->exitScope();

    return procType;
}

std::any SemanticVisitor::visitAssignStatement(WPLParser::AssignStatementContext *ctx)
{
    // This one is the update one!
    auto exprType = std::any_cast<const Type *>(ctx->ex->accept(this));

    // FIXME: DO BETTER W/ Type Inference & such
    // if (exprType == Types::UNDEFINED)
    // {
    //     errorHandler.addSemanticError(ctx->getStart(), "Expression evaluates to an Types::UNDEFINED type");
    // }

    std::string varId = ctx->to->getText();

    std::optional<Symbol *> opt = stmgr->lookup(varId);

    // FIXME: need to still do body checks!!!
    if (opt)
    {
        Symbol *symbol = opt.value();

        if (symbol->type->isNot(exprType))
        {
            errorHandler.addSemanticError(ctx->getStart(), "Assignment statement expected " + symbol->type->toString() + " but got " + exprType->toString());
        }
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot assign to undefined variable: " + varId);
    }

    return Types::UNDEFINED; // FIXME: VERIFY
}

std::any SemanticVisitor::visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    // FIXME: need lookup in current scope!!!

    // FIXME: make sure this lookup checks undefined!!!
    auto assignType = std::any_cast<const Type *>(ctx->typeOrVar());

    for (auto e : ctx->assignments)
    {
        auto exprType = std::any_cast<const Type *>(e->ex->accept(this));

        if (assignType->isNot(exprType))
        {
            errorHandler.addSemanticError(e->getStart(), "Expression of type " + exprType->toString() + " cannot be assigned to " + assignType->toString());
        }

        for (auto var : e->v)
        {
            std::string id = var->getText();

            std::optional<Symbol *> symOpt = stmgr->lookup(id);

            if (symOpt)
            {
                errorHandler.addSemanticError(e->getStart(), "Redeclaration of " + id);
            }
            else
            {
                Symbol *symbol = new Symbol(id, exprType); // Done with exprType for later inferencing purposes
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
    // FIXME:Type inference!!!
    ctx->check->accept(this);

    ctx->trueBlk->accept(this);

    if (ctx->falseBlk)
    {
        ctx->falseBlk->accept(this);
    }

    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitSelectStatement(WPLParser::SelectStatementContext *ctx)
{
    //FIXME: VERIFY
    for(auto e : ctx->cases)
    {
        //FIXME: do better?
        e->accept(this); 
    }

    return Types::UNDEFINED;
}

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