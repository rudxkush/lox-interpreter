#include "callable.h"
#include "interpreter.h"
#include "../parser/ast.h"
#include "environment.h"
#include "../common/error.h"

LoxFunction::LoxFunction(FunctionStmt& declaration, std::shared_ptr<Environment> closure, bool isInitializer)
    : declaration(&declaration), closure(closure), isInitializer(isInitializer) {}

int LoxFunction::arity() {
    return declaration->params.size();
}

Value LoxFunction::call(Interpreter& interpreter, std::vector<Value>& arguments) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
    
    for (size_t i = 0; i < declaration->params.size(); i++) {
        environment->define(declaration->params[i].lexeme, arguments[i]);
    }
    
    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch (const ReturnException& returnValue) {
        if (isInitializer) return closure->getAt(0, "this");
        return returnValue.value;
    }
    
    if (isInitializer) return closure->getAt(0, "this");
    return Value();
}

std::string LoxFunction::toString() const {
    return "<fn " + declaration->name.lexeme + ">";
}

std::string LoxFunction::getType() const {
    return "function";
}

std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
    std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
    environment->define("this", Value(instance));
    return std::make_shared<LoxFunction>(*declaration, environment, isInitializer);
}

NativeFunction::NativeFunction(const std::string& name, int arity, Value (*function)(int, Value*))
    : arity_(arity), function(function), name(name) {}

Value NativeFunction::call(Interpreter& interpreter, std::vector<Value>& arguments) {
    (void)interpreter; // Suppress unused parameter warning
    return function(arguments.size(), arguments.data());
}