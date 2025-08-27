#pragma once

#include <unordered_map>
#include <memory>
#include "../common/value.h"
#include "../common/token.h"

class Environment : public std::enable_shared_from_this<Environment> {
private:
    std::shared_ptr<Environment> enclosing;
    std::unordered_map<std::string, Value> values;

public:
    Environment();
    explicit Environment(std::shared_ptr<Environment> enclosing);
    
    void define(const std::string& name, const Value& value);
    Value get(const Token& name);
    void assign(const Token& name, const Value& value);
    Value getAt(int distance, const std::string& name);
    void assignAt(int distance, const Token& name, const Value& value);
    
    std::shared_ptr<Environment> ancestor(int distance);
};