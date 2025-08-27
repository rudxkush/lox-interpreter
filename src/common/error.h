#pragma once

#include <stdexcept>
#include <string>
#include "token.h"

// Forward declaration
class Value;

class LoxError : public std::runtime_error {
public:
    LoxError(const std::string& message) : std::runtime_error(message) {}
};

class ParseError : public LoxError {
public:
    Token token;
    
    ParseError(const Token& token, const std::string& message)
        : LoxError(message), token(token) {}
};

class RuntimeError : public LoxError {
public:
    Token token;
    
    RuntimeError(const Token& token, const std::string& message)
        : LoxError(message), token(token) {}
};

// ReturnException moved to interpreter.h to avoid circular dependency

class ErrorReporter {
public:
    static bool hadError;
    static bool hadRuntimeError;
    
    static void error(int line, const std::string& message);
    static void error(const Token& token, const std::string& message);
    static void runtimeError(const RuntimeError& error);
    static void report(int line, const std::string& where, const std::string& message);
};