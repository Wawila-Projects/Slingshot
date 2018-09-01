#ifndef _AST_H_
#define _AST_H_

#include <string>
#include <vector>
#include <string.h>
#include <map>
#include <iostream>
#include <typeinfo>

using namespace std;

extern string codedata;

typedef struct {
    string code;
    string place;

    bool isNumeric;
    int value;

}   Code;


// EXPRESSION
class Expr {
public:
    string context;
    virtual void setContext(string context) = 0;
    virtual Code generateCode() = 0;
    virtual bool evaluate() = 0;
};

class Stmnt {   
public:
    string context;
    virtual void setContext(string context) = 0;
    virtual Code generateCode() = 0;
    virtual bool evaluate() = 0;
};

class ExprList: public Expr {
public:
    vector<Expr *> content;
    void push_back(Expr * val) { content.push_back(val); }
    Code generateCode();

    void setContext(string context) {
        for(auto& c : content) {
            c->setContext(context);
        }
    }

    bool evaluate() {
        for(auto& c : content) {
            if(!c->evaluate())
                return false;
        }

        return true;
    }
};


class StmntList: public Stmnt{
public:
    vector<Stmnt *> content;
    void push_back(Stmnt * val) { content.push_back(val); }


    void setContext(string context) {
        for(auto& c : content) {
            c->setContext(context);
        }
    }

    bool evaluate() {
        for(auto& c : content) {
            if(!c->evaluate())
                return false;
        }

        return true;
    }

    Code generateCode();
};

class EmptyExpr: public Expr {
public:
    EmptyExpr() {}

    Code generateCode() {
        Code empty;
        return empty;
    }

    void setContext(string context) {
        this->context = context;
    }    
    
    bool evaluate() { return true; }
};

class BinaryExpr: public Expr {
public: 
    BinaryExpr(Expr * expr1, Expr * expr2)
    {
        this->expr1 = expr1;
        this->expr2 = expr2;
    }

    Expr * expr1;
    Expr * expr2;

    void setContext(string context) {
        this->context = context;
        expr1->setContext(context);
        expr2->setContext(context);
    }

    bool evaluate() { return expr1->evaluate() && expr2->evaluate(); }
};

class UnaryExpr: public Expr {
public:
    UnaryExpr(Expr * expr) 
    {
        this->expr = expr;
    }

    Expr * expr;

    void setContext(string context) {
        this->context = context;
        expr->setContext(context);
    }

    bool evaluate() { return expr->evaluate(); }
};

class TernaryExpr: public Expr {
public:
    TernaryExpr(Expr * condition, Expr * truevalue, Expr * falsevalue ) {
        this->condition = condition;
        this->truevalue = truevalue;
        this->falsevalue = falsevalue;
    }

    Expr * condition;
    Expr * truevalue;
    Expr * falsevalue;


    Code generateCode();

    void setContext(string context) {
        this->context = context;
        condition->setContext(context);
        truevalue->setContext(context);
        falsevalue->setContext(context);
    }

    bool evaluate() { return condition->evaluate() && 
                             truevalue->evaluate() && 
                             falsevalue->evaluate(); }
};

class IdExpr: public Expr {
public:    
    IdExpr(string id) {
        this->id = id; 
    }

    Code generateCode();
    string id;

    void setContext(string context) {
        this->context = context;
    }

    bool evaluate();
};

class IntExpr: public Expr {
public:    
    IntExpr(int value) {
        this->value = value;
    }

    Code generateCode();
    int value;

    void setContext(string context) {
        this->context = context;
    }

    bool evaluate() { return true; }
};

class CharExpr: public Expr {
public:    
    CharExpr(char value) {
        this->value = value;
    }

    Code generateCode();
    char value;

    void setContext(string context) {
        this->context = context;
    }

    bool evaluate() { return true; }
};

class ArrayCallExpr: public Expr {
public:
    ArrayCallExpr(string name, Expr * index) {
        this->name = name;
        this->index = index;
    }

    string name;
    Expr * index;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        index->setContext(context);
    }

    bool evaluate();
};

class FunctionCallExpr: public Expr {
public:
    FunctionCallExpr(string name, ExprList * args) {
        this->name = name;
        this->args = args;
    }

    string name;
    ExprList * args;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        args->setContext(context);
    }

    bool evaluate();
};

#define DEFINE_BINARY_EXPR(name) \
class name##Expr: public BinaryExpr { \
public:                                \
    name##Expr(Expr * expr1, Expr * expr2) : BinaryExpr(expr1, expr2){ } \
    Code generateCode();                \
};

DEFINE_BINARY_EXPR(Plus);
DEFINE_BINARY_EXPR(Sub);
DEFINE_BINARY_EXPR(Mul);
DEFINE_BINARY_EXPR(Div);
DEFINE_BINARY_EXPR(Mod);

