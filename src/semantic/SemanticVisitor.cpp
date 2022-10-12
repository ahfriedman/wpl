#include "SemanticVisitor.h"

const Type *SemanticVisitor::visitCtx(WPLParser::CompilationUnitContext *ctx)
{
    // Enter initial scope
    stmgr->enterScope();

    // Visit externs first; they will report any errors if they have any.
    for (auto e : ctx->extens)
    {
        this->visitCtx(e);
    }

    // Visit the statements contained in the unit
    for (auto e : ctx->stmts)
    {
        if (!(dynamic_cast<WPLParser::FuncDefContext *>(e) || dynamic_cast<WPLParser::ProcDefContext *>(e) || dynamic_cast<WPLParser::VarDeclStatementContext *>(e)))
        {
            errorHandler.addSemanticCritWarning(ctx->getStart(), "Currently, only FUNC, PROC, EXTERN, and variable declarations allowed at top-level. Not: " + e->getText());
        }
        e->accept(this);
    }

    /*******************************************
     * Extra checks depending on compiler flags
     *******************************************/

    if(flags & CompilerFlags::NO_RUNTIME)
    {
        /**********************************************************
         * As there is no runtime, we need to make sure that 
         * there is NO main block and that we have a program block
         **********************************************************/

        if(stmgr->lookup("main"))
        {
            errorHandler.addSemanticError(ctx->getStart(), "When compiling with no-runtime, main is reserved!");
        }
        
        //Check that program is invokeable and correctly defined. 
        {
            std::optional<Symbol *> opt = stmgr->lookup("program");
            if(!opt) {
                errorHandler.addSemanticError(ctx->getStart(), "When compiling with no-runtime, program() must be defined!");
            }
            else 
            {
                Symbol * sym = opt.value(); 

                if(const TypeInvoke * inv = dynamic_cast<const TypeInvoke*>(sym->type))
                {
                    if(inv->getParamTypes().size() != 0)
                    {
                        errorHandler.addSemanticError(ctx->getStart(), "When compiling with no-runtime, program must not require arguments!");
                    }

                    {
                        std::optional<const Type*> retOpt = inv->getReturnType(); 

                        if(!retOpt || !dynamic_cast<const TypeInt*>(retOpt.value())) {
                            errorHandler.addSemanticError(ctx->getStart(), "When compiling with no-runtime, program() must return INT");
                        }
                    }
                }
                else 
                {
                    errorHandler.addSemanticError(ctx->getStart(), "When compiling with no-runtime, program() must be an invokable!");
                }
            }
        }
    }


    // Return UNDEFINED as this should be viewed as a statement and not something assignable
    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::InvocationContext *ctx)
{
    /*
     * Look up the symbol to make sure that it is defined
     */
    std::string name = ctx->VARIABLE()->getText();
    std::optional<Symbol *> opt = stmgr->lookup(name);

    if (!opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot invoke undefined function: " + name);
        return Types::UNDEFINED;
    }

    /*
     * Given that the symbol is defined, check that it is a FUNC/PROC (something that we can invoke)
     */
    Symbol *sym = opt.value();

    if (const TypeInvoke *invokeable = dynamic_cast<const TypeInvoke *>(sym->type))
    {
        /*
         * The symbol is something we can invoke, so check that we provide it with valid parameters
         */
        std::vector<const Type *> fnParams = invokeable->getParamTypes();

        /*
         *  If the symbol is NOT a variadic and the number of arguments we provide
         *      are not the same as the number in the invokable's definition
         *  OR the symbol IS a variadic and the number of arguments in the
         *      invokable's definition is greater than the number we provide,
         *
         * THEN we have an error as we do not provide a valid number of arguments
         * to allow for this invocation.
         */
        if (
            (!invokeable->isVariadic() && fnParams.size() != ctx->args.size()) || (invokeable->isVariadic() && fnParams.size() > ctx->args.size()))
        {
            std::ostringstream errorMsg;
            errorMsg << "Invocation of " << name << " expected " << fnParams.size() << " argument(s), but got " << ctx->args.size();
            errorHandler.addSemanticError(ctx->getStart(), errorMsg.str());
            return Types::UNDEFINED; // TODO: Could change this to the return type to catch more errors?
        }

        /*
         * Now that we have a valid number of parameters, we can make sure that
         * they have the correct types as per our arguments.
         *
         * To do this, we first loop through the number of parameters that WE provide
         * as this should be AT LEAST the same number as in the definition.
         */
        for (unsigned int i = 0; i < ctx->args.size(); i++)
        {
            // Get the type of the current argument
            const Type *providedType = std::any_cast<const Type *>(ctx->args.at(i)->accept(this));

            // If the invokable is variadic and has no specified type parameters, then we can
            // skip over subsequent checks--we just needed to run type checking on each parameter.
            if (invokeable->isVariadic() && i >= fnParams.size())//&& fnParams.size() == 0)
            {
                if (dynamic_cast<const TypeBot *>(providedType))
                {
                    errorHandler.addSemanticError(ctx->getStart(), "Cannot provide " + providedType->toString() + " to a function.");
                }
                continue;
            }

            // Loop up the expected type. This is either the type at the
            // ith index OR the last type specified by the function
            // if i > fnParams.size() as that would imply we are
            // checking a variadic
            const Type *expectedType = fnParams.at(
                i < fnParams.size() ? i : (fnParams.size() - 1));

            // If the types do not match, report an error.
            if (providedType->isNotSubtype(expectedType))
            {
                std::ostringstream errorMsg;
                errorMsg << "Argument " << i << " provided to " << name << " expected " << expectedType->toString() << " but got " << providedType->toString();

                errorHandler.addSemanticError(ctx->getStart(), errorMsg.str());
            }
        }

        // Return the type of the invokable or BOT if it has none.
        return invokeable->getReturnType();//.has_value() ? invokeable->getReturnType().value() : Types::UNDEFINED;
    }

    // Symbol was not an invokeable type, so report an error & return UNDEFINED.
    errorHandler.addSemanticError(ctx->getStart(), "Can only invoke PROC and FUNC, not " + name + " : " + sym->type->toString());
    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::ArrayAccessContext *ctx)
{
    /*
     * Check that we are provided an INT for the index.
     */

    const Type *exprType = std::any_cast<const Type *>(ctx->index->accept(this));
    if (exprType->isNotSubtype(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Array access index expected type INT but got " + exprType->toString());
    }

    /*
     * Look up the symbol and check that it is defined.
     */

    std::string name = ctx->var->getText();
    std::optional<Symbol *> opt = stmgr->lookup(name);

    if (!opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot access value from undefined array: " + name);
        return Types::UNDEFINED;
    }

    /*
     * Check that the symbol is of array type.
     */

    Symbol *sym = opt.value();
    if (const TypeArray *arr = dynamic_cast<const TypeArray *>(sym->type))
    {
        bindings->bind(ctx, sym);
        return arr->getValueType(); // Return type of array
    }

    // Report error
    errorHandler.addSemanticError(ctx->getStart(), "Cannot use array access on non-array expression " + name + " : " + sym->type->toString());
    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::ArrayOrVarContext *ctx)
{
    // Check if we are a var or an array
    if (ctx->var)
    {
        /*
         * Based on starter; Same as VAR
         *
         * Get the variable name and look it up in the symbol table
         */
        std::string id = ctx->var->getText();
        std::optional<Symbol *> opt = stmgr->lookup(id);

        // If we can't find the variable, report an error as it is undefined.
        if (!opt)
        {
            errorHandler.addSemanticError(ctx->getStart(), "Undefined variable in expression: " + id);
            return Types::UNDEFINED;
        }

        // Otherwise, get the symbol's value
        Symbol *symbol = opt.value();

        // Bind the larger context to the symbol, and return the symbol's type.
        bindings->bind(ctx, symbol);
        return symbol->type;
    }

    /*
     * As we are not a var, we must be an array access, so we must visit that context.
     */
    const Type *arrType = this->visitCtx(ctx->array);

    // Lookup the binding of the array
    Symbol *binding = bindings->getBinding(ctx->array);

    // If we didn't get a binding, report an error.
    if (!binding)
    {
        errorHandler.addSemanticError(ctx->array->getStart(), "Could not correctly bind to array access!");
        return Types::UNDEFINED;
    }

    // Otherwise, bind this context to the same binding as the array access, and return its type.
    bindings->bind(ctx, binding);
    return arrType;
}

const Type *SemanticVisitor::visitCtx(WPLParser::IConstExprContext *ctx) { return Types::INT; }

const Type *SemanticVisitor::visitCtx(WPLParser::ArrayAccessExprContext *ctx) { return this->visitCtx(ctx->arrayAccess()); }

const Type *SemanticVisitor::visitCtx(WPLParser::SConstExprContext *ctx) { return Types::STR; }

/**
 * @brief Typechecks Unary Expressions
 *
 * @param ctx The UnaryExpressionContext to type check
 * @return const Type* Returns the type of the inner expression if valid; UNDEFINED otherwise.
 */
const Type *SemanticVisitor::visitCtx(WPLParser::UnaryExprContext *ctx)
{
    // Lookup the inner type
    const Type *innerType = std::any_cast<const Type *>(ctx->ex->accept(this));

    // Switch on the operation so we can ensure that the type and operation are compatable.
    switch (ctx->op->getType())
    {
    case WPLParser::MINUS:
        if (innerType->isNotSubtype(Types::INT))
        {
            errorHandler.addSemanticError(ctx->getStart(), "INT expected in unary minus, but got " + innerType->toString());
            return Types::UNDEFINED;
        }
        break;
    case WPLParser::NOT:
        if (innerType->isNotSubtype(Types::BOOL))
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
const Type *SemanticVisitor::visitCtx(WPLParser::BinaryArithExprContext *ctx)
{
    // Based on starter
    bool valid = true;

    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (left->isNotSubtype(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + left->toString());
        valid = false;
    }

    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
    if (right->isNotSubtype(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + right->toString());
        valid = false;
    }

    return (valid) ? Types::INT : Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::EqExprContext *ctx)
{
    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (right->isNotSubtype(left))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Both sides of '=' must have the same type");
        return Types::UNDEFINED;
    }

    if(dynamic_cast<const TypeArray*>(left) || dynamic_cast<const TypeArray*>(right))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot perform equality operation on arrays; they are always seen as unequal!");
    }

    return Types::BOOL;
}

/**
 * @brief Visits a Logical And Expression ensuring LHS and RHS are BOOL.
 *
 * @param ctx The LogAndExprContext to Visit
 * @return const Type* BOOL if lhs and rhs are BOOL; UNDEFINED otherwise.
 */
const Type *SemanticVisitor::visitCtx(WPLParser::LogAndExprContext *ctx)
{
    // Based on starter
    bool valid = true;

    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (left->isNotSubtype(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + left->toString());
        valid = false;
    }

    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
    if (right->isNotSubtype(Types::BOOL))
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
const Type *SemanticVisitor::visitCtx(WPLParser::LogOrExprContext *ctx)
{
    // Based on starter
    bool valid = true;

    auto left = std::any_cast<const Type *>(ctx->left->accept(this));
    if (left->isNotSubtype(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL left expression expected, but was " + left->toString());
        valid = false;
    }

    auto right = std::any_cast<const Type *>(ctx->right->accept(this));
    if (right->isNotSubtype(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "BOOL right expression expected, but was " + right->toString());
        valid = false;
    }

    return valid ? Types::BOOL : Types::UNDEFINED;
}

// Passthrough to visitInvocation
const Type *SemanticVisitor::visitCtx(WPLParser::CallExprContext *ctx) { return this->visitCtx(ctx->call); }

const Type *SemanticVisitor::visitCtx(WPLParser::VariableExprContext *ctx)
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
const Type *SemanticVisitor::visitCtx(WPLParser::FieldAccessExprContext *ctx)
{
    // Determine the type of the expression we are visiting
    std::optional<Symbol *> opt = stmgr->lookup(ctx->VARIABLE().at(0)->getText());
    if(!opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Undefined variable reference: " + ctx->ex->getText());
        return Types::UNDEFINED;
    }

    Symbol * sym = opt.value(); 
    bindings->bind(ctx->VARIABLE().at(0), sym);

    const Type* ty = sym->type;

    // Currently we only support arrays, so if its not an array, report an error.
    if (const TypeArray *a = dynamic_cast<const TypeArray *>(ty))
    {
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot perform operation: " + ctx->field->getText() + " on " + ty->toString());
        return Types::UNDEFINED;
    }

    // As only supported operation is length, ensure that is the requested operation
    if (ctx->field->getText() != "length")
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unsupported operation on " + ty->toString() + ": " + ctx->field->getText());
        return Types::UNDEFINED;
    }

    return Types::INT;
}

// Passthrough to expression
const Type *SemanticVisitor::visitCtx(WPLParser::ParenExprContext *ctx) { return std::any_cast<const Type *>(ctx->ex->accept(this)); }

/**
 * @brief Visits a BinaryRelational Expression ensuring both lhs and rhs are INT.
 *
 * @param ctx The BinaryRelExprContext to visit.
 * @return const Type* BOOL if lhs and rhs INT; UNDEFINED otherwise.
 */
const Type *SemanticVisitor::visitCtx(WPLParser::BinaryRelExprContext *ctx)
{
    // Based on starter
    bool valid = true;

    auto left = std::any_cast<const Type *>(ctx->left->accept(this));

    if (left->isNotSubtype(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + left->toString());
        valid = false;
    }

    auto right = std::any_cast<const Type *>(ctx->right->accept(this));

    if (right->isNotSubtype(Types::INT))
    {
        errorHandler.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + right->toString() + " in " + ctx->getText());
        valid = false;
    }
    return valid ? Types::BOOL : Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::BConstExprContext *ctx) { return Types::BOOL; }

const Type *SemanticVisitor::visitCtx(WPLParser::BlockContext *ctx)
{
    return this->safeVisitBlock(ctx, true);
}

/**
 * @brief Visits a condition's expression ensuring that it is of type BOOL.
 *
 * @param ctx The ConditionContext to visit
 * @return const Type* Always returns UNDEFINED as to prevent assignments
 */
const Type *SemanticVisitor::visitCtx(WPLParser::ConditionContext *ctx)
{
    auto conditionType = std::any_cast<const Type *>(ctx->ex->accept(this));

    if (conditionType->isNotSubtype(Types::BOOL))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Condition expected BOOL, but was given " + conditionType->toString());
    }

    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::SelectAlternativeContext *ctx)
{
    //Enter the scope (needed as we may define variables or do other stuff)
    stmgr->enterScope(); 
    //Accept the evaluation context
    ctx->eval->accept(this);
    //Safe exit the scope 
    this->safeExitScope(ctx);

    /*
     *  Just make sure that we don't try to define functions and stuff in a select as that doesn't make sense (and would cause codegen issues as it stands).
     */ 
    if (dynamic_cast<WPLParser::FuncDefContext *>(ctx->eval) ||
        dynamic_cast<WPLParser::ProcDefContext *>(ctx->eval) ||
        dynamic_cast<WPLParser::VarDeclStatementContext *>(ctx->eval))
    {
        errorHandler.addSemanticError(ctx->getStart(), "Dead code: definition as select alternative.");
    }
    
    //Confirm that the check type is a boolean 
    const Type *checkType = std::any_cast<const Type *>(ctx->check->accept(this));

    if (const TypeBool *b = dynamic_cast<const TypeBool *>(checkType))
    {
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Select alternative expected BOOL but got " + checkType->toString());
    }

    //Return UNDEFINED as its a statement.
    return Types::UNDEFINED;
}

