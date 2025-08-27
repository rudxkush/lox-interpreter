#pragma once

#include <vector>
#include <memory>
#include "opcodes.h"
#include "../common/value.h"

class Chunk {
private:
    std::vector<unsigned char> code;
    std::vector<int> lines;
    std::vector<Value> constants;

public:
    void writeChunk(unsigned char byte, int line);
    void writeChunk(OpCode opcode, int line);
    int addConstant(const Value& value);
    
    // Getters
    const std::vector<unsigned char>& getCode() const { return code; }
    const std::vector<int>& getLines() const { return lines; }
    const std::vector<Value>& getConstants() const { return constants; }
    
    unsigned char getByte(int offset) const { return code[offset]; }
    Value getConstant(int index) const { return constants[index]; }
    int getLine(int offset) const { return lines[offset]; }
    
    size_t count() const { return code.size(); }
    
    // Disassembly
    void disassembleChunk(const std::string& name) const;
    int disassembleInstruction(int offset) const;
    
private:
    int constantInstruction(const std::string& name, int offset) const;
    int simpleInstruction(const std::string& name, int offset) const;
    int byteInstruction(const std::string& name, int offset) const;
    int jumpInstruction(const std::string& name, int sign, int offset) const;
    int invokeInstruction(const std::string& name, int offset) const;
};