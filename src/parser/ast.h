#pragma once

#include <memory>
#include <vector>
#include "../common/token.h"
#include "../common/value.h"

// Forward declarations
class ExprVisitor;
class StmtVisitor;

// Base expression class
class Expr {
public:
    virtual ~Expr() = default;
    virtual Value accept(ExprVisitor& visitor) = 0;
};

// Base statement class
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor& visitor) = 0;
};

// Expression types
class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token operator_;
    std::unique_ptr<Expr> right;
    
    BinaryExpr(std::unique_ptr<Expr> left, Token operator_, std::unique_ptr<Expr> right)
        : left(std::move(left)), operator_(operator_), right(std::move(right)) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class GroupingExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;
    
    explicit GroupingExpr(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class LiteralExpr : public Expr {
public:
    Value value;
    
    explicit LiteralExpr(Value value) : value(value) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class UnaryExpr : public Expr {
public:
    Token operator_;
    std::unique_ptr<Expr> right;
    
    UnaryExpr(Token operator_, std::unique_ptr<Expr> right)
        : operator_(operator_), right(std::move(right)) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class VariableExpr : public Expr {
public:
    Token name;
    
    explicit VariableExpr(Token name) : name(name) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class AssignExpr : public Expr {
public:
    Token name;
    std::unique_ptr<Expr> value;
    
    AssignExpr(Token name, std::unique_ptr<Expr> value)
        : name(name), value(std::move(value)) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class LogicalExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    Token operator_;
    std::unique_ptr<Expr> right;
    
    LogicalExpr(std::unique_ptr<Expr> left, Token operator_, std::unique_ptr<Expr> right)
        : left(std::move(left)), operator_(operator_), right(std::move(right)) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class CallExpr : public Expr {
public:
    std::unique_ptr<Expr> callee;
    Token paren;
    std::vector<std::unique_ptr<Expr>> arguments;
    
    CallExpr(std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class GetExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    Token name;
    
    GetExpr(std::unique_ptr<Expr> object, Token name)
        : object(std::move(object)), name(name) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class SetExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    Token name;
    std::unique_ptr<Expr> value;
    
    SetExpr(std::unique_ptr<Expr> object, Token name, std::unique_ptr<Expr> value)
        : object(std::move(object)), name(name), value(std::move(value)) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class ThisExpr : public Expr {
public:
    Token keyword;
    
    explicit ThisExpr(Token keyword) : keyword(keyword) {}
    
    Value accept(ExprVisitor& visitor) override;
};

class SuperExpr : public Expr {
public:
    Token keyword;
    Token method;
    
    SuperExpr(Token keyword, Token method) : keyword(keyword), method(method) {}
    
    Value accept(ExprVisitor& visitor) override;
};

// Statement types
class ExpressionStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;
    
    explicit ExpressionStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class PrintStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;
    
    explicit PrintStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class VarStmt : public Stmt {
public:
    Token name;
    std::unique_ptr<Expr> initializer;
    
    VarStmt(Token name, std::unique_ptr<Expr> initializer)
        : name(name), initializer(std::move(initializer)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class BlockStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
    
    IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
    
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class FunctionStmt : public Stmt {
public:
    Token name;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
    
    FunctionStmt(Token name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body)
        : name(name), params(std::move(params)), body(std::move(body)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class ReturnStmt : public Stmt {
public:
    Token keyword;
    std::unique_ptr<Expr> value;
    
    ReturnStmt(Token keyword, std::unique_ptr<Expr> value)
        : keyword(keyword), value(std::move(value)) {}
    
    void accept(StmtVisitor& visitor) override;
};

class ClassStmt : public Stmt {
public:
    Token name;
    std::unique_ptr<VariableExpr> superclass;
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    
    ClassStmt(Token name, std::unique_ptr<VariableExpr> superclass, std::vector<std::unique_ptr<FunctionStmt>> methods)
        : name(name), superclass(std::move(superclass)), methods(std::move(methods)) {}
    
    void accept(StmtVisitor& visitor) override;
};

// Visitor interfaces
class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual Value visitBinaryExpr(BinaryExpr& expr) = 0;
    virtual Value visitGroupingExpr(GroupingExpr& expr) = 0;
    virtual Value visitLiteralExpr(LiteralExpr& expr) = 0;
    virtual Value visitUnaryExpr(UnaryExpr& expr) = 0;
    virtual Value visitVariableExpr(VariableExpr& expr) = 0;
    virtual Value visitAssignExpr(AssignExpr& expr) = 0;
    virtual Value visitLogicalExpr(LogicalExpr& expr) = 0;
    virtual Value visitCallExpr(CallExpr& expr) = 0;
    virtual Value visitGetExpr(GetExpr& expr) = 0;
    virtual Value visitSetExpr(SetExpr& expr) = 0;
    virtual Value visitThisExpr(ThisExpr& expr) = 0;
    virtual Value visitSuperExpr(SuperExpr& expr) = 0;
};

class StmtVisitor {
public:
    virtual ~StmtVisitor() = default;
    virtual void visitExpressionStmt(ExpressionStmt& stmt) = 0;
    virtual void visitPrintStmt(PrintStmt& stmt) = 0;
    virtual void visitVarStmt(VarStmt& stmt) = 0;
    virtual void visitBlockStmt(BlockStmt& stmt) = 0;
    virtual void visitIfStmt(IfStmt& stmt) = 0;
    virtual void visitWhileStmt(WhileStmt& stmt) = 0;
    virtual void visitFunctionStmt(FunctionStmt& stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt& stmt) = 0;
    virtual void visitClassStmt(ClassStmt& stmt) = 0;
};