#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "common/error.h"

class Lox {
private:
    static Interpreter interpreter;

public:
    static void runFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << path << std::endl;
            exit(74);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        
        run(source);
        
        if (ErrorReporter::hadError) exit(65);
        if (ErrorReporter::hadRuntimeError) exit(70);
    }

    static void runPrompt() {
        std::string line;
        
        std::cout << "Lox Interpreter v1.0" << std::endl;
        std::cout << "Type 'exit' to quit." << std::endl;
        
        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;
            
            if (line == "exit") break;
            if (line.empty()) continue;
            
            run(line);
            ErrorReporter::hadError = false;
        }
    }

    static void run(const std::string& source) {
        try {
            Lexer lexer(source);
            std::vector<Token> tokens = lexer.scanTokens();
            
            Parser parser(tokens);
            std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
            
            if (ErrorReporter::hadError) return;
            
            interpreter.interpret(statements);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
};

Interpreter Lox::interpreter;

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: lox [script]" << std::endl;
        exit(64);
    } else if (argc == 2) {
        Lox::runFile(argv[1]);
    } else {
        Lox::runPrompt();
    }
    
    return 0;
}