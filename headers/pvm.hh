#pragma once

#include "pch.hh"


typedef unsigned long size_t;

// whether the given register is a bit register
#define isBitRegister(reg) (reg == Registers::RZF || reg == Registers::RSF)


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

        CMP,        // compare registers A and B

        LDCA,       // load long integer constant into register A
        LDCB,       // load long integer constant into register A

        LDA,        // load long integer from memory into register A
        LDB,        // load long intefer from memory into register B

        MEM_MOV,    // copy a long value from a memory address to another
        REG_MOV,    // copy a long value from a register into a memory address
        REG_MOV_BIT,// copy a bit value from a register into a memory address

        MEM_SET,    // sets a memory address to a value

        JMP,        // unconditional jump to index   
        IF_JUMP,    // conditional jump based on zero flag register's value (1 = true, 0 = false)

        PUSH,       // push a new scope to the stack
        POP,        // pop the current scope from the stack


    } OpCode;


    class Memory
    {
    private:

        // memory size in bytes
        size_t size;
        // the actual memory
        Byte* memory = nullptr;

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
        RGA = 0,
        RGB = 1,
        RDR = 2,
        RZF = 3,
        RSF = 4

    } Registers;


    // stack data structure holding a memory offset
    typedef struct CallStack
    {
        
        Address address;
        CallStack* prev;

        CallStack(Address address, CallStack* prev);

    } CallStack;

    
    // Perma Virtual Machine
    class Pvm
    {
    private:

        Memory memory;

        CallStack* callStack;

        // REGISTERS

        // general purpose register A
        long rga;
        // general purpose register B
        long rgb;

        // division remainder register
        long rdr;
        
        // zero flag register (see x86 assembly for reference)
        bool rzf;

        // sign flag register (see x86 assembly for reference)
        bool rsf;

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


};


std::ostream& operator<<(std::ostream& stream, const pvm::Byte* byteCode);

std::ostream& operator<<(std::ostream& stream, const pvm::Registers& reg);

std::ostream& operator<<(std::ostream& stream, const pvm::Byte byte);

