#include "SemanticVisitor.h"

const Type* SemanticVisitor::visitCtx(WPLParser::CompilationUnitContext *ctx)
{

    std::cout << ctx->getText() << std::endl;
    
    // Enter initial scope
    stmgr->enterScope();

    for(auto e : ctx->extens)
    {
        this->visitCtx(e); 
    }

    for (auto e : ctx->stmts)
    {
        e->accept(this);
    }

    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::InvocationContext *ctx)
{
    // FIXME: should probably make it so that InvokableTypes use BOT instead of optionals...

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

        return invokeable->getReturnType().has_value() ? invokeable->getReturnType().value() : Types::UNDEFINED;
    }

    errorHandler.addSemanticError(ctx->getStart(), "Can only invoke PROC and FUNC, not " + name + " : " + sym->type->toString());
    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::ArrayAccessContext *ctx)
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
            // Symbol * tempSym = new Symbol(ctx->getText(), arr->getValueType()); //FIXME: DO BETTER
            bindings->bind(ctx, sym); 
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

const Type * SemanticVisitor::visitCtx(WPLParser::ArrayOrVarContext *ctx)
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
    const Type * arrType = this->visitCtx(ctx->array);
    bindings->bind(ctx, bindings->getBinding(ctx->array)); //FIXME: DO BETTER
    return arrType;
}

const Type * SemanticVisitor::visitCtx(WPLParser::IConstExprContext *ctx) { return Types::INT; }

const Type * SemanticVisitor::visitCtx(WPLParser::ArrayAccessExprContext *ctx) { return this->visitCtx(ctx->arrayAccess()); }

const Type * SemanticVisitor::visitCtx(WPLParser::SConstExprContext *ctx) { return Types::STR; }

const Type * SemanticVisitor::visitCtx(WPLParser::UnaryExprContext *ctx)
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

/**
 * @brief Visits a Binary Arithmetic Expression ensuring LHS and RHS are INT. 
 * 
 * @param ctx The BinaryArithExprContext to Visit
 * @return const Type* INT if lhs and rhs are INT; UNDEFINED otherwise. 
 */
const Type * SemanticVisitor::visitCtx(WPLParser::BinaryArithExprContext *ctx)
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

const Type * SemanticVisitor::visitCtx(WPLParser::EqExprContext *ctx)
{
    // FIXME: do better! WILL THIS EVEN WORK FOR ARRAYS, STRINGS, ETC?
    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (right->isNot(left))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Both sides of '=' must have the same type");
        return Types::UNDEFINED;
    }
    return Types::BOOL;
}

/**
 * @brief Visits a Logical And Expression ensuring LHS and RHS are BOOL. 
 * 
 * @param ctx The LogAndExprContext to Visit
 * @return const Type* BOOL if lhs and rhs are BOOL; UNDEFINED otherwise. 
 */
const Type * SemanticVisitor::visitCtx(WPLParser::LogAndExprContext *ctx)
{
    // Based on starter 
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

/**
 * @brief Visits a Logical Or Expression ensuring LHS and RHS are BOOL. 
 * 
 * @param ctx The LogOrExprContext to Visit
 * @return const Type* BOOL if lhs and rhs are BOOL; UNDEFINED otherwise. 
 */
const Type * SemanticVisitor::visitCtx(WPLParser::LogOrExprContext *ctx)
{
    // Based on starter
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

//Passthrough to visitInvocation
const Type * SemanticVisitor::visitCtx(WPLParser::CallExprContext *ctx) { return this->visitCtx(ctx->call); }

const Type * SemanticVisitor::visitCtx(WPLParser::VariableExprContext *ctx)
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

/**
 * @brief Visits a FieldAccessExprContext---Currently limited to array lengths
 * 
 * @param ctx the FieldAccessExprContext to visit
 * @return const Type* INT if correctly used to test array length; UNDEFINED if any errors.
 */
const Type * SemanticVisitor::visitCtx(WPLParser::FieldAccessExprContext *ctx)
{
    //Determine the type of the expression we are visiting
    const Type *ty = std::any_cast<const Type *>(ctx->ex->accept(this));

    //Currently we only support arrays, so if its not an array, report an error.
    if (const TypeArray *a = dynamic_cast<const TypeArray *>(ty))
    {
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot perform operation: " + ctx->field->getText() + " on " + ty->toString());
        return Types::UNDEFINED;
    }

    //As only supported operation is length, ensure that is the requested operation
    if (ctx->field->getText() != "length")
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unsupported operation on " + ty->toString() + ": " + ctx->field->getText());
        return Types::UNDEFINED;
    }

    return Types::INT;
}

//Passthrough to expression
const Type * SemanticVisitor::visitCtx(WPLParser::ParenExprContext *ctx) { return std::any_cast<const Type*>(ctx->ex->accept(this)); }

/**
 * @brief Visits a BinaryRelational Expression ensuring both lhs and rhs are INT.
 * 
 * @param ctx The BinaryRelExprContext to visit.
 * @return const Type* BOOL if lhs and rhs INT; UNDEFINED otherwise.
 */
const Type * SemanticVisitor::visitCtx(WPLParser::BinaryRelExprContext *ctx)
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
        errorHandler.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + right->toString() + " in " + ctx->getText());
        valid = false;
    }
    return valid ? Types::BOOL : Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::BConstExprContext *ctx) { return Types::BOOL; }

