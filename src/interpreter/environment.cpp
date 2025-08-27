#include "environment.h"
#include "../common/error.h"

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}

void Environment::define(const std::string& name, const Value& value) {
    values[name] = value;
}

Value Environment::get(const Token& name) {
    auto it = values.find(name.lexeme);
    if (it != values.end()) {
        return it->second;
    }
    
    if (enclosing != nullptr) {
        return enclosing->get(name);
    }
    
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, const Value& value) {
    auto it = values.find(name.lexeme);
    if (it != values.end()) {
        it->second = value;
        return;
    }
    
    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }
    
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

Value Environment::getAt(int distance, const std::string& name) {
    return ancestor(distance)->values[name];
}

void Environment::assignAt(int distance, const Token& name, const Value& value) {
    ancestor(distance)->values[name.lexeme] = value;
}

std::shared_ptr<Environment> Environment::ancestor(int distance) {
    std::shared_ptr<Environment> environment = shared_from_this();
    for (int i = 0; i < distance; i++) {
        environment = environment->enclosing;
    }
    return environment;
}