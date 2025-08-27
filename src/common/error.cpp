#include "error.h"
#include <iostream>

bool ErrorReporter::hadError = false;
bool ErrorReporter::hadRuntimeError = false;

void ErrorReporter::error(int line, const std::string& message) {
    report(line, "", message);
}

void ErrorReporter::error(const Token& token, const std::string& message) {
    if (token.type == TokenType::TOKEN_EOF) {
        report(token.line, " at end", message);
    } else {
        report(token.line, " at '" + token.lexeme + "'", message);
    }
}

void ErrorReporter::runtimeError(const RuntimeError& error) {
    std::cerr << error.what() << std::endl;
    std::cerr << "[line " << error.token.line << "]" << std::endl;
    hadRuntimeError = true;
}

void ErrorReporter::report(int line, const std::string& where, const std::string& message) {
    std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
    hadError = true;
}