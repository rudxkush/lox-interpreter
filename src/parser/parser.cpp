#include "parser.h"
#include "../common/error.h"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!isAtEnd()) {
        try {
            auto stmt = declaration();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        } catch (const ParseError& error) {
            synchronize();
        }
    }
    
    return statements;
}

bool Parser::match(std::initializer_list<TokenType> types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::TOKEN_EOF;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    throw ParseError(peek(), message);
}

void Parser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUN:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
            case TokenType::RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match({TokenType::CLASS})) return classDeclaration();
        if (match({TokenType::FUN})) return function("function");
        if (match({TokenType::VAR})) return varDeclaration();
        
        return statement();
    } catch (const ParseError& error) {
        synchronize();
        return nullptr;
    }
}

std::unique_ptr<Stmt> Parser::classDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
    
    std::unique_ptr<VariableExpr> superclass = nullptr;
    if (match({TokenType::LESS})) {
        consume(TokenType::IDENTIFIER, "Expect superclass name.");
        superclass = std::make_unique<VariableExpr>(previous());
    }
    
    consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");
    
    std::vector<std::unique_ptr<FunctionStmt>> methods;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto method = std::unique_ptr<FunctionStmt>(
            static_cast<FunctionStmt*>(function("method").release())
        );
        methods.push_back(std::move(method));
    }
    
    consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
    
    return std::make_unique<ClassStmt>(name, std::move(superclass), std::move(methods));
}

std::unique_ptr<Stmt> Parser::function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (parameters.size() >= 255) {
                ErrorReporter::error(peek(), "Can't have more than 255 parameters.");
            }
            
            parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
        } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<std::unique_ptr<Stmt>> body = block();
    
    return std::make_unique<FunctionStmt>(name, std::move(parameters), std::move(body));
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    
    std::unique_ptr<Expr> initializer = nullptr;
    if (match({TokenType::EQUAL})) {
        initializer = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarStmt>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::RETURN})) return returnStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LEFT_BRACE})) return std::make_unique<BlockStmt>(block());
    
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    
    std::unique_ptr<Stmt> initializer;
    if (match({TokenType::SEMICOLON})) {
        initializer = nullptr;
    } else if (match({TokenType::VAR})) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }
    
    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    
    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
    
    std::unique_ptr<Stmt> body = statement();
    
    if (increment != nullptr) {
        std::vector<std::unique_ptr<Stmt>> statements;
        statements.push_back(std::move(body));
        statements.push_back(std::make_unique<ExpressionStmt>(std::move(increment)));
        body = std::make_unique<BlockStmt>(std::move(statements));
    }
    
    if (condition == nullptr) {
        condition = std::make_unique<LiteralExpr>(Value(true));
    }
    body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    
    if (initializer != nullptr) {
        std::vector<std::unique_ptr<Stmt>> statements;
        statements.push_back(std::move(initializer));
        statements.push_back(std::move(body));
        body = std::make_unique<BlockStmt>(std::move(statements));
    }
    
    return body;
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    
    std::unique_ptr<Stmt> thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
        elseBranch = statement();
    }
    
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::printStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    Token keyword = previous();
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expect ';' after return value.");
    return std::make_unique<ReturnStmt>(keyword, std::move(value));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    std::unique_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    std::unique_ptr<Stmt> body = statement();
    
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    std::unique_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        statements.push_back(declaration());
    }
    
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
}

std::unique_ptr<Expr> Parser::expression() {
    return assignment();
}

std::unique_ptr<Expr> Parser::assignment() {
    std::unique_ptr<Expr> expr = or_();
    
    if (match({TokenType::EQUAL})) {
        Token equals = previous();
        std::unique_ptr<Expr> value = assignment();
        
        if (auto variable = dynamic_cast<VariableExpr*>(expr.get())) {
            Token name = variable->name;
            expr.release(); // Release ownership before creating AssignExpr
            return std::make_unique<AssignExpr>(name, std::move(value));
        } else if (auto get = dynamic_cast<GetExpr*>(expr.get())) {
            auto object = std::unique_ptr<Expr>(get->object.release());
            Token name = get->name;
            expr.release(); // Release ownership
            return std::make_unique<SetExpr>(std::move(object), name, std::move(value));
        }
        
        ErrorReporter::error(equals, "Invalid assignment target.");
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::or_() {
    std::unique_ptr<Expr> expr = and_();
    
    while (match({TokenType::OR})) {
        Token operator_ = previous();
        std::unique_ptr<Expr> right = and_();
        expr = std::make_unique<LogicalExpr>(std::move(expr), operator_, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::and_() {
    std::unique_ptr<Expr> expr = equality();
    
    while (match({TokenType::AND})) {
        Token operator_ = previous();
        std::unique_ptr<Expr> right = equality();
        expr = std::make_unique<LogicalExpr>(std::move(expr), operator_, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    std::unique_ptr<Expr> expr = comparison();
    
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token operator_ = previous();
        std::unique_ptr<Expr> right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    std::unique_ptr<Expr> expr = term();
    
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token operator_ = previous();
        std::unique_ptr<Expr> right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    std::unique_ptr<Expr> expr = factor();
    
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token operator_ = previous();
        std::unique_ptr<Expr> right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    std::unique_ptr<Expr> expr = unary();
    
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token operator_ = previous();
        std::unique_ptr<Expr> right = unary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), operator_, std::move(right));
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::unary() {
    if (match({TokenType::BANG, TokenType::MINUS})) {
        Token operator_ = previous();
        std::unique_ptr<Expr> right = unary();
        return std::make_unique<UnaryExpr>(operator_, std::move(right));
    }
    
    return call();
}

std::unique_ptr<Expr> Parser::call() {
    std::unique_ptr<Expr> expr = primary();
    
    while (true) {
        if (match({TokenType::LEFT_PAREN})) {
            expr = finishCall(std::move(expr));
        } else if (match({TokenType::DOT})) {
            Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            expr = std::make_unique<GetExpr>(std::move(expr), name);
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expr> Parser::finishCall(std::unique_ptr<Expr> callee) {
    std::vector<std::unique_ptr<Expr>> arguments;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (arguments.size() >= 255) {
                ErrorReporter::error(peek(), "Can't have more than 255 arguments.");
            }
            arguments.push_back(expression());
        } while (match({TokenType::COMMA}));
    }
    
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    
    return std::make_unique<CallExpr>(std::move(callee), paren, std::move(arguments));
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>(Value(false));
    if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>(Value(true));
    if (match({TokenType::NIL})) return std::make_unique<LiteralExpr>(Value());
    
    if (match({TokenType::NUMBER})) {
        double value = std::stod(previous().literal);
        return std::make_unique<LiteralExpr>(Value(value));
    }
    
    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(Value(previous().literal));
    }
    
    if (match({TokenType::SUPER})) {
        Token keyword = previous();
        consume(TokenType::DOT, "Expect '.' after 'super'.");
        Token method = consume(TokenType::IDENTIFIER, "Expect superclass method name.");
        return std::make_unique<SuperExpr>(keyword, method);
    }
    
    if (match({TokenType::THIS})) {
        return std::make_unique<ThisExpr>(previous());
    }
    
    if (match({TokenType::IDENTIFIER})) {
        return std::make_unique<VariableExpr>(previous());
    }
    
    if (match({TokenType::LEFT_PAREN})) {
        std::unique_ptr<Expr> expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }
    
    throw ParseError(peek(), "Expect expression.");
}