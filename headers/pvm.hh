#pragma once

#include "pch.hh"

#include "utils.hh"


// DEFINITIONS

typedef unsigned long size_t;

// whether the given register is a bit register
// dependant on constant enum values
#define isBitRegister(reg) (reg > Registers::RESULT)


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
        EXIT,               // end of the program, expects a 1 byte exit code

        ADD,                // long integer add from registers A and B
        SUB,                // long integer subtract from registers A and B
        MUL,                // long integer multiply from registers A and B
        DIV,                // long integer divide from registers A and B

        CMP,                // compare registers A and B, set the ZERO FLAG
        CMP_REVERSE,        // compare registers A and B, set the ZERO FLAG

        LD_CONST_A_8,       // load 8 constant bytes into register A
        LD_CONST_A_4,       // load 4 constant bytes into register A
        LD_CONST_A_1,       // load 1 constant byte into register A
        LD_CONST_A_BIT,     // load 1 constant bit into register A

        LD_CONST_B_8,       // load 8 constant bytes into register B
        LD_CONST_B_4,       // load 4 constant bytes into register B
        LD_CONST_B_1,       // load 1 constant byte into register B
        LD_CONST_B_BIT,     // load 1 constant bit into register B

        LD_CONST_RESULT_8,  // load 8 constant bytes into register RESULT
        LD_CONST_RESULT_4,  // load 4 constant bytes into register RESULT
        LD_CONST_RESULT_1,  // load 1 constant byte into register RESULT
        LD_CONST_RESULT_BIT,// load 1 constant bit into register RESULT

        LD_A_8,             // load 8 bytes from memory into register A
        LD_A_4,             // load 4 bytes from memory into register A
        LD_A_1,             // load 1 byte from memory into register A
        LD_A_BIT,           // load 1 bit from memory into register A

        LD_B_8,             // load 8 bytes from memory into register B
        LD_B_4,             // load 4 bytes from memory into register B
        LD_B_1,             // load 1 byte from memory into register B
        LD_B_BIT,           // load 1 bit from memory into register B

        LD_RESULT_8,        // load 8 bytes from memory into register RESULT    
        LD_RESULT_4,        // load 4 bytes from memory into register RESULT
        LD_RESULT_1,        // load 1 byte from memory into register RESULT
        LD_RESULT_BIT,      // load 1 bit from memory into register RESULT

        LD_ZERO_FLAG,       // load 1 bit from memory into register ZERO FLAG

        MEM_MOV_8,          // copy 8 bytes from a memory address to another
        MEM_MOV_4,          // copy 4 bytes from a memory address to another
        MEM_MOV_1,          // copy 1 byte from a memory address to another
        MEM_MOV_BIT,        // copy 1 bit from a memory address to another

        REG_MOV_8,          // copy a 8 bytes from a register into a memory address
        REG_MOV_4,          // copy an int value from a register into a memory address        
        REG_MOV_1,          // copy 1 byte from a register into a memory address
        REG_MOV_BIT,        // copy 1 bit from a register into a memory address

        REG_TO_REG,         // copy a value from a register to another

        MEM_SET_8,          // sets 8 bytes from a memory address to a value
        MEM_SET_4,          // sets 4 bytes from a memory address to a value
        MEM_SET_1,          // sets 1 byte from a memory address to a value
        MEM_SET_BIT,        // sets 1 bit from a memory address to a value

        JMP,                // unconditional jump to index   
        IF_JUMP,            // conditional jump based on ZERO FLAG register's value (1 = true, 0 = false)
        IF_NOT_JUMP,        // conditional jump based on ZERO FLAG register's value (0 = true, 1 = false)

        PUSH_CONST,         // push a constant long value on the stack
        PUSH_REG,           // push a value on the stack from a specified register
        PUSH_BYTES,         // increments the STACK POINTER by a specified amount

        POP,                // pop the STACK POINTER by a specified amount

        CALL,               // calls a function

        PRINT,              // prints the content of register A

        NO_OP,              // does nothing


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
        void set(Address address, int value);
        void set(Address address, Byte value);
        void set(Address address, bool value);
        
        long getLong(Address address) const;
        int getInt(Address address) const;
        Byte getByte(Address address) const;
        bool getBit(Address address) const;

    };


    // enum of Pvm registers
    // enum values must be constant for lookup tables
    typedef enum class Registers
    {
        // long registers
        GENERAL_A,
        GENERAL_B,
        DIVISION_REMAINDER,
        RESULT,
        // bit registers
        ZERO_FLAG,
        SIGN_FLAG,

    } Registers;


    // string representation of register
    const char* registerName(Registers reg);

    
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

        // stack pointer, points to the last used address in the stack
        long rStackPointer;
        
        // zero flag register, whether the result of the last operation was 0 (see x86 assembly for reference)
        bool rZeroFlag;

        // sign flag register, holds the sign of the last operation (see x86 assembly for reference)
        bool rSignFlag;


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

        ByteNode(OpCode data);
        ByteNode(Registers data);
        ByteNode(Value data, InstructionSize dataSize);       

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

        // add a new ByteNode to the list
        // update nodeCount and byteSize counters
        void add(ByteNode* node);

        // extends this ByteList with the elements of the other ByteList
        // destructive for the other ByteList, don't use it afterwards
        // updates nodeCount and byteSize counters
        void extend(ByteList& other);

        // resets the ByteList to its default values
        // - sets size counters to 0
        // - sets pointers to nullptr
        void clear();

        ByteCode toByteCode() const;


        size_t getCurrentSize() const;

    };


    // loads byte code from an executable file
    ByteCode loadByteCode(const char* executable);


    // writes the byte code to an executable file
    void generateExecutable(ByteCode byteCode, const char* name);


};


std::ostream& operator<<(std::ostream& stream, const pvm::ByteCode& byteCode);

std::ostream& operator<<(std::ostream& stream, const pvm::Registers& reg);

std::ostream& operator<<(std::ostream& stream, const pvm::OpCode opCode);

