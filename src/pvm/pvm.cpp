#include "pvm.hh"


#define fetchLong() (*((long*) (byteCode + offset)))


using namespace pvm;



Pvm::Pvm(size_t memSize)
: memory(memSize)
{

}


void* Pvm::getRegister(Registers reg) const
{
    switch (reg)
    {
    case Registers::RGA:
        return (void*) &rga;
    case Registers::RGB:
        return (void*) &rgb;
    case Registers::RDR:
        return (void*) &rdr;
    case Registers::RZF:
        return (void*) &rzf;
    case Registers::RSF:
        return (void*) &rsf;    
    }

    // this code never gets reached
    return nullptr;
}


Byte Pvm::execute(const Byte* byteCode)
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
            rzf = rga == rgb;
            // increment offset to pass to the next instruction
            offset ++;
            break;
        }

        
        case OpCode::ADD:
        {
            // add value stored in B to A
            rga += rgb;

            // set the sign flag (true if result is negative, else false)
            rsf = rga < 0;

            // set the zero flag
            rzf = rga == 0;

            break;
        }

        case OpCode::SUB:
        {
            rga -= rgb;

            rsf = rga < 0;

            rzf = rga == 0;

            break;
        }

        case OpCode::MUL:
        {
            rga *= rgb;

            rsf = rga < 0;

            rzf = rga == 0;

            break;
        }

        case OpCode::DIV:
        {
            rga /= rgb;

            // set the remainder
            rdr = rga % rgb;

            rzf = rga == 0;

            break;
        }

        
        case OpCode::LDCA:
        {
            // get the long constant value from the byteCode
            const long value = fetchLong();

            // since a long has been read, increment offset by sizeof(long)
            // to pass to the next instruction
            offset += sizeof(long);

            // load value into register
            rga = value;
            break;
        }


        case OpCode::LDCB:
        {
            // get the long constant value from the byteCode
            const long value = fetchLong();

            // since a long has been read, increment offset by sizeof(long)
            // to pass to the next instruction
            offset += sizeof(long);

            // load value into register
            rgb = value;
            break;
        }


        case OpCode::LDA:
        {
            Address address = fetchLong();

            offset += sizeof(long);

            rga = memory.getLong(address);

            break;            
        }


        case OpCode::LDB:
        {
            Address address = fetchLong();

            offset += sizeof(long);

            rgb = memory.getLong(address);

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

            Registers reg = (Registers) byteCode[offset];

            offset ++;

            memory.set(address, *((long*) getRegister(reg)));
            
            break;
        }   


        case OpCode::REG_MOV_BIT:
        {
            Address address = fetchLong();
            offset += sizeof(long);

            Registers reg = (Registers) byteCode[offset];

            offset ++;

            memory.set(address, *((bool*) getRegister(reg)));

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


        case OpCode::IF_JUMP:
        {
            // if zero flag register is set to 1 perform the jump
            if (rzf)
            {
                offset = fetchLong();
                break;
            }

            // else pass to the next instruction

            offset += sizeof(long);

            break;
        }


        } // switch ((OpCode) byte)

    } // while (executing)


    // finally return exit code
    return exitCode;
}

