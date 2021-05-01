#pragma once


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


    // PVM instruction set, similar to x86 assembly
    typedef enum class OpCode
    {
        NO_OP,  // no operation

        EXIT,   // end of the program

        I_ADD,  // long integer add from registers A and B
        I_SUB,  // long integer subtract from registers A and B
        I_MUL,  // long integer multiply from registers A and B
        I_DIV,  // long integer divide from registers A and B

        CMP,    // compare registers A and B

        LD_IL,   // load long integer

        MOV,    // copy a value

    } OpCode;

    
    // Perma Virtual Machine
    class Pvm
    {
    private:

        // REGISTERS

        // general purpose registers
        // accessed via the subscript operator []
        long rgp[2];
        
        // zero flag register (see x86 assembly for reference)
        bool rzf;

        // sign flag register (see x86 assembly for reference)
        bool rsf;

    public:

        Pvm();

        ~Pvm();

        // execute the given ByteCode in the PVM
        // returns an exit code
        Byte execute(ByteCode bytecode);

    };

};

