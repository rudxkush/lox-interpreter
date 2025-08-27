#include "ast.h"

// Expression accept methods
Value BinaryExpr::accept(ExprVisitor& visitor) {
    return visitor.visitBinaryExpr(*this);
}

Value GroupingExpr::accept(ExprVisitor& visitor) {
    return visitor.visitGroupingExpr(*this);
}

Value LiteralExpr::accept(ExprVisitor& visitor) {
    return visitor.visitLiteralExpr(*this);
}

Value UnaryExpr::accept(ExprVisitor& visitor) {
    return visitor.visitUnaryExpr(*this);
}

Value VariableExpr::accept(ExprVisitor& visitor) {
    return visitor.visitVariableExpr(*this);
}

Value AssignExpr::accept(ExprVisitor& visitor) {
    return visitor.visitAssignExpr(*this);
}

Value LogicalExpr::accept(ExprVisitor& visitor) {
    return visitor.visitLogicalExpr(*this);
}

Value CallExpr::accept(ExprVisitor& visitor) {
    return visitor.visitCallExpr(*this);
}

Value GetExpr::accept(ExprVisitor& visitor) {
    return visitor.visitGetExpr(*this);
}

Value SetExpr::accept(ExprVisitor& visitor) {
    return visitor.visitSetExpr(*this);
}

Value ThisExpr::accept(ExprVisitor& visitor) {
    return visitor.visitThisExpr(*this);
}

Value SuperExpr::accept(ExprVisitor& visitor) {
    return visitor.visitSuperExpr(*this);
}

// Statement accept methods
void ExpressionStmt::accept(StmtVisitor& visitor) {
    visitor.visitExpressionStmt(*this);
}

void PrintStmt::accept(StmtVisitor& visitor) {
    visitor.visitPrintStmt(*this);
}

void VarStmt::accept(StmtVisitor& visitor) {
    visitor.visitVarStmt(*this);
}

void BlockStmt::accept(StmtVisitor& visitor) {
    visitor.visitBlockStmt(*this);
}

void IfStmt::accept(StmtVisitor& visitor) {
    visitor.visitIfStmt(*this);
}

void WhileStmt::accept(StmtVisitor& visitor) {
    visitor.visitWhileStmt(*this);
}

void FunctionStmt::accept(StmtVisitor& visitor) {
    visitor.visitFunctionStmt(*this);
}

void ReturnStmt::accept(StmtVisitor& visitor) {
    visitor.visitReturnStmt(*this);
}

void ClassStmt::accept(StmtVisitor& visitor) {
    visitor.visitClassStmt(*this);
}