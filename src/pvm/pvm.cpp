#include "pvm.hh"


using namespace pvm;



Pvm::Pvm(size_t memSize)
: memory(memSize)
{

}


// utility function
// returns a long value from the byteCode
// updates the offset by sizeof(long)
static inline long getLongValue(const Byte* byteCode, size_t& offset)
{
    long value = *((long*) (byteCode + offset));
    offset += sizeof(long);
    return value;
}


Byte Pvm::execute(const Byte* byteCode)
{

    // index of execution (offset from byteCode pointer)
    size_t offset = 0;

    // exit code that will be set by the EXIT instruction and finally returned
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
            // set zero flag register to the result of comparison (see x86 asm)
            rZeroFlag = rGeneralA == rGeneralB;
            
            offset ++;
            break;
        }


        case OpCode::CMP_REVERSE:
        {
            // set zero flag register to the result of comparison (see x86 asm)
            rZeroFlag = rGeneralA != rGeneralB;
            
            offset ++;
            break;
        }

        
        case OpCode::ADD:
        {
            // add value stored in B to A
            rResult = rGeneralA + rGeneralB;

            // set the sign flag (true if result is negative, else false)
            rSignFlag = rResult < 0;

            // set the zero flag
            rZeroFlag = rResult == 0;

            break;
        }

        case OpCode::SUB:
        {
            rResult = rGeneralA - rGeneralB;

            rSignFlag = rResult < 0;

            rZeroFlag = rResult == 0;

            break;
        }

        case OpCode::MUL:
        {
            rResult = rGeneralA * rGeneralB;

            rSignFlag = rResult < 0;

            rZeroFlag = rResult == 0;

            break;
        }

        case OpCode::DIV:
        {
            rResult = rGeneralA / rGeneralB;

            rDivisionRemainder = rResult % rGeneralB;

            rZeroFlag = rResult == 0;

            break;
        }

        
        case OpCode::LD_CONST_A:
        {
            const long value = getLongValue(byteCode, offset);

            rGeneralA = value;
            break;
        }


        case OpCode::LD_CONST_B:
        {
            const long value = getLongValue(byteCode, offset);

            rGeneralB = value;
            break;
        }


        case OpCode::LD_A:
        {
            Address address = getLongValue(byteCode, offset);

            rGeneralA = memory.getLong(address);

            break;            
        }


        case OpCode::LD_B:
        {
            Address address = getLongValue(byteCode, offset);

            rGeneralB = memory.getLong(address);

            break;            
        }


        case OpCode::MEM_MOV:
        {
            Address addr1 = getLongValue(byteCode, offset);

            Address addr2 = getLongValue(byteCode, offset);

            memory.set(addr1, memory.getLong(addr2));

            break;
        }


        case OpCode::REG_MOV:
        {
            Address address = getLongValue(byteCode, offset);

            Registers reg = (Registers) byteCode[offset];

            offset ++;

            memory.set(address, *((long*) getRegister(reg)));
            
            break;
        }   


        case OpCode::REG_MOV_BIT:
        {
            Address address = getLongValue(byteCode, offset);

            Registers reg = (Registers) byteCode[offset];

            offset ++;

            memory.set(address, *((bool*) getRegister(reg)));

            break;
        }


        case OpCode::MEM_SET:
        {
            Address address = getLongValue(byteCode, offset);

            long value = getLongValue(byteCode, offset);

            memory.set(address, value);

            break;
        }


        case OpCode::JMP:
        {
            // set offset to the instruction to jump to
            offset = *((long*) (byteCode + offset));
            break;
        }


        case OpCode::IF_JUMP:
        {
            // if zero flag register is set to 1 perform the jump
            // by updating the offset from the byteCode*
            if (rZeroFlag)
            {
                offset = *((long*) (byteCode + offset));
                break;
            }

            // else pass to the next instruction

            offset += sizeof(long);

            break;
        }


        case OpCode::PUSH_CONST:
        {
            long value = getLongValue(byteCode, offset);

            memory.set(rStackPointer, value);

            rStackPointer += sizeof(long);

            break;
        }


        case OpCode::PUSH_REG:
        {
            long value = *(long*) getRegister((Registers) byteCode[offset]);

            memory.set(rStackPointer, value);

            rStackPointer += sizeof(long);

            break;
        }


        case OpCode::PUSH_BYTES:
        {
            size_t amount = getLongValue(byteCode, offset);

            rStackPointer += amount;
        }


        case OpCode::POP:
        {
            size_t amount = getLongValue(byteCode, offset);

            rStackPointer -= amount;

            break;
        }


        } // switch ((OpCode) byte)

    } // while (executing)


    return exitCode;
}