/**
 * @brief Constructs a TypeInvoke based on the parameter types and assumes a return type of BOT.
 *
 * @param ctx The ParameterListContext to process.
 * @return const Type* TypeInvoke representing the parameter types.
 */
const Type *SemanticVisitor::visitCtx(WPLParser::ParameterListContext *ctx)
{
    std::vector<const Type *> params;
    std::map<std::string, WPLParser::ParameterContext *> map; 

    for (auto param : ctx->params)
    {
        std::string name = param->name->getText(); 

        auto prevUse = map.find(name);
        if(prevUse != map.end())
        {
            errorHandler.addSemanticError(param->getStart(), "Re-use of previously defined parameter " + name + ".");
        }
        else 
        {
            map.insert({name, param});
        }

        const Type *type = this->visitCtx(param);
        params.push_back(type);
    }

    const Type *type = new TypeInvoke(params); // Needs to be two separate lines b/c of how C++ handles returns?
    return type;
}

// Passthrough to visit the inner expression
const Type *SemanticVisitor::visitCtx(WPLParser::ParameterContext *ctx) { return this->visitCtx(ctx->ty); }

const Type *SemanticVisitor::visitCtx(WPLParser::AssignmentContext *ctx)
{
    errorHandler.addSemanticError(ctx->getStart(), "Assignment should never be visited directly during type checking!");
    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::ExternStatementContext *ctx)
{

    bool variadic = ctx->variadic || ctx->ELLIPSIS();

    std::string id = ctx->name->getText();

    std::optional<Symbol *> opt = stmgr->lookup(id);

    if (opt)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unsupported redeclaration of " + id);
        // return Types::UNDEFINED;
    }
    
    const Type *ty = (ctx->paramList) ? this->visitCtx(ctx->paramList)
                                      : new TypeInvoke();

    const TypeInvoke *procType = dynamic_cast<const TypeInvoke *>(ty); // Always true, but needs separate statement to make C happy.

    const Type *retType = ctx->ty ? this->visitCtx(ctx->ty)
                                  : Types::UNDEFINED;

    const TypeInvoke *funcType = (ctx->ty) ? new TypeInvoke(procType->getParamTypes(), retType, variadic)
                                           : new TypeInvoke(procType->getParamTypes(), variadic);

    Symbol *funcSymbol = new Symbol(id, funcType);

    stmgr->addSymbol(funcSymbol);

    return Types::UNDEFINED;
};

