#pragma once

#include <memory>
#include <unordered_map>
#include "../parser/ast.h"
#include "../common/value.h"
#include "environment.h"

class ReturnException : public std::runtime_error {
public:
    Value value;
    
    ReturnException(const Value& value) 
        : std::runtime_error("return"), value(value) {}
};

// Forward declarations
class LoxCallable;
class LoxFunction;
class LoxClass;
class LoxInstance;

class Interpreter : public ExprVisitor, public StmtVisitor {
private:
    std::shared_ptr<Environment> globals;
    std::shared_ptr<Environment> environment;
    std::unordered_map<Expr*, int> locals;

    void checkNumberOperand(const Token& operator_, const Value& operand);
    void checkNumberOperands(const Token& operator_, const Value& left, const Value& right);
    bool isTruthy(const Value& value);
    bool isEqual(const Value& a, const Value& b);
    std::string stringify(const Value& value);
    Value evaluate(Expr& expr);
    void execute(Stmt& stmt);
    void resolve(Expr& expr, int depth);

public:
    Interpreter();
    
    void interpret(std::vector<std::unique_ptr<Stmt>>& statements);
    void executeBlock(std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);
    
    // Expression visitors
    Value visitBinaryExpr(BinaryExpr& expr) override;
    Value visitGroupingExpr(GroupingExpr& expr) override;
    Value visitLiteralExpr(LiteralExpr& expr) override;
    Value visitUnaryExpr(UnaryExpr& expr) override;
    Value visitVariableExpr(VariableExpr& expr) override;
    Value visitAssignExpr(AssignExpr& expr) override;
    Value visitLogicalExpr(LogicalExpr& expr) override;
    Value visitCallExpr(CallExpr& expr) override;
    Value visitGetExpr(GetExpr& expr) override;
    Value visitSetExpr(SetExpr& expr) override;
    Value visitThisExpr(ThisExpr& expr) override;
    Value visitSuperExpr(SuperExpr& expr) override;
    
    // Statement visitors
    void visitExpressionStmt(ExpressionStmt& stmt) override;
    void visitPrintStmt(PrintStmt& stmt) override;
    void visitVarStmt(VarStmt& stmt) override;
    void visitBlockStmt(BlockStmt& stmt) override;
    void visitIfStmt(IfStmt& stmt) override;
    void visitWhileStmt(WhileStmt& stmt) override;
    void visitFunctionStmt(FunctionStmt& stmt) override;
    void visitReturnStmt(ReturnStmt& stmt) override;
    void visitClassStmt(ClassStmt& stmt) override;
    
    // Resolver support
    
    // Global environment access
    std::shared_ptr<Environment> getGlobals() { return globals; }
};



// Class object
class LoxClass : public LoxCallable {
private:
    std::string name;
    std::shared_ptr<LoxClass> superclass;
    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;

public:
    LoxClass(const std::string& name, std::shared_ptr<LoxClass> superclass, 
             std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods);
    
    int arity() override;
    Value call(Interpreter& interpreter, std::vector<Value>& arguments) override;
    std::string toString() const override;
    std::string getType() const override;
    
    std::shared_ptr<LoxFunction> findMethod(const std::string& name);
};

// Instance object
class LoxInstance : public LoxObject {
private:
    std::shared_ptr<LoxClass> klass;
    std::unordered_map<std::string, Value> fields;

public:
    explicit LoxInstance(std::shared_ptr<LoxClass> klass);
    
    std::string toString() const override;
    std::string getType() const override;
    
    Value get(const Token& name);
    void set(const Token& name, const Value& value);
};