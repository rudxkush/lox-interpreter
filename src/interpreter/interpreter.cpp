#include "interpreter.h"
#include "../common/error.h"
#include <iostream>

Interpreter::Interpreter() {
    globals = std::make_shared<Environment>();
    environment = globals;
}

void Interpreter::interpret(std::vector<std::unique_ptr<Stmt>>& statements) {
    try {
        for (auto& statement : statements) {
            execute(*statement);
        }
    } catch (const RuntimeError& error) {
        ErrorReporter::runtimeError(error);
    }
}

Value Interpreter::evaluate(Expr& expr) {
    return expr.accept(*this);
}

void Interpreter::execute(Stmt& stmt) {
    stmt.accept(*this);
}

Value Interpreter::visitLiteralExpr(LiteralExpr& expr) {
    return expr.value;
}

Value Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    return evaluate(*expr.expression);
}

Value Interpreter::visitUnaryExpr(UnaryExpr& expr) {
    Value right = evaluate(*expr.right);
    
    switch (expr.operator_.type) {
        case TokenType::MINUS:
            checkNumberOperand(expr.operator_, right);
            return Value(-right.asNumber());
        case TokenType::BANG:
            return Value(!isTruthy(right));
        default:
            return Value(); // Unreachable
    }
}

Value Interpreter::visitBinaryExpr(BinaryExpr& expr) {
    Value left = evaluate(*expr.left);
    Value right = evaluate(*expr.right);
    
    switch (expr.operator_.type) {
        case TokenType::GREATER:
            checkNumberOperands(expr.operator_, left, right);
            return Value(left.asNumber() > right.asNumber());
        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr.operator_, left, right);
            return Value(left.asNumber() >= right.asNumber());
        case TokenType::LESS:
            checkNumberOperands(expr.operator_, left, right);
            return Value(left.asNumber() < right.asNumber());
        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr.operator_, left, right);
            return Value(left.asNumber() <= right.asNumber());
        case TokenType::BANG_EQUAL:
            return Value(!isEqual(left, right));
        case TokenType::EQUAL_EQUAL:
            return Value(isEqual(left, right));
        case TokenType::MINUS:
            checkNumberOperands(expr.operator_, left, right);
            return Value(left.asNumber() - right.asNumber());
        case TokenType::PLUS:
            if (left.isNumber() && right.isNumber()) {
                return Value(left.asNumber() + right.asNumber());
            }
            if (left.isString() || right.isString()) {
                return Value(stringify(left) + stringify(right));
            }
            throw RuntimeError(expr.operator_, "Operands must be two numbers or two strings.");
        case TokenType::SLASH:
            checkNumberOperands(expr.operator_, left, right);
            return Value(left.asNumber() / right.asNumber());
        case TokenType::STAR:
            checkNumberOperands(expr.operator_, left, right);
            return Value(left.asNumber() * right.asNumber());
        default:
            return Value(); // Unreachable
    }
}

void Interpreter::visitExpressionStmt(ExpressionStmt& stmt) {
    evaluate(*stmt.expression);
}

void Interpreter::visitPrintStmt(PrintStmt& stmt) {
    Value value = evaluate(*stmt.expression);
    std::cout << stringify(value) << std::endl;
}

bool Interpreter::isTruthy(const Value& value) {
    return value.isTruthy();
}

bool Interpreter::isEqual(const Value& a, const Value& b) {
    return a.isEqual(b);
}

void Interpreter::checkNumberOperand(const Token& operator_, const Value& operand) {
    if (!operand.isNumber()) {
        throw RuntimeError(operator_, "Operand must be a number.");
    }
}

void Interpreter::checkNumberOperands(const Token& operator_, const Value& left, const Value& right) {
    if (!left.isNumber() || !right.isNumber()) {
        throw RuntimeError(operator_, "Operands must be numbers.");
    }
}

std::string Interpreter::stringify(const Value& value) {
    return value.toString();
}

Value Interpreter::visitVariableExpr(VariableExpr& expr) {
    return environment->get(expr.name);
}

Value Interpreter::visitAssignExpr(AssignExpr& expr) {
    Value value = evaluate(*expr.value);
    environment->assign(expr.name, value);
    return value;
}
Value Interpreter::visitLogicalExpr(LogicalExpr& expr) {
    Value left = evaluate(*expr.left);
    
    if (expr.operator_.type == TokenType::OR) {
        if (isTruthy(left)) return left;
    } else {
        if (!isTruthy(left)) return left;
    }
    
    return evaluate(*expr.right);
}
Value Interpreter::visitCallExpr(CallExpr& expr) {
    Value callee = evaluate(*expr.callee);
    
    std::vector<Value> arguments;
    for (auto& argument : expr.arguments) {
        arguments.push_back(evaluate(*argument));
    }
    
    if (!callee.isCallable()) {
        throw RuntimeError(expr.paren, "Can only call functions and classes.");
    }
    
    std::shared_ptr<LoxCallable> function = callee.asCallable();
    if (arguments.size() != function->arity()) {
        throw RuntimeError(expr.paren, "Expected " + std::to_string(function->arity()) + 
                          " arguments but got " + std::to_string(arguments.size()) + ".");
    }
    
    return function->call(*this, arguments);
}
Value Interpreter::visitGetExpr(GetExpr& expr) { return Value(); }
Value Interpreter::visitSetExpr(SetExpr& expr) { return Value(); }
Value Interpreter::visitThisExpr(ThisExpr& expr) { return Value(); }
Value Interpreter::visitSuperExpr(SuperExpr& expr) { return Value(); }

void Interpreter::visitVarStmt(VarStmt& stmt) {
    Value value;
    if (stmt.initializer != nullptr) {
        value = evaluate(*stmt.initializer);
    }
    environment->define(stmt.name.lexeme, value);
}

void Interpreter::visitBlockStmt(BlockStmt& stmt) {
    executeBlock(stmt.statements, std::make_shared<Environment>(environment));
}

void Interpreter::visitIfStmt(IfStmt& stmt) {
    if (isTruthy(evaluate(*stmt.condition))) {
        execute(*stmt.thenBranch);
    } else if (stmt.elseBranch != nullptr) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::visitWhileStmt(WhileStmt& stmt) {
    while (isTruthy(evaluate(*stmt.condition))) {
        execute(*stmt.body);
    }
}
void Interpreter::visitFunctionStmt(FunctionStmt& stmt) {
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(stmt, environment, false);
    environment->define(stmt.name.lexeme, Value(function));
}

void Interpreter::visitReturnStmt(ReturnStmt& stmt) {
    Value value;
    if (stmt.value != nullptr) {
        value = evaluate(*stmt.value);
    }
    throw ReturnException(value);
}
void Interpreter::visitClassStmt(ClassStmt& stmt) {}
void Interpreter::resolve(Expr& expr, int depth) {}

void Interpreter::executeBlock(std::vector<std::unique_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = this->environment;
    try {
        this->environment = environment;
        for (auto& statement : statements) {
            execute(*statement);
        }
    } catch (...) {
        this->environment = previous;
        throw;
    }
    this->environment = previous;
}