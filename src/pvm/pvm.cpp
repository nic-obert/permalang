#include "pvm.hh"


#define fetchLong() (*((long*) (byteCode + offset)))


using namespace pvm;



Pvm::Pvm(size_t memSize)
: memory(memSize)
{

}


Pvm::~Pvm()
{

}


Byte Pvm::execute(ByteCode byteCode)
{

    // index of execution (offset from byteCode)
    size_t offset = 0;

    // exit code
    Byte exitCode;

    // exexution loop
    bool executing = true;
    while (executing)
    {
        Byte byte = byteCode[offset];

        // increase offset by 1 byte so not having to write it inside every case
        offset ++;

        // handle operations
        switch ((OpCode) byte)
        {

        case OpCode::EXIT:
        {
            exitCode = byteCode[offset];
            
            executing = false;
            break;
        }
        

        case OpCode::CMP:
        {
            // set zero flag register to the result of comparison
            rzf = rgp[GP_A] == rgp[GP_B];
            // increment offset to pass to the next instruction
            offset ++;
            break;
        }

        
        case OpCode::ADD:
        {
            // add value stored in B to A
            rgp[GP_A] += rgp[GP_B];

            // set the sign flag (true if result is negative, else false)
            rsf = rgp[GP_A] < 0;

            break;
        }

        case OpCode::SUB:
        {
            rgp[GP_A] -= rgp[GP_B];

            rsf = rgp[GP_A] < 0;

            break;
        }

        case OpCode::MUL:
        {
            rgp[GP_A] *= rgp[GP_B];

            rsf = rgp[GP_A] < 0;

            break;
        }

        case OpCode::DIV:
        {
            rgp[GP_A] /= rgp[GP_B];

            // set the remainder
            rdr = rgp[GP_A] % rgp[GP_B];

            break;
        }

        
        case OpCode::LD:
        {
            Byte reg = byteCode[offset];

            // incremet offset to get the second operator
            offset ++;

            // get the long value from the byteCode
            long value = fetchLong();

            // since a long has been read, increment offset by sizeof(long)
            // to pass to the next instruction
            offset += sizeof(long);

            // load value into register
            rgp[reg] = value;
            break;
        }


        case OpCode::MEM_LD:
        {
            Byte reg = byteCode[offset];

            offset ++;

            Address address = fetchLong();

            offset += sizeof(long);

            rgp[reg] = memory.getLong(address);

            break;            
        }
        

        case OpCode::MEM_MOV:
        {
            Address addr1 = fetchLong();
            
            offset += sizeof(long);

            Address addr2 = fetchLong();
            
            offset += sizeof(long);

            memory.set(addr1, memory.getLong(addr2));

            break;
        }


        case OpCode::REG_MOV:
        {
            Address address = fetchLong();
            offset += sizeof(long);

            Byte reg = byteCode[offset];

            offset ++;

            rgp[reg] = memory.getLong(address);

            break;
        }


        case OpCode::MEM_SET:
        {
            Address address = fetchLong();
            offset += sizeof(long);

            long value = fetchLong();
            offset += sizeof(long);

            memory.set(address, value);

            break;
        }


        case OpCode::JMP:
        {
            // set offset to the instruction to jump to
            offset = fetchLong();
            break;
        }


        } // switch ((OpCode) byte)

    } // while (executing)


    // finally return exit code
    return exitCode;
}

