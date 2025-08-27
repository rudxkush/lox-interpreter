#include "chunk.h"
#include <iostream>
#include <iomanip>

void Chunk::writeChunk(unsigned char byte, int line) {
    code.push_back(byte);
    lines.push_back(line);
}

void Chunk::writeChunk(OpCode opcode, int line) {
    writeChunk(static_cast<unsigned char>(opcode), line);
}

int Chunk::addConstant(const Value& value) {
    constants.push_back(value);
    return constants.size() - 1;
}

void Chunk::disassembleChunk(const std::string& name) const {
    std::cout << "== " << name << " ==" << std::endl;
    
    for (int offset = 0; offset < static_cast<int>(code.size());) {
        offset = disassembleInstruction(offset);
    }
}

int Chunk::disassembleInstruction(int offset) const {
    std::cout << std::setfill('0') << std::setw(4) << offset << " ";
    
    if (offset > 0 && lines[offset] == lines[offset - 1]) {
        std::cout << "   | ";
    } else {
        std::cout << std::setw(4) << lines[offset] << " ";
    }
    
    OpCode instruction = static_cast<OpCode>(code[offset]);
    switch (instruction) {
        case OpCode::OP_CONSTANT:
            return constantInstruction("OP_CONSTANT", offset);
        case OpCode::OP_NIL:
            return simpleInstruction("OP_NIL", offset);
        case OpCode::OP_TRUE:
            return simpleInstruction("OP_TRUE", offset);
        case OpCode::OP_FALSE:
            return simpleInstruction("OP_FALSE", offset);
        case OpCode::OP_ADD:
            return simpleInstruction("OP_ADD", offset);
        case OpCode::OP_SUBTRACT:
            return simpleInstruction("OP_SUBTRACT", offset);
        case OpCode::OP_MULTIPLY:
            return simpleInstruction("OP_MULTIPLY", offset);
        case OpCode::OP_DIVIDE:
            return simpleInstruction("OP_DIVIDE", offset);
        case OpCode::OP_NEGATE:
            return simpleInstruction("OP_NEGATE", offset);
        case OpCode::OP_RETURN:
            return simpleInstruction("OP_RETURN", offset);
        default:
            std::cout << "Unknown opcode " << static_cast<int>(instruction) << std::endl;
            return offset + 1;
    }
}

int Chunk::constantInstruction(const std::string& name, int offset) const {
    unsigned char constant = code[offset + 1];
    std::cout << std::left << std::setw(16) << name << " " 
              << std::setw(4) << static_cast<int>(constant) << " '";
    std::cout << constants[constant].toString() << "'" << std::endl;
    return offset + 2;
}

int Chunk::simpleInstruction(const std::string& name, int offset) const {
    std::cout << name << std::endl;
    return offset + 1;
}

int Chunk::byteInstruction(const std::string& name, int offset) const {
    unsigned char slot = code[offset + 1];
    std::cout << std::left << std::setw(16) << name << " " 
              << static_cast<int>(slot) << std::endl;
    return offset + 2;
}

int Chunk::jumpInstruction(const std::string& name, int sign, int offset) const {
    unsigned short jump = static_cast<unsigned short>(code[offset + 1] << 8);
    jump |= code[offset + 2];
    std::cout << std::left << std::setw(16) << name << " " 
              << std::setw(4) << offset << " -> " << (offset + 3 + sign * jump) << std::endl;
    return offset + 3;
}

int Chunk::invokeInstruction(const std::string& name, int offset) const {
    unsigned char constant = code[offset + 1];
    unsigned char argCount = code[offset + 2];
    std::cout << std::left << std::setw(16) << name << " (" 
              << static_cast<int>(argCount) << " args) " << std::setw(4) << static_cast<int>(constant) 
              << " '" << constants[constant].toString() << "'" << std::endl;
    return offset + 3;
}