#pragma once

#include "pch.hh"

#include "utils.hh"


typedef unsigned long size_t;

// whether the given register is a bit register
#define isBitRegister(reg) (reg == Registers::ZERO_FLAG || reg == Registers::SIGN_FLAG)


// Perma Virtual Machine
namespace pvm
{

    // a byte which ByteCode consists of
    typedef unsigned char Byte;

    // address in the Memory class
    typedef unsigned long Address;


    // PVM instruction set, similar to x86 assembly
    typedef enum class OpCode : Byte
    {
        EXIT,       // end of the program

        ADD,        // long integer add from registers A and B
        SUB,        // long integer subtract from registers A and B
        MUL,        // long integer multiply from registers A and B
        DIV,        // long integer divide from registers A and B

        CMP,        // compare registers A and B, set the zero flag
        CMP_REVERSE,// compare registers A and B, set the zero flag

        LD_CONST_A, // load long integer constant into register A
        LD_CONST_B, // load long integer constant into register A

        LD_A,       // load long integer from memory into register A
        LD_B,       // load long intefer from memory into register B

        MEM_MOV,    // copy a long value from a memory address to another
        REG_MOV,    // copy a long value from a register into a memory address
        REG_MOV_BIT,// copy a bit value from a register into a memory address
        REG_TO_REG, // copy a value from a register to another

        MEM_SET,    // sets a memory address to a value

        JMP,        // unconditional jump to index   
        IF_JUMP,    // conditional jump based on zero flag register's value (1 = true, 0 = false)

        PUSH_CONST, // push a constant long value on the stack
        PUSH_REG,   // push a value on the stack from a specified register
        PUSH_BYTES, // increments the stack pointer by a specified amount

        POP,        // pop the stack pointer by a specified amount

        CALL,       // calls a function


    } OpCode;


    class Memory
    {
    private:

        // memory size in bytes
        size_t size;

        // the stack section 
        Byte* stack = nullptr;

    public:

        Memory(size_t size);

        ~Memory();


        void set(Address address, long value);
        void set(Address address, Byte value);
        void set(Address address, bool value);
        
        Byte getByte(Address address) const;
        long getLong(Address address) const;
        bool getBit(Address address) const;

    };


    // enum of Pvm registers
    // enum values must be constant for lookup tables
    typedef enum class Registers : Byte
    {
        GENERAL_A           = 0,
        GENERAL_B           = 1,
        DIVISION_REMAINDER  = 2,
        ZERO_FLAG           = 3,
        SIGN_FLAG           = 4,
        RESULT              = 5,

    } Registers;

    
    // Perma Virtual Machine
    class Pvm
    {
    private:

        Memory memory;

        // REGISTERS

        // general purpose register A
        long rGeneralA;
        // general purpose register B
        long rGeneralB;

        // result register, result of the last performed operation
        long rResult;

        // division remainder register, remainder of the last division operation
        long rDivisionRemainder;
        
        // zero flag register, whether the result of the last operation was 0 (see x86 assembly for reference)
        bool rZeroFlag;

        // sign flag register, holds the sign of the last operation (see x86 assembly for reference)
        bool rSignFlag;

        // stack pointer, points to the last used address in the stack
        long rStackPointer;


        // returns a pointer to the requested register
        // the returned pointer has to be cast to the right type
        void* getRegister(Registers reg) const;

    public:

        Pvm(size_t memSize);

        // execute the given ByteCode in the PVM
        // returns an exit code
        Byte execute(const Byte* bytecode);

    };


    // a pair of <Byte*, size_t>
    // holds the byte code along with its size
    typedef struct ByteCode
    {
        // the actual byte code
        Byte* byteCode;
        size_t size;

        ByteCode(Byte* byteCode, size_t size);
        ByteCode();

    } ByteCode;


    typedef unsigned char InstructionSize;


    // node of doubly-linked list of instuctions
    typedef struct ByteNode
    {
        ByteNode* next;
        ByteNode* prev;

        Value data;
        InstructionSize dataSize; 

        ByteNode();
        ByteNode(OpCode data);
        ByteNode(Registers data);
        ByteNode(Value data, InstructionSize dataSize = sizeof(Value));       

    } ByteNode;


    class ByteList
    {
    private:

        ByteNode* start;
        ByteNode* end;

        size_t nodeCount;
        size_t byteSize;

    public:

        ByteList();
        
        ~ByteList();

        void add(ByteNode* node);

        void insertFisrt(ByteNode* node);

        ByteCode toByteCode() const;

    };


};


std::ostream& operator<<(std::ostream& stream, const pvm::Byte* byteCode);

std::ostream& operator<<(std::ostream& stream, const pvm::Registers& reg);

std::ostream& operator<<(std::ostream& stream, const pvm::Byte byte);