const Type * SemanticVisitor::visitCtx(WPLParser::BlockContext *ctx)
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

/**
 * @brief Visits a condition's expression ensuring that it is of type BOOL. 
 * 
 * @param ctx The ConditionContext to visit
 * @return const Type* Always returns UNDEFINED as to prevent assignments
 */
const Type * SemanticVisitor::visitCtx(WPLParser::ConditionContext *ctx)
{
    auto conditionType = std::any_cast<const Type *>(ctx->ex->accept(this));

    if (conditionType->isNot(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Condition expected BOOL, but was given " + conditionType->toString());
    }

    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::SelectAlternativeContext *ctx)
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

/**
 * @brief Constructs a TypeInvoke based on the parameter types and assumes a return type of BOT.
 * 
 * @param ctx The ParameterListContext to process. 
 * @return const Type* TypeInvoke representing the parameter types. 
 */
const Type * SemanticVisitor::visitCtx(WPLParser::ParameterListContext *ctx)
{
    std::vector<const Type *> params;

    for (auto param : ctx->params)
    {
        const Type *type = this->visitCtx(param);
        params.push_back(type);
    }

    const Type *type = new TypeInvoke(params); // Needs to be two separate lines b/c of how C++ handles returns?
    return type;
}

//Passthrough to visit the inner expression
const Type * SemanticVisitor::visitCtx(WPLParser::ParameterContext *ctx) { return this->visitCtx(ctx->ty); }

//Unused
const Type * SemanticVisitor::visitCtx(WPLParser::AssignmentContext *ctx) 
{
    errorHandler.addSemanticError(ctx->getStart(), "Assignment should never be visited directly during type checking!");
    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::ExternStatementContext *ctx)
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
    const Type *ty = (ctx->paramList) ? this->visitCtx(ctx->paramList)
                                      : new TypeInvoke();

    const TypeInvoke *procType = dynamic_cast<const TypeInvoke *>(ty); // Always true, but needs separate statement to make C happy.

    // FIXME: DO BETTER
    const Type *retType = ctx->ty ? this->visitCtx(ctx->ty)
                                  : Types::UNDEFINED;

    const TypeInvoke *funcType = (ctx->ty) ? new TypeInvoke(procType->getParamTypes(), retType, variadic)
                                           : new TypeInvoke(procType->getParamTypes(), variadic);

    Symbol *funcSymbol = new Symbol(id, funcType);

    stmgr->addSymbol(funcSymbol);

    return Types::UNDEFINED;
};

const Type * SemanticVisitor::visitCtx(WPLParser::FuncDefContext *ctx)
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
    const Type *ty = (ctx->paramList) ? this->visitCtx(ctx->paramList)
                                      : new TypeInvoke();

    const TypeInvoke *procType = dynamic_cast<const TypeInvoke *>(ty); // Always true, but needs separate statement to make C happy.
    const Type *retType = this->visitCtx(ctx->ty);

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
            const Type *paramType = this->visitCtx(param->ty);
            Symbol *paramSymbol = new Symbol(param->name->getText(), paramType);

            stmgr->addSymbol(paramSymbol);
        }
    }

    this->visitCtx(ctx->block());

    if (ctx->block()->stmts.size() == 0 || !dynamic_cast<WPLParser::ReturnStatementContext *>(ctx->block()->stmts.at(ctx->block()->stmts.size() - 1)))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Function must end in return statement");
    }

    // Double scope for params.... should maybe make this a function....
    stmgr->exitScope();

    bindings->bind(ctx, funcSymbol);
    
    return funcType; // FIXME: Should this return nothing?
}

