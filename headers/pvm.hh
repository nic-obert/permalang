#pragma once


#define size_t unsigned long


// Perma Virtual Machine
namespace pvm
{

    // general purpose register A
    #define GP_A 0
    // general purpose register B
    #define GP_B 1


    // a byte which ByteCode consists of
    typedef unsigned char Byte;

    // array of Bytes
    typedef const Byte* ByteCode;

    // address in the Memory class
    typedef unsigned long Address;


    // PVM instruction set, similar to x86 assembly
    typedef enum class OpCode
    {
        NO_OP,    // no operation

        EXIT,     // end of the program

        ADD,      // long integer add from registers A and B
        SUB,      // long integer subtract from registers A and B
        MUL,      // long integer multiply from registers A and B
        DIV,      // long integer divide from registers A and B

        CMP,      // compare registers A and B

        LD,       // load long integer literal into register
        MEM_LD,   // load long integer from memory

        MEM_MOV,  // copy a value from a memory address to another
        REG_MOV,  // copy a value from a register into a memory address


    } OpCode;


    class Memory
    {
    private:

        Byte* memory;

    public:

        Memory(size_t size);

        ~Memory();


        void set(Address address, long value);
        void set(Address address, Byte value);
        
        Byte getByte(Address address) const;
        long getLong(Address address) const;


    };

    
    // Perma Virtual Machine
    class Pvm
    {
    private:

        Memory memory;

        // REGISTERS

        // general purpose registers
        // accessed via the subscript operator []
        long rgp[2];

        // division remainder register
        long rdr;
        
        // zero flag register (see x86 assembly for reference)
        bool rzf;

        // sign flag register (see x86 assembly for reference)
        bool rsf;

    public:

        Pvm(size_t memSize);

        ~Pvm();

        // execute the given ByteCode in the PVM
        // returns an exit code
        Byte execute(ByteCode bytecode);

    };

};