DEFINE_BINARY_EXPR(Equal);
DEFINE_BINARY_EXPR(NotEqual);
DEFINE_BINARY_EXPR(LesserThan);
DEFINE_BINARY_EXPR(GreaterThan);
DEFINE_BINARY_EXPR(LesserEqual);
DEFINE_BINARY_EXPR(GreaterEqual);

DEFINE_BINARY_EXPR(Assign);
DEFINE_BINARY_EXPR(PlusAssign);
DEFINE_BINARY_EXPR(SubAssign);
DEFINE_BINARY_EXPR(MulAssign);
DEFINE_BINARY_EXPR(DivAssign);
DEFINE_BINARY_EXPR(ModAssign);
DEFINE_BINARY_EXPR(RShiftAssign);
DEFINE_BINARY_EXPR(LShiftAssign);
DEFINE_BINARY_EXPR(XorAssign);
DEFINE_BINARY_EXPR(OrAssign);
DEFINE_BINARY_EXPR(AndAssign);

DEFINE_BINARY_EXPR(LogicalAnd);
DEFINE_BINARY_EXPR(LogicalOr);
DEFINE_BINARY_EXPR(IncOr);
DEFINE_BINARY_EXPR(Xor);
DEFINE_BINARY_EXPR(And);


DEFINE_BINARY_EXPR(LeftShift);
DEFINE_BINARY_EXPR(RightShift);

#define DEFINE_UNARY_EXPR(name) \
class name##UnExpr: public UnaryExpr { \
public:                                \
    name##UnExpr(Expr * operand) : UnaryExpr(operand){ } \
    Code generateCode();                \
};

DEFINE_UNARY_EXPR(PreInc);
DEFINE_UNARY_EXPR(PreDec);
DEFINE_UNARY_EXPR(Negative);
DEFINE_UNARY_EXPR(BitwiseNot);
DEFINE_UNARY_EXPR(Not);
DEFINE_UNARY_EXPR(PostInc);
DEFINE_UNARY_EXPR(PostDec);

class SizeOfUnExpr: public UnaryExpr { 
    public:                                
        SizeOfUnExpr(Expr * operand) : UnaryExpr(operand){ } 
        SizeOfUnExpr(Expr * operand, int type) : UnaryExpr(operand)
        {
            this->type = type;
        }

        int type;
        Code generateCode();     
        
        void setContext(string context) {
            this->context = context;
        }

        bool evaluate() { return true; }
};

// Stmnt
class EmptyStmnt: public Stmnt {
public:
    EmptyStmnt() {}

    Code generateCode() {
        Code empty;
        return empty;
    }
    void setContext(string context) {
        this->context = context;
    }

    bool evaluate() { return true; }
};

class ReturnStmnt: public Stmnt{
public:
    ReturnStmnt(Expr * value) {
        this->value = value;
    }


    Code generateCode();
    Expr * value;

    void setContext(string context) {
        this->context = context;
        if(value != NULL)
            value->setContext(context);
    }

    bool evaluate() { return value != NULL ? value->evaluate() : true; }
};

class IfStmnt: public Stmnt {
public:
    IfStmnt(Expr* condition, Stmnt * body, Stmnt * optelse)
    {
        this->condition = condition;
        this->body = body;
        this->optelse = optelse;
    }


    Code generateCode();
    Stmnt * body;
    Stmnt * optelse;
    Expr * condition;

    void setContext(string context) {
        this->context = context;
        body->setContext(context);
        condition->setContext(context);

        if(optelse != NULL)
            optelse->setContext(context);
    }

    bool evaluate() { 
                        bool validelse = true;
                        if(optelse != NULL)
                            validelse = optelse->evaluate();

                        return body->evaluate() && 
                             condition->evaluate() &&
                             validelse; 
                    }
};

class WhileStmnt: public Stmnt {
public:
    WhileStmnt(Expr * condition, Stmnt * body)
    {
        this->condition = condition;
        this->body = body;
    }
    

    Code generateCode();
    Expr* condition;
    Stmnt * body;

    void setContext(string context) {
        this->context = context;
        body->setContext(context);
        condition->setContext(context);
    }

    bool evaluate() { return body->evaluate() && 
                             condition->evaluate();
                    }
};

class ForStmnt: public Stmnt {
public: 
    ForStmnt(Expr * id, Expr * condition, Expr * change, Stmnt * body){
        this->id = id;
        this->condition = condition;
        this->change = change;
        this->body = body;
    }

