#pragma once

#include <string>
#include <memory>
#include <variant>

// Forward declarations
class LoxObject;
class LoxFunction;
class LoxClass;
class LoxInstance;

enum class ValueType {
    NIL,
    BOOLEAN,
    NUMBER,
    STRING,
    OBJECT
};

class Value {
public:
    ValueType type;
    std::variant<std::nullptr_t, bool, double, std::string, std::shared_ptr<LoxObject>> value;

    Value() : type(ValueType::NIL), value(nullptr) {}
    Value(bool b) : type(ValueType::BOOLEAN), value(b) {}
    Value(double d) : type(ValueType::NUMBER), value(d) {}
    Value(const std::string& s) : type(ValueType::STRING), value(s) {}
    Value(std::shared_ptr<LoxObject> obj) : type(ValueType::OBJECT), value(obj) {}

    bool isTruthy() const;
    bool isEqual(const Value& other) const;
    std::string toString() const;

    // Type checking helpers
    bool isNil() const { return type == ValueType::NIL; }
    bool isBool() const { return type == ValueType::BOOLEAN; }
    bool isNumber() const { return type == ValueType::NUMBER; }
    bool isString() const { return type == ValueType::STRING; }
    bool isObject() const { return type == ValueType::OBJECT; }
    bool isCallable() const;

    // Value extraction helpers
    bool asBool() const { return std::get<bool>(value); }
    double asNumber() const { return std::get<double>(value); }
    std::string asString() const { return std::get<std::string>(value); }
    std::shared_ptr<LoxObject> asObject() const { return std::get<std::shared_ptr<LoxObject>>(value); }
    std::shared_ptr<class LoxCallable> asCallable() const;
};

// Base class for all Lox objects
class LoxObject {
public:
    virtual ~LoxObject() = default;
    virtual std::string toString() const = 0;
    virtual std::string getType() const = 0;
    
    // GC support
    bool isMarked = false;
    std::shared_ptr<LoxObject> next = nullptr;
};