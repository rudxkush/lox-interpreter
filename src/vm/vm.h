#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "chunk.h"
#include "../common/value.h"
#include "../common/token.h"

enum class InterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
};

// Forward declarations
class ObjString;
class ObjFunction;
class ObjClosure;
class ObjUpvalue;
class ObjClass;
class ObjInstance;
class ObjBoundMethod;

struct CallFrame {
    std::shared_ptr<ObjClosure> closure;
    unsigned char* ip;
    Value* slots;
    
    CallFrame(std::shared_ptr<ObjClosure> closure, unsigned char* ip, Value* slots)
        : closure(closure), ip(ip), slots(slots) {}
};

class VM {
private:
    static const int FRAMES_MAX = 64;
    static const int STACK_MAX = FRAMES_MAX * 256;
    
    CallFrame frames[FRAMES_MAX];
    int frameCount;
    
    Value stack[STACK_MAX];
    Value* stackTop;
    
    std::unordered_map<std::string, Value> globals;
    std::shared_ptr<ObjUpvalue> openUpvalues;
    
    std::string initString;
    
    void resetStack();
    void runtimeError(const char* format, ...);
    void defineNative(const std::string& name, int arity, Value (*function)(int argCount, Value* args));
    
    Value peek(int distance);
    bool call(std::shared_ptr<ObjClosure> closure, int argCount);
    bool callValue(Value callee, int argCount);
    bool invokeFromClass(std::shared_ptr<ObjClass> klass, const std::string& name, int argCount);
    bool invoke(const std::string& name, int argCount);
    bool bindMethod(std::shared_ptr<ObjClass> klass, const std::string& name);
    std::shared_ptr<ObjUpvalue> captureUpvalue(Value* local);
    void closeUpvalues(Value* last);
    void defineMethod(const std::string& name);
    bool isFalsey(Value value);
    void concatenate();

public:
    VM();
    ~VM();
    
    InterpretResult interpret(const std::string& source);
    InterpretResult run();
    
    void push(Value value);
    Value pop();
    
    // Stack manipulation
    void printStack();
};