    Expr * id;
    Expr * condition;
    Expr * change;
    Stmnt * body;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        body->setContext(context);
        id->setContext(context);
        condition->setContext(context);
        change->setContext(context);
    }

    bool evaluate() { return body->evaluate() && 
                             condition->evaluate() &&
                             id->evaluate() &&
                             change->evaluate();
                    }
};

class BlockStmnt: public Stmnt {
public:
    BlockStmnt(StmntList * declarations, StmntList * statements)
    {
        if(declarations == NULL) 
            this->declarations = new StmntList;            
        else 
            this->declarations = declarations;

        if(statements == NULL) 
            this->statements = new StmntList;
        else 
            this->statements = statements;
    }

    StmntList * declarations;
    StmntList * statements;


    Code generateCode();

    void setContext(string context) {
        this->context = context;
        if(declarations != NULL)
            this->declarations->setContext(this->context);

        if(statements != NULL)
            this->statements->setContext(this->context);
    }

    bool evaluate();
};

class ExpressionStmnt: public Stmnt {
public:
    ExpressionStmnt(Expr * expr) {
        this->expr = expr;
    }

    Expr * expr;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        expr->setContext(context);
    }

    bool evaluate() { return expr->evaluate(); }
    
};

class VarDeclaration: public Stmnt {
public:
    VarDeclaration(string id) {
        this->id = id;
    }

    int type;
    string id;
    Expr * value;
    

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        if(value != NULL)
            value->setContext(context);
    }

    bool evaluate();
};

/*
    TYPES:
    1 - VOID
    2 - CHAR
    3 - INT


*/

class ArrayDeclaration: public Stmnt {
public:
    ArrayDeclaration(string id, int size) {
        this->id = id;
        this->size = size;
        backup = id;
    }
    
    string backup;
    string id;
    ExprList * value;
    int size;
    int type;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        if(value != NULL)
            value->setContext(context);
    }

    bool evaluate();
};

class FuncDeclaration: public Stmnt {
public:
    FuncDeclaration(string id, StmntList * params) {
        this->id = id;
        this->params = params;
    }

    int type;
    string id;
    StmntList * params;

    Code generateCode();

    void setContext(string context) {
        this->context = id;
        params->setContext(id);
    }

    bool evaluate();
};

class FuncDefinition: public Stmnt {
public:
    FuncDefinition(int type, Stmnt * id, Stmnt * body) {
        this->type = type;
        this->id = ((FuncDeclaration *)id)->id;
        this->body = body;
    }

    int type;
    string id;
    Stmnt * body;


    void setContext(string context) {
        this->context = id;
        this->body->setContext(id);
    }

    Code generateCode();

    bool evaluate() { return body->evaluate(); }
};

class ParamDeclaration: public Stmnt {
public:
    ParamDeclaration(int type, Stmnt * decl) {
        this->type = type;
        this->decl = decl;
    }

    int type;
    Stmnt * decl;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        decl->setContext(context);
    }

    bool evaluate() { return decl->evaluate(); }
};

class Declarations: public Stmnt {
public:
    Declarations(int type, StmntList * decls) {
        this->decls = decls;

        for(auto& dec : this->decls->content) {
            if(VarDeclaration * var = dynamic_cast<VarDeclaration *>(dec))
            {
                var->type = type;
                continue;
            }

            if(ArrayDeclaration * var = dynamic_cast<ArrayDeclaration *>(dec))
            {
                var->type = type;
                continue;
            }

            if(FuncDeclaration * var = dynamic_cast<FuncDeclaration *>(dec))
            {
                var->type = type;
                continue;
            }
        }
    }

    StmntList * decls;

    void setContext(string context) {
        this->context = context;
        for(auto& c : decls->content) {
            c->setContext(context);
        }
    }
    
    Code generateCode();

    bool evaluate();
};

class TimeFunc: public Expr {
public:
    TimeFunc(int timer) {
        this->timer = timer;
    }

    int timer;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
    }

    bool evaluate() { return true; }
};

class SrandFunc: public Stmnt {
public:
    SrandFunc(Expr * param) {
        this->param = param;
    }

    Expr * param;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        param->setContext(context);
    }

    bool evaluate() { return true; }
};

class RandFunc: public Expr {
public:
    Code generateCode();
    void setContext(string context) {
        this->context = context;
    }

    bool evaluate() { return true; }
};

class PrintfFunc: public Stmnt {
public:
    PrintfFunc(char * text) {
        this->text = text;
        this->params = NULL;
    }

    PrintfFunc(char * text, ExprList * params) {
        this->text = text;
        this->params = params;
    }

    char * text;
    ExprList * params;

    Code generateCode();

    void setContext(string context) {
        this->context = context;
        if(params != NULL)
            params->setContext(context);
    }

    bool evaluate();
};

#endif