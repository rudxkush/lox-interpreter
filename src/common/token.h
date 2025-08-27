#pragma once

#include <string>
#include <unordered_map>

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS,
    SEMICOLON, SLASH, STAR,

    // One or two character tokens
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // Keywords
    AND, CLASS, ELSE, FALSE,
    FUN, FOR, IF, NIL, OR,
    PRINT, RETURN, SUPER, THIS,
    TRUE, VAR, WHILE,

    TOKEN_EOF
};

struct Token {
    TokenType type;
    std::string lexeme;
    std::string literal;
    int line;

    Token(TokenType type, const std::string& lexeme, const std::string& literal, int line)
        : type(type), lexeme(lexeme), literal(literal), line(line) {}
};

class TokenUtils {
public:
    static std::unordered_map<std::string, TokenType> keywords;
    static std::string tokenTypeToString(TokenType type);
    static TokenType getKeywordType(const std::string& text);
};