#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include "../lexer/lexer.h"
#include "../common/token.h"
#include "chunk.h"

enum class Precedence {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
};

enum class FunctionType {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_SCRIPT
};

struct ParseRule {
    void (Compiler::*prefix)();
    void (Compiler::*infix)();
    Precedence precedence;
};

struct Local {
    Token name;
    int depth;
    bool isCaptured;
    
    Local(Token name, int depth) : name(name), depth(depth), isCaptured(false) {}
};

struct Upvalue {
    unsigned char index;
    bool isLocal;
    
    Upvalue(unsigned char index, bool isLocal) : index(index), isLocal(isLocal) {}
};

class Compiler {
private:
    struct CompilerState {
        std::shared_ptr<CompilerState> enclosing;
        std::shared_ptr<ObjFunction> function;
        FunctionType type;
        
        std::vector<Local> locals;
        std::vector<Upvalue> upvalues;
        int scopeDepth;
        
        CompilerState(FunctionType type, std::shared_ptr<CompilerState> enclosing = nullptr);
    };
    
    std::shared_ptr<CompilerState> current;
    std::shared_ptr<ClassCompiler> currentClass;
    
    Lexer* lexer;
    Token currentToken;
    Token previousToken;
    bool hadError;
    bool panicMode;
    
    static std::unordered_map<TokenType, ParseRule> rules;
    
    // Error handling
    void errorAtCurrent(const std::string& message);
    void error(const std::string& message);
    void errorAt(Token& token, const std::string& message);
    void synchronize();
    
    // Token management
    void advance();
    void consume(TokenType type, const std::string& message);
    bool check(TokenType type);
    bool match(TokenType type);
    
    // Code generation
    void emitByte(unsigned char byte);
    void emitBytes(unsigned char byte1, unsigned char byte2);
    void emitLoop(int loopStart);
    int emitJump(OpCode instruction);
    void emitReturn();
    unsigned char makeConstant(Value value);
    void emitConstant(Value value);
    void patchJump(int offset);
    
    // Compilation
    void expression();
    void statement();
    void declaration();
    void classDeclaration();
    void funDeclaration();
    void varDeclaration();
    void expressionStatement();
    void forStatement();
    void ifStatement();
    void printStatement();
    void returnStatement();
    void whileStatement();
    void block();
    void function(FunctionType type);
    void method();
    
    void beginScope();
    void endScope();
    
    // Expression parsing
    void parsePrecedence(Precedence precedence);
    ParseRule* getRule(TokenType type);
    
    // Expression handlers
    void binary();
    void call();
    void dot();
    void literal();
    void grouping();
    void number();
    void or_();
    void string();
    void super_();
    void this_();
    void unary();
    void variable();
    void and_();
    
    // Variable handling
    unsigned char parseVariable(const std::string& errorMessage);
    void markInitialized();
    void defineVariable(unsigned char global);
    unsigned char argumentList();
    unsigned char identifierConstant(Token& name);
    int resolveLocal(CompilerState& compiler, Token& name);
    int addUpvalue(CompilerState& compiler, unsigned char index, bool isLocal);
    int resolveUpvalue(CompilerState& compiler, Token& name);
    void addLocal(Token name);
    void declareVariable();
    void namedVariable(Token name, bool canAssign);
    
    // Current chunk access
    Chunk& currentChunk();

public:
    Compiler();
    std::shared_ptr<ObjFunction> compile(const std::string& source);
};

struct ClassCompiler {
    std::shared_ptr<ClassCompiler> enclosing;
    bool hasSuperclass;
    
    ClassCompiler(std::shared_ptr<ClassCompiler> enclosing = nullptr) 
        : enclosing(enclosing), hasSuperclass(false) {}
};