const Type *SemanticVisitor::visitCtx(WPLParser::FuncDefContext *ctx)
{
    return this->visitInvokeable(ctx, ctx->name->getText(), ctx->paramList, ctx->ty, ctx->block());
}

const Type *SemanticVisitor::visitCtx(WPLParser::ProcDefContext *ctx)
{
    return visitInvokeable(ctx, ctx->name->getText(), ctx->paramList, nullptr, ctx->block());
}

const Type *SemanticVisitor::visitCtx(WPLParser::AssignStatementContext *ctx)
{
    // This one is the update one!

    // Determine the expression type
    const Type *exprType = std::any_cast<const Type *>(ctx->ex->accept(this));

    // Determine the expected type
    const Type *type = this->visitCtx(ctx->to);

    // If we actually have a type... (prevents things like null ptrs)
    if (type)
    {
        // Make sure that the types are compatible. Inference automatically managed here.
        if (exprType->isNotSubtype(type))
        {
            errorHandler.addSemanticError(ctx->getStart(), "Assignment statement expected " + type->toString() + " but got " + exprType->toString());
        }
    }
    else
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot assign to undefined variable: " + ctx->to->getText());
    }

    // Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::VarDeclStatementContext *ctx)
{

    for (auto e : ctx->assignments)
    {
        //Needs to happen in case we have vars
        const Type *assignType = this->visitCtx(ctx->typeOrVar());
        auto exprType = (e->ex) ? std::any_cast<const Type *>(e->ex->accept(this)) : assignType;

        if (e->ex && stmgr->isGlobalScope())
        {
            if (!(dynamic_cast<WPLParser::BConstExprContext *>(e->ex) ||
                  dynamic_cast<WPLParser::IConstExprContext *>(e->ex) ||
                  dynamic_cast<WPLParser::SConstExprContext *>(e->ex)))
            {
                errorHandler.addSemanticError(e->ex->getStart(), "Global variables must be assigned explicit constants or initialized at runtime!");
            }
        }

        // Note: This automatically performs checks to prevent issues with setting VAR = VAR
        if (e->ex && exprType->isNotSubtype(assignType))
        {
            errorHandler.addSemanticError(e->getStart(), "Expression of type " + exprType->toString() + " cannot be assigned to " + assignType->toString());
        }

        for (auto var : e->VARIABLE())
        {
            std::string id = var->getText();

            std::optional<Symbol *> symOpt = stmgr->lookupInCurrentScope(id);

            if (symOpt)
            {
                errorHandler.addSemanticError(e->getStart(), "Redeclaration of " + id);
            }
            else
            {
                //Needed to ensure vars get their own inf type
                const Type * newAssignType = this->visitCtx(ctx->typeOrVar());
                const Type * newExprType = (e->ex) ? std::any_cast<const Type *>(e->ex->accept(this)) : newAssignType;

                Symbol *symbol = new Symbol(id, newExprType, stmgr->isGlobalScope()); // Done with exprType for later inferencing purposes
                stmgr->addSymbol(symbol);
                bindings->bind(var, symbol);
            }
        }
    }
    // Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

/**
 * @brief Type checks a Loops
 *
 * @param ctx The LoopStatementContext to type check
 * @return const Type* UNDEFINED as this is a statement.
 */
const Type *SemanticVisitor::visitCtx(WPLParser::LoopStatementContext *ctx)
{
    this->visitCtx(ctx->check);   // Visiting check will make sure we have a boolean condition
    this->visitCtx(ctx->block()); // Visiting block to make sure everything type checks there as well

    // Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::ConditionalStatementContext *ctx)
{
    // Automatically handles checking that we have a valid condition
    this->visitCtx(ctx->check);

    // Type check the then/true block
    this->visitCtx(ctx->trueBlk);

    // If we have an else block, then type check it
    if (ctx->falseBlk)
    {
        this->visitCtx(ctx->falseBlk);
    }

    // Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

const Type *SemanticVisitor::visitCtx(WPLParser::SelectStatementContext *ctx)
{

    if(ctx->cases.size() < 1)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Select statement expected at least one alternative, but was given 0!");
        return Types::UNDEFINED; //Shouldn't matter as the for loop won't have anything to do 
    }
    // Here we just need to visit each of the individual cases; they each handle their own logic.
    for (auto e : ctx->cases)
    {
        this->visitCtx(e);
    }

    // Return UNDEFINED because this is a statement, and UNDEFINED cannot be assigned to anything
    return Types::UNDEFINED;
}

// Passthrough to visit invocation
const Type *SemanticVisitor::visitCtx(WPLParser::CallStatementContext *ctx) { return this->visitCtx(ctx->call); }

const Type *SemanticVisitor::visitCtx(WPLParser::ReturnStatementContext *ctx)
{
    /*
     * Lookup the @RETURN symbol which can ONLY be defined by entering FUNC/PROC
     */
    std::optional<Symbol *> sym = stmgr->lookup("@RETURN");

    // If we don't have the symbol, we're not in a place that we can return from.
    if (!sym)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Cannot use return outside of FUNC or PROC");
        return Types::UNDEFINED;
    }

    // If the return statement has an expression...
    if (ctx->expression())
    {
        // Evaluate the expression type
        const Type *valType = std::any_cast<const Type *>(ctx->expression()->accept(this));

        // If the type of the return symbol is a BOT, then we must be in a PROC and, thus, we cannot return anything
        if (const TypeBot *b = dynamic_cast<const TypeBot *>(sym.value()->type))
        {
            errorHandler.addSemanticError(ctx->getStart(), "PROC cannot return value, yet it was given a " + valType->toString() + " to return!");
            return Types::UNDEFINED;
        }

        // As the return type is not a BOT, we have to make sure that it is the correct type to return

        if (valType->isNotSubtype(sym.value()->type))
        {
            errorHandler.addSemanticError(ctx->getStart(), "Expected return type of " + sym.value()->type->toString() + " but got " + valType->toString());
            return Types::UNDEFINED;
        }

        // Valid return statement; return UNDEFINED as its a statement.
        return Types::UNDEFINED;
    }
    else
    {
        // We do not have an expression to return, so make sure that the return type is also a BOT.
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

// Passthrough to visitBlock
const Type *SemanticVisitor::visitCtx(WPLParser::BlockStatementContext *ctx) { return this->visitCtx(ctx->block()); }

const Type *SemanticVisitor::visitCtx(WPLParser::TypeOrVarContext *ctx)
{
    // If we don't have a type context, then we know that we must be doing inference
    if (!(ctx->type()))
    {
        const Type *ans = new TypeInfer();
        return ans;
    }

    // If we do have a type, then visit that context.
    return this->visitCtx(ctx->type());
}

const Type *SemanticVisitor::visitCtx(WPLParser::TypeContext *ctx)
{

    const Type *ty = Types::UNDEFINED;
    bool valid = false;

    if (ctx->TYPE_INT())
    {
        ty = Types::INT;
        valid = true;
    }
    else if (ctx->TYPE_BOOL())
    {
        ty = Types::BOOL;
        valid = true;
    }
    else if (ctx->TYPE_STR())
    {
        ty = Types::STR;
        valid = true;
    }

    if (!valid)
    {
        errorHandler.addSemanticError(ctx->getStart(), "Unknown type: " + ctx->getText());
        return Types::UNDEFINED;
    }

    if (ctx->len)
    {
        int len = std::stoi(ctx->len->getText());

        if (len < 1)
        {
            errorHandler.addSemanticError(ctx->getStart(), "Cannot initialize array with a size of less than 1!");
        }

        const Type *arr = new TypeArray(
            ty, len);
        return arr;
    }

    return ty;
}

const Type *SemanticVisitor::visitCtx(WPLParser::BooleanConstContext *ctx) { return Types::BOOL; }