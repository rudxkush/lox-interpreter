#include "value.h"
#include "../interpreter/callable.h"
#include <sstream>

bool Value::isTruthy() const {
    switch (type) {
        case ValueType::NIL: return false;
        case ValueType::BOOLEAN: return asBool();
        default: return true;
    }
}

bool Value::isEqual(const Value& other) const {
    if (type != other.type) return false;
    
    switch (type) {
        case ValueType::NIL: return true;
        case ValueType::BOOLEAN: return asBool() == other.asBool();
        case ValueType::NUMBER: return asNumber() == other.asNumber();
        case ValueType::STRING: return asString() == other.asString();
        case ValueType::OBJECT: return asObject() == other.asObject();
        default: return false;
    }
}

std::string Value::toString() const {
    switch (type) {
        case ValueType::NIL: return "nil";
        case ValueType::BOOLEAN: return asBool() ? "true" : "false";
        case ValueType::NUMBER: {
            std::ostringstream oss;
            oss << asNumber();
            return oss.str();
        }
        case ValueType::STRING: return asString();
        case ValueType::OBJECT: return asObject()->toString();
        default: return "unknown";
    }
}

bool Value::isCallable() const {
    return isObject() && asObject() && 
           (asObject()->getType() == "function" || asObject()->getType() == "class");
}

std::shared_ptr<LoxCallable> Value::asCallable() const {
    return std::dynamic_pointer_cast<LoxCallable>(asObject());
}