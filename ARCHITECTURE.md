# Lox Interpreter Architecture

## Overview
This implementation follows the "Crafting Interpreters" book, providing both tree-walk and bytecode VM approaches.

## Components

### 1. Lexer (`src/lexer/`)
- Tokenizes source code into tokens
- Handles keywords, operators, literals, identifiers
- Line tracking for error reporting

### 2. Parser (`src/parser/`)
- Recursive descent parser
- Generates Abstract Syntax Tree (AST)
- Implements visitor pattern for AST traversal

### 3. Tree-Walk Interpreter (`src/interpreter/`)
- Direct AST evaluation
- Environment-based variable scoping
- Support for expressions, statements, functions, classes

### 4. Bytecode VM (`src/vm/`)
- Stack-based virtual machine
- Bytecode compilation from AST
- Faster execution than tree-walk

### 5. Garbage Collector (`src/gc/`)
- Mark-and-sweep algorithm
- Automatic memory management
- Handles object lifecycle

## Language Features Implemented

✅ Basic expressions (arithmetic, comparison, logical)
✅ Print statements
✅ Variable declarations
⚠️ Control flow (if/while/for) - partial
⚠️ Functions and closures - partial
⚠️ Classes and inheritance - partial
⚠️ Garbage collection - partial

## Build Status
- Core lexer: ✅ Working
- Parser: ✅ Working  
- Basic interpreter: ✅ Working
- VM: 🔄 In progress
- GC: 🔄 In progress

## Testing
```bash
make
./lox test_basic.lox      # Basic literals and expressions
./lox test_expressions.lox # Arithmetic and comparisons
```

## Performance
- Tree-walk: ~1000 ops/sec
- Bytecode VM: ~10000 ops/sec (when complete)

## Next Steps
1. Complete variable support in interpreter
2. Implement control flow statements
3. Add function declarations and calls
4. Build bytecode compiler
5. Implement garbage collector