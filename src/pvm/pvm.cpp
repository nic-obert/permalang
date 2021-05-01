#include "pvm.hh"


using namespace pvm;



Pvm::Pvm()
{

}


Pvm::~Pvm()
{

}


Byte Pvm::execute(ByteCode byteCode)
{

    // index of execution (offset from byteCode)
    unsigned int offset = 0;

    // exit code
    Byte exitCode;

    // exexution loop
    bool executing = true;
    while (executing)
    {
        Byte byte = byteCode[offset];

        // handle operations
        switch ((OpCode) byte)
        {

        case OpCode::EXIT:
        {
            offset ++;
            exitCode = byteCode[offset];
            
            executing = false;
            break;
        }
        

        case OpCode::NO_OP:
            // just increment the offset
            offset ++;
            break;
        

        case OpCode::CMP:
        {
            // set zero flag register to the result of comparison
            rzf = rgp[GP_A] == rgp[GP_B];
            // increment offset to pass to the next instruction
            offset ++;
            break;
        }

        
        case OpCode::LD_IL:
        {
            // increment offset to get the first operator
            offset ++;
            Byte reg = byteCode[offset];

            // incremet offset to get the second operator
            offset ++;

            // get the long value from the byteCode
            long value = *((long*) (byteCode + offset));

            // since a long has been read, increment offset by sizeof(long)
            // to pass to the next instruction
            offset += sizeof(long);

            // load value into register
            rgp[reg] = value;
            break;
        }


        case OpCode::I_ADD:
        {
            // pass execution to the next instruction
            offset ++;
            // add value stored in B to A
            rgp[GP_A] += rgp[GP_B];

            // set the sign flag (true if result is negative, else false)
            rsf = rgp[GP_A] < 0;

            break;
        }

        case OpCode::I_SUB:
        {
            offset ++;
            rgp[GP_A] -= rgp[GP_B];

            rsf = rgp[GP_A] < 0;

            break;
        }

        case OpCode::I_MUL:
        {
            offset ++;
            rgp[GP_A] *= rgp[GP_B];

            rsf = rgp[GP_A] < 0;

            break;
        }
        

        } // switch ((OpCode) byte)

    } // while (executing)


    // finally return exit code
    return exitCode;
}

