#pragma once

#include "../common/value.h"
#include <vector>

class Interpreter;

class LoxCallable : public LoxObject {
public:
    virtual int arity() = 0;
    virtual Value call(Interpreter& interpreter, std::vector<Value>& arguments) = 0;
};

class LoxFunction : public LoxCallable {
private:
    class FunctionStmt* declaration;
    std::shared_ptr<class Environment> closure;
    bool isInitializer;

public:
    LoxFunction(class FunctionStmt& declaration, std::shared_ptr<class Environment> closure, bool isInitializer = false);
    
    int arity() override;
    Value call(Interpreter& interpreter, std::vector<Value>& arguments) override;
    std::string toString() const override;
    std::string getType() const override;
    
    std::shared_ptr<LoxFunction> bind(std::shared_ptr<class LoxInstance> instance);
};

class NativeFunction : public LoxCallable {
private:
    int arity_;
    Value (*function)(int argCount, Value* args);
    std::string name;

public:
    NativeFunction(const std::string& name, int arity, Value (*function)(int, Value*));
    
    int arity() override { return arity_; }
    Value call(Interpreter& interpreter, std::vector<Value>& arguments) override;
    std::string toString() const override { return "<native fn " + name + ">"; }
    std::string getType() const override { return "function"; }
};