const Type * SemanticVisitor::visitCtx(WPLParser::ProcDefContext *ctx)
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
    const Type *procType = (ctx->paramList) ? this->visitCtx(ctx->paramList)
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
            const Type *paramType = this->visitCtx(param->ty);
            Symbol *paramSymbol = new Symbol(param->name->getText(), paramType);

            stmgr->addSymbol(paramSymbol);
        }
    }

    this->visitCtx(ctx->block());

    // Double scope for params.... should maybe make this a function....
    stmgr->exitScope();

    bindings->bind(ctx, procSymbol);

    return procType;
}

const Type * SemanticVisitor::visitCtx(WPLParser::AssignStatementContext *ctx)
{
    // This one is the update one!
    const Type * exprType = std::any_cast<const Type *>(ctx->ex->accept(this));

    // FIXME: DO BETTER W/ Type Inference & such
    // if (exprType == Types::UNDEFINED)
    // {
    //     errorHandler.addSemanticError(ctx->getStart(), "Expression evaluates to an Types::UNDEFINED type");
    // }

    //FIXME: IS THIS SAFE?
    const Type * type = this->visitCtx(ctx->to);
    
    // Symbol *opt = bindings->getBinding(ctx->to);

    // FIXME: need to still do body checks!!!
    if (type)
    {
        // Symbol *symbol = opt; //.value();
        if (type->isNot(exprType))
        {
            errorHandler.addSemanticError(ctx->getStart(), "Assignment statement expected " + type->toString() + " but got " + exprType->toString());
        }
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot assign to undefined variable: " + ctx->to->getText());
    }

    //Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::VarDeclStatementContext *ctx)
{
    // FIXME: need lookup in current scope!!!

    // FIXME: make sure this lookup checks undefined!!!
    const Type * assignType = this->visitCtx(ctx->typeOrVar());

    for (auto e : ctx->assignments)
    {
        auto exprType = (e->ex) ? std::any_cast<const Type *>(e->ex->accept(this)) : assignType;

        //Need to check here to prevent var = var issues... //FIXME: WHAT IF WE GOT A VAR = VAR?
        if (e->ex && assignType->isNot(exprType))
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
            }
        }
    }
    //Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::LoopStatementContext *ctx)
{
    this->visitCtx(ctx->check); //Visiting check will make sure we have a boolean condition
    this->visitCtx(ctx->block()); //Visiting block to make sure everything type checks there as well

    //Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::ConditionalStatementContext *ctx)
{
    // FIXME:Type inference!!!

    // Automatically handles checking that we have a valid condition
    this->visitCtx(ctx->check);

    //Type check the then/true block
    this->visitCtx(ctx->trueBlk);

    //If we have an else block, then type check it
    if (ctx->falseBlk)
    {
        this->visitCtx(ctx->falseBlk);
    }

    //Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

const Type * SemanticVisitor::visitCtx(WPLParser::SelectStatementContext *ctx)
{
    // FIXME: VERIFY
    for (auto e : ctx->cases)
    {
        // FIXME: do better?
        this->visitCtx(e); 
    }

    //Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

// Passthrough to visit invocation
const Type * SemanticVisitor::visitCtx(WPLParser::CallStatementContext *ctx) { return this->visitCtx(ctx->call); }

const Type * SemanticVisitor::visitCtx(WPLParser::ReturnStatementContext *ctx)
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

            //FIXME: VERIFY ORDER CORRECT
            if (valType->isNot(sym.value()->type))
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

//Passthrough to visitBlock
const Type * SemanticVisitor::visitCtx(WPLParser::BlockStatementContext *ctx) { return this->visitCtx(ctx->block()); }

const Type * SemanticVisitor::visitCtx(WPLParser::TypeOrVarContext *ctx)
{
    //If we don't have a type context, then we know that we must be doing inference
    if (!(ctx->type()))
    {
        const Type * ans = new TypeInfer(); 
        return ans; 
    }
    
    //If we do have a type, then visit that context. 
    return this->visitCtx(ctx->type());
}

const Type * SemanticVisitor::visitCtx(WPLParser::TypeContext *ctx)
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

const Type * SemanticVisitor::visitCtx(WPLParser::BooleanConstContext *ctx) { return Types::BOOL; }