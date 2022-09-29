#pragma once
#include "antlr4-runtime.h"
#include "WPLBaseVisitor.h"
#include "STManager.h"
#include "PropertyManager.h"
#include "WPLErrorHandler.h"

#include "TypeVisitor.h"

class SemanticVisitor : WPLBaseVisitor
{

public:
    SemanticVisitor(STManager *s, PropertyManager *p)
    {
        stmgr = s;
        bindings = p;
    }

    std::string getErrors() { return errorHandler.errorList(); }
    STManager *getSTManager() { return stmgr; }
    PropertyManager *getBindings() { return bindings; }
    bool hasErrors() { return errorHandler.hasErrors(); }

    const Type *visitCtx(WPLParser::CompilationUnitContext *ctx);
    const Type *visitCtx(WPLParser::InvocationContext *ctx);
    const Type *visitCtx(WPLParser::ArrayAccessContext *ctx);
    const Type *visitCtx(WPLParser::ArrayOrVarContext *ctx);
    const Type *visitCtx(WPLParser::IConstExprContext *ctx);
    const Type *visitCtx(WPLParser::ArrayAccessExprContext *ctx);
    const Type *visitCtx(WPLParser::SConstExprContext *ctx);
    const Type *visitCtx(WPLParser::UnaryExprContext *ctx);
    const Type *visitCtx(WPLParser::BinaryArithExprContext *ctx);
    const Type *visitCtx(WPLParser::EqExprContext *ctx);
    const Type *visitCtx(WPLParser::LogAndExprContext *ctx);
    const Type *visitCtx(WPLParser::LogOrExprContext *ctx);
    const Type *visitCtx(WPLParser::CallExprContext *ctx);
    const Type *visitCtx(WPLParser::VariableExprContext *ctx);
    const Type *visitCtx(WPLParser::FieldAccessExprContext *ctx);
    const Type *visitCtx(WPLParser::ParenExprContext *ctx);
    const Type *visitCtx(WPLParser::BinaryRelExprContext *ctx);
    const Type *visitCtx(WPLParser::BConstExprContext *ctx);
    const Type *visitCtx(WPLParser::BlockContext *ctx);
    const Type *visitCtx(WPLParser::ConditionContext *ctx);
    const Type *visitCtx(WPLParser::SelectAlternativeContext *ctx);
    const Type *visitCtx(WPLParser::ParameterListContext *ctx);
    const Type *visitCtx(WPLParser::ParameterContext *ctx);
        const Type * visitCtx(WPLParser::AssignmentContext *ctx) ;
    const Type *visitCtx(WPLParser::ExternStatementContext *ctx);
    const Type *visitCtx(WPLParser::FuncDefContext *ctx);
    const Type *visitCtx(WPLParser::ProcDefContext *ctx);
    const Type *visitCtx(WPLParser::AssignStatementContext *ctx);
    const Type *visitCtx(WPLParser::VarDeclStatementContext *ctx);
    const Type *visitCtx(WPLParser::LoopStatementContext *ctx);
    const Type *visitCtx(WPLParser::ConditionalStatementContext *ctx);
    const Type *visitCtx(WPLParser::SelectStatementContext *ctx);
    const Type *visitCtx(WPLParser::CallStatementContext *ctx);
    const Type *visitCtx(WPLParser::ReturnStatementContext *ctx);
    const Type *visitCtx(WPLParser::BlockStatementContext *ctx);
    const Type *visitCtx(WPLParser::TypeOrVarContext *ctx);
    const Type *visitCtx(WPLParser::TypeContext *ctx);
    const Type *visitCtx(WPLParser::BooleanConstContext *ctx);


    std::any visitCompilationUnit(WPLParser::CompilationUnitContext *ctx) override { return visitCtx(ctx); }
    std::any visitInvocation(WPLParser::InvocationContext *ctx) override { return visitCtx(ctx); }
    std::any visitArrayAccess(WPLParser::ArrayAccessContext *ctx) override { return visitCtx(ctx); }
    std::any visitArrayOrVar(WPLParser::ArrayOrVarContext *ctx) override { return visitCtx(ctx); }
    std::any visitIConstExpr(WPLParser::IConstExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitArrayAccessExpr(WPLParser::ArrayAccessExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitSConstExpr(WPLParser::SConstExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitUnaryExpr(WPLParser::UnaryExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitEqExpr(WPLParser::EqExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitLogAndExpr(WPLParser::LogAndExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitLogOrExpr(WPLParser::LogOrExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitCallExpr(WPLParser::CallExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitVariableExpr(WPLParser::VariableExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitFieldAccessExpr(WPLParser::FieldAccessExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitParenExpr(WPLParser::ParenExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBConstExpr(WPLParser::BConstExprContext *ctx) override { return visitCtx(ctx); }
    std::any visitBlock(WPLParser::BlockContext *ctx) override { return visitCtx(ctx); }
    std::any visitCondition(WPLParser::ConditionContext *ctx) override { return visitCtx(ctx); }
    std::any visitSelectAlternative(WPLParser::SelectAlternativeContext *ctx) override { return visitCtx(ctx); }
    std::any visitParameterList(WPLParser::ParameterListContext *ctx) override { return visitCtx(ctx); }
    std::any visitParameter(WPLParser::ParameterContext *ctx) override { return visitCtx(ctx); }
    std::any visitAssignment(WPLParser::AssignmentContext *ctx) override { return visitCtx(ctx); }
    std::any visitExternStatement(WPLParser::ExternStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitFuncDef(WPLParser::FuncDefContext *ctx) override { return visitCtx(ctx); }
    std::any visitProcDef(WPLParser::ProcDefContext *ctx) override { return visitCtx(ctx); }
    std::any visitAssignStatement(WPLParser::AssignStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitVarDeclStatement(WPLParser::VarDeclStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitLoopStatement(WPLParser::LoopStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitConditionalStatement(WPLParser::ConditionalStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitSelectStatement(WPLParser::SelectStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitCallStatement(WPLParser::CallStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitReturnStatement(WPLParser::ReturnStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitBlockStatement(WPLParser::BlockStatementContext *ctx) override { return visitCtx(ctx); }
    std::any visitTypeOrVar(WPLParser::TypeOrVarContext *ctx) override { return visitCtx(ctx); }
    std::any visitType(WPLParser::TypeContext *ctx) override { return visitCtx(ctx); }
    std::any visitBooleanConst(WPLParser::BooleanConstContext *ctx) override { return visitCtx(ctx); }

    const Type* safeVisitBlock(WPLParser::BlockContext* ctx, bool newScope)
    {
        if(newScope)
            stmgr->enterScope();

        bool foundReturn = false; 
        for(auto e : ctx->stmts)
        {

            e->accept(this);

            if(foundReturn) {
                errorHandler.addSemanticError(ctx->getStart(), "Dead code.");
                break; 
            }
            if(dynamic_cast<WPLParser::ReturnStatementContext *>(e)) foundReturn = true; 

            if(dynamic_cast<WPLParser::FuncDefContext*>(e) || dynamic_cast<WPLParser::ProcDefContext*>(e))
            {
                errorHandler.addSemanticError(ctx->getStart(), "Currenly, nested PROC/FUNCs are not supported by codegen.");
            }
        }

        if(newScope)
            this->safeExitScope(ctx); 

        return Types::UNDEFINED; 
    }

    //FIXME: USES NULLS!
    const Type * visitInvokeable(antlr4::ParserRuleContext * ctx, std::string funcId, WPLParser::ParameterListContext *paramList, WPLParser::TypeContext * ty, WPLParser::BlockContext * block)
    {
        // FIXME: NEEDS TO BE LOCAL SCOPE ONLY AND THEN NEEDS TO COMPARE TYPES (OR JUST GLOBAL SCOPE)

        std::optional<Symbol *> opt = stmgr->lookup(funcId);

        // FIXME: DO BETTER, NEED ORDERING TO CATCH ALL ERRORS (BASICALLY SEE ANY ISSUE THAT APPLIES TO PROCs)
        if (opt)
        {
            errorHandler.addSemanticError(ctx->getStart(), "Unsupported redeclaration of " + funcId);
            return Types::UNDEFINED;
        }

        // FIXME: test breaking params somehow!! like using something thats not a type!!!!
        const Type* tmpTy = (paramList) ? visitCtx(paramList) : new TypeInvoke(); 

        const TypeInvoke * procType = dynamic_cast<const TypeInvoke*>(tmpTy); // Always true, but needs separate statement to make C happy.
        const Type *retType = ty ? this->visitCtx(ty)
                                 : Types::UNDEFINED; 

        const TypeInvoke * funcType = ty ? new TypeInvoke(procType->getParamTypes(), retType) 
                                         : procType;

        Symbol * funcSymbol = new Symbol(funcId, funcType);

        stmgr->addSymbol(funcSymbol);
        stmgr->enterScope(); // FIXME DOUBLING SCOPES!

        stmgr->addSymbol(new Symbol("@RETURN", retType));

        // FIXME: we double up work here b/c we essentially get the type twice....
        if (paramList)
        {
            for (auto param : paramList->params)
            {
                const Type *paramType = this->visitCtx(param->ty);
                Symbol *paramSymbol = new Symbol(param->name->getText(), paramType);

                stmgr->addSymbol(paramSymbol);
            }
        }

        //FIXME: TEST THAT WE CAN'T REDEFINE ARGS!
        this->safeVisitBlock(block, false);

        if(ty && (block->stmts.size() == 0 || !dynamic_cast<WPLParser::ReturnStatementContext *>(block->stmts.at(block->stmts.size() - 1))))
        {
            errorHandler.addSemanticError(ctx->getStart(), "Function must end in return statement");
        }

        // Double scope for params.... should maybe make this a function....
        // stmgr->exitScope();
        safeExitScope(ctx);

        bindings->bind(ctx, funcSymbol);


        return funcType;
    }

private:
    STManager *stmgr;
    PropertyManager *bindings;
    WPLErrorHandler errorHandler;

    //FIXME: TEST UNERLYING FNS!!!
    std::optional<Scope*> safeExitScope(antlr4::ParserRuleContext * ctx) {
        std::optional<Scope*> res = stmgr->exitScope(); 

        if(res)
        {
            Scope* scope = res.value(); 
            std::vector<const Symbol*> uninf = scope->getUninferred(); 

            if(uninf.size() > 0)
            {
                std::ostringstream details;

                for(auto e : uninf)
                {
                    details << e->toString() << "; ";
                }

                errorHandler.addSemanticError(ctx->getStart(), "Uninferred types in context: " + details.str());
            }
            
        }

        return res; 
    }
};