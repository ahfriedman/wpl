#include "SemanticVisitor.h"

std::any SemanticVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx)
{
    // Enter initial scope
    stmgr->enterScope();

    for(auto e : ctx->extens)
    {
        e->accept(this); 
    }

    for (auto e : ctx->stmts)
    {
        std::cout << "-> " << e->getText() << std::endl;
        e->accept(this);
        std::cout << "<-" << e->getText() << std::endl;
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

        if (
            (!invokeable->isVariadic() && fnParams.size() != ctx->args.size())
            || (invokeable->isVariadic() && fnParams.size() > ctx->args.size()) //FIXME: verify correct
            )
        {
            std::ostringstream errorMsg;
            errorMsg << "Invocation of " << name << " expected " << fnParams.size() << " argument(s), but got " << ctx->args.size();
            errorHandler.addSemanticError(ctx->getStart(), errorMsg.str());
            return Types::UNDEFINED;
        }

        for (unsigned int i = 0; i < ctx->args.size(); i++)//fnParams.size(); i++)
        {
            const Type *providedType = std::any_cast<const Type *>(ctx->args.at(i)->accept(this));

            if(invokeable->isVariadic() && fnParams.size() == 0) continue; //FIXME: DO BETTER, USED FOR VARIADIC

            const Type *expectedType = fnParams.at(
                i < fnParams.size() ? i : (fnParams.size() - 1)
                );

            if (providedType->isNot(expectedType))
            {
                std::ostringstream errorMsg;
                errorMsg << "Argument " << i << " provided to " << name << " expected " << expectedType->toString() << " but got " << providedType->toString();

                errorHandler.addSemanticError(ctx->getStart(), errorMsg.str());
            }
        }

        // if(invokeable->isVariadic() && fnParams.size() < ctx->args.size())
        // {
        //     const Type * expectedType = fnParams.at(fnParams.size() - 1);

        //     for(unsigned int )
        // }

        return invokeable->getReturnType().has_value() ? invokeable->getReturnType().value() : Types::UNDEFINED;
    }

    errorHandler.addSemanticError(ctx->getStart(), "Can only invoke PROC and FUNC, not " + name + " : " + sym->type->toString());
    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitArrayAccess(WPLParser::ArrayAccessContext *ctx)
{
    // FIXME: PROBABLY NEED TO DO SOMETHING HERE WITH BINDINGS INSTEAD!!!
    std::string name = ctx->var->getText();

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

std::any SemanticVisitor::visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx)
{
    if (ctx->var)
    {
        // FIXME: SAME AS VAR
        //  Based on starter
        std::string id = ctx->var->getText();

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

    //FIXME: THIS WON'T WORK AS WE'LL MISS THE BINDINGS!!!
    return ctx->array->accept(this);
}

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

std::any SemanticVisitor::visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx)
{
    const Type *ty = std::any_cast<const Type *>(ctx->ex->accept(this));

    if (const TypeArray *a = dynamic_cast<const TypeArray *>(ty))
    {
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot perform operation: " + ctx->field->getText() + " on " + ty->toString());
        return Types::UNDEFINED;
    }

    if (ctx->field->getText() != "length")
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unsupported operation on " + ty->toString() + ": " + ctx->field->getText());
        return Types::UNDEFINED;
    }

    return Types::INT; // FIXME: DO THIS WHOLE THING BETTER
}

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
    auto conditionType = std::any_cast<const Type *>(ctx->ex->accept(this));

    if (conditionType->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Condition expected BOOL, but was given " + conditionType->toString());
    }

    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx)
{
    // FIXME: VERIFY
    ctx->eval->accept(this);

    const Type *checkType = std::any_cast<const Type *>(ctx->check->accept(this));

    if (const TypeBool *b = dynamic_cast<const TypeBool *>(checkType))
    {
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Select alternative expected BOOL but got " + checkType->toString());
    }

    return Types::UNDEFINED;
}

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

std::any SemanticVisitor::visitExternStatement(WPLParser::ExternStatementContext *ctx)
{

    bool variadic = ctx->variadic || ctx->ELLIPSIS(); 

    std::string id = ctx->name->getText();

    std::optional<Symbol *> opt = stmgr->lookup(id);

    // FIXME: DO BETTER, NEED ORDERING TO CATCH ALL ERRORS (BASICALLY SEE ANY ISSUE THAT APPLIES TO PROCs)
    if (opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unsupported redeclaration of " + id);
        return Types::UNDEFINED;
    }

    // FIXME: test breaking params somehow!! like using something thats not a type!!!!
    const Type *ty = (ctx->paramList) ? std::any_cast<const Type *>(ctx->paramList->accept(this))
                                      : new TypeInvoke();

    const TypeInvoke *procType = dynamic_cast<const TypeInvoke *>(ty); // Always true, but needs separate statement to make C happy.

    // FIXME: DO BETTER
    const Type *retType = ctx->ty ? std::any_cast<const Type *>(ctx->ty->accept(this))
                                  : Types::UNDEFINED;

    const TypeInvoke *funcType = (ctx->ty) ? new TypeInvoke(procType->getParamTypes(), retType, variadic)
                                           : new TypeInvoke(procType->getParamTypes(), variadic);

    Symbol *funcSymbol = new Symbol(id, funcType);

    stmgr->addSymbol(funcSymbol);

    return Types::UNDEFINED;
};

std::any SemanticVisitor::visitFuncDef(WPLParser::FuncDefContext *ctx)
{
    std::string funcId = ctx->name->getText();

    // FIXME: NEEDS TO BE LOCAL SCOPE ONLY AND THEN NEEDS TO COMPARE TYPES (OR JUST GLOBAL SCOPE)

    std::optional<Symbol *> opt = stmgr->lookup(funcId);

    // FIXME: DO BETTER, NEED ORDERING TO CATCH ALL ERRORS (BASICALLY SEE ANY ISSUE THAT APPLIES TO PROCs)
    if (opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unsupported redeclaration of " + funcId);
        return Types::UNDEFINED;
    }

    // FIXME: test breaking params somehow!! like using something thats not a type!!!!
    const Type *ty = (ctx->paramList) ? std::any_cast<const Type *>(ctx->paramList->accept(this))
                                      : new TypeInvoke();

    const TypeInvoke *procType = dynamic_cast<const TypeInvoke *>(ty); // Always true, but needs separate statement to make C happy.
    const Type *retType = std::any_cast<const Type *>(ctx->ty->accept(this));

    const TypeInvoke *funcType = new TypeInvoke(procType->getParamTypes(), retType);

    Symbol *funcSymbol = new Symbol(funcId, funcType);

    stmgr->addSymbol(funcSymbol);
    stmgr->enterScope(); // FIXME DOUBLING SCOPES!

    stmgr->addSymbol(new Symbol("@RETURN", retType));

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

    if (ctx->block()->stmts.size() == 0 || !dynamic_cast<WPLParser::ReturnStatementContext *>(ctx->block()->stmts.at(ctx->block()->stmts.size() - 1)))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Function must end in return statement");
    }

    // Double scope for params.... should maybe make this a function....
    stmgr->exitScope();

    bindings->bind(ctx, funcSymbol);
    
    return funcType; // FIXME: Should this return nothing?
}

std::any SemanticVisitor::visitProcDef(WPLParser::ProcDefContext *ctx)
{
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
    const Type *procType = (ctx->paramList) ? std::any_cast<const Type *>(ctx->paramList->accept(this))
                                            : new TypeInvoke();

    Symbol *procSymbol = new Symbol(procId, procType);

    stmgr->addSymbol(procSymbol);

    stmgr->enterScope(); // FIXME: we double up on scope entrances here. Is that ok?

    // Used to help manage return types //FIXME: maybe do better?
    stmgr->addSymbol(new Symbol("@RETURN", Types::UNDEFINED));

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

    bindings->bind(ctx, procSymbol);

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

    ctx->to->accept(this);

    Symbol *opt = bindings->getBinding(ctx->to);

    // std::string varId = ctx->to->getText();

    // std::optional<Symbol *> opt = stmgr->lookup(varId);

    // FIXME: need to still do body checks!!!
    if (opt)
    {
        Symbol *symbol = opt; //.value();

        if (symbol->type->isNot(exprType))
        {
            errorHandler.addSemanticError(ctx->getStart(), "Assignment statement expected " + symbol->type->toString() + " but got " + exprType->toString());
        }
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot assign to undefined variable: " + ctx->to->getText());
    }

    return Types::UNDEFINED; // FIXME: VERIFY
}

std::any SemanticVisitor::visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx)
{
    // FIXME: need lookup in current scope!!!

    // FIXME: make sure this lookup checks undefined!!!
    auto assignType = std::any_cast<const Type *>(ctx->typeOrVar()->accept(this));
    std::cout << "415" << ctx->getText() << std::endl;
    for (auto e : ctx->assignments)
    {
        auto exprType = (e->ex) ? std::any_cast<const Type *>(e->ex->accept(this)) : assignType;
        std::cout << "419" << std::endl;
        if (assignType->isNot(exprType))
        {
            errorHandler.addSemanticError(e->getStart(), "Expression of type " + exprType->toString() + " cannot be assigned to " + assignType->toString());
        }

        for (auto var : e->VARIABLE())
        {
            std::string id = var->getText();

            std::optional<Symbol *> symOpt = stmgr->lookup(id);

            if (symOpt)
            {
                errorHandler.addSemanticError(e->getStart(), "Redeclaration of " + id);
            }
            else
            {
                //FIXME: may cause issues with reusing exprType!!! VERIFY VARS DONT HAVE PARODY
                Symbol *symbol = new Symbol(id, exprType); // Done with exprType for later inferencing purposes
                stmgr->addSymbol(symbol);
                bindings->bind(var, symbol); 
                // bindings->bind() //FIXME: What to do about bindings????
            }
        }
    }
    std::cout << "570" << std::endl;
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
    // FIXME: VERIFY
    for (auto e : ctx->cases)
    {
        // FIXME: do better?
        e->accept(this);
    }

    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitCallStatement(WPLParser::CallStatementContext *ctx)
{
    return ctx->call->accept(this);
}

std::any SemanticVisitor::visitReturnStatement(WPLParser::ReturnStatementContext *ctx)
{
    // FIXME: DO BETTER!!!

    std::optional<Symbol *> sym = stmgr->lookup("@RETURN");

    if (!sym)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot use return outside of FUNC or PROC");
        return Types::UNDEFINED;
    }

    if (ctx->expression())
    {
        if (const TypeBot *b = dynamic_cast<const TypeBot *>(sym.value()->type))
        {
            const Type *valType = std::any_cast<const Type *>(ctx->expression()->accept(this));
            errorHandler.addSemanticError(ctx->getStart(), "PROC cannot return value, yet it was given a " + valType->toString() + " to return!");
            return Types::UNDEFINED;
        }
        else
        {
            const Type *valType = std::any_cast<const Type *>(ctx->expression()->accept(this));

            if (sym.value()->type->isNot(valType))
            {
                errorHandler.addSemanticError(ctx->getStart(), "Expected return type of " + sym.value()->type->toString() + " but got " + valType->toString());
                return Types::UNDEFINED;
            }

            return Types::UNDEFINED;
        }
    }
    else
    {
        if (const TypeBot *b = dynamic_cast<const TypeBot *>(sym.value()->type))
        {
            return Types::UNDEFINED;
        }

        errorHandler.addSemanticError(ctx->getStart(), "Expected to return a " + sym.value()->type->toString() + " but recieved nothing.");
        return Types::UNDEFINED;
    }

    errorHandler.addSemanticError(ctx->getStart(), "Unknown case");
    return Types::UNDEFINED;
}

std::any SemanticVisitor::visitBlockStatement(WPLParser::BlockStatementContext *ctx)
{
    return ctx->block()->accept(this);
}

std::any SemanticVisitor::visitTypeOrVar(WPLParser::TypeOrVarContext *ctx)
{
    std::cout << "495" << std::endl;
    if (!(ctx->type()))
    {
        errorHandler.addSemanticError(ctx->getStart(), "UNIMPLEMENTED: var"); // FIXME: TYPE INFERENC
        return Types::UNDEFINED;
    }
    std::cout << "500" << std::endl;
    return ctx->type()->accept(this);
}

std::any SemanticVisitor::visitType(WPLParser::TypeContext *ctx)
{

    const Type * ty = Types::UNDEFINED; 
    bool valid = false; 

    if (ctx->TYPE_INT())
    {
        ty =  Types::INT;
        valid = true; 
    }
    else if (ctx->TYPE_BOOL())
    {
        ty = Types::BOOL;
        valid = true; 
    }
    else if (ctx->TYPE_STR())
    {
        ty =  Types::STR;
        valid = true; 
    }

    if(!valid)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unknown type: " + ctx->getText());
        return Types::UNDEFINED;
    }

    if (ctx->len)
    {
        int len = std::stoi(ctx->len->getText());
        const Type * arr = new TypeArray(
            ty, len
        );
        return arr; 
    }

    return ty; 
}

std::any SemanticVisitor::visitBooleanConst(WPLParser::BooleanConstContext *ctx)
{
    return Types::BOOL;
}