#pragma once


#define size_t unsigned long


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


    } OpCode;


    class Memory
    {
    private:

        // memory size in bytes
        size_t size;

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


    typedef enum class Registers : Byte
    {
        RGA,
        RGB,
        RDR,
        RZF,
        RSF

    } Registers;


    #define isBitRegister(reg) (reg == Registers::RZF || reg == Registers::RSF)    

    
    // Perma Virtual Machine
    class Pvm
    {
    private:

        Memory memory;

        // REGISTERS

        // general purpose registers
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


        void* getRegister(Registers reg) const;

    public:

        Pvm(size_t memSize);

        // execute the given ByteCode in the PVM
        // returns an exit code
        Byte execute(const Byte* bytecode);

    };

};

