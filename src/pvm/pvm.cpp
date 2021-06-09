#include "pvm.hh"


using namespace pvm;


Pvm::Pvm(size_t memSize)
:   memory(memSize), rGeneralA(0), rGeneralB(0),
    rResult(0), rDivisionRemainder(0), rZeroFlag(0),
    rSignFlag(0), rStackPointer(0)
{

}


static inline long getLongValue(const Byte* byteCode, size_t& offset)
{
    const long value = *((long*) (byteCode + offset));
    offset += sizeof(long);
    return value;
}

static inline int getIntValue(const Byte* byteCode, size_t& offset)
{
    const int value = *((int*) (byteCode + offset));
    offset += sizeof(int);
    return value;
}


static inline Byte getByteValue(const Byte* byteCode, size_t& offset)
{
    const Byte value = byteCode[offset];
    offset ++;
    return value;
}


Byte Pvm::execute(const Byte* byteCode)
{

    // index of execution (offset from byteCode pointer)
    size_t offset = 0;

    // exit code that will be set by the EXIT instruction and finally returned
    Byte exitCode;

    bool executing = true;
    while (executing)
    {
        switch ((OpCode) byteCode[offset ++])
        {

        case OpCode::EXIT:

            exitCode = byteCode[offset];
            
            executing = false;
            break;
        

        case OpCode::CMP:
            // set zero flag register to the result of comparison (see x86 asm)
            rZeroFlag = rGeneralA == rGeneralB;
            break;


        case OpCode::CMP_REVERSE:
            // set zero flag register to the result of comparison (see x86 asm)
            rZeroFlag = rGeneralA != rGeneralB;
            break;
        

        case OpCode::ADD:
            // add value stored in B to A
            rResult = rGeneralA + rGeneralB;

            // set the sign flag (true if result is negative, else false)
            rSignFlag = rResult < 0;

            // set the zero flag
            rZeroFlag = rResult == 0;

            break;
        

        case OpCode::SUB:
            rResult = rGeneralA - rGeneralB;

            rSignFlag = rResult < 0;

            rZeroFlag = rResult == 0;

            break;
        

        case OpCode::MUL:
            rResult = rGeneralA * rGeneralB;

            rSignFlag = rResult < 0;

            rZeroFlag = rResult == 0;

            break;
        

        case OpCode::DIV:
            rResult = rGeneralA / rGeneralB;

            rDivisionRemainder = rResult % rGeneralB;

            rZeroFlag = rResult == 0;

            break;

        
        case OpCode::LD_CONST_A:
            rGeneralA = getLongValue(byteCode, offset);
            break;


        case OpCode::LD_CONST_B:
            rGeneralB = getLongValue(byteCode, offset);
            break;


        case OpCode::LD_CONST_RESULT:
            rResult = getLongValue(byteCode, offset);
            break;


        case OpCode::LD_A_8:
            rGeneralA = memory.getLong(
                getLongValue(byteCode, offset)
            );
            break;
        case OpCode::LD_A_4:
            rGeneralA = memory.getInt(
                getLongValue(byteCode, offset)
            );
            break;
        case OpCode::LD_A_1:
            rGeneralA = memory.getByte(
                getLongValue(byteCode, offset)
            );
            break;
        case OpCode::LD_A_BIT:
            rGeneralA = memory.getBit(
                getLongValue(byteCode, offset)
            );
            break;


        case OpCode::LD_B_8:
            rGeneralB = memory.getLong(
                    getLongValue(byteCode, offset)
                );
                break;
        case OpCode::LD_B_4:
            rGeneralB = memory.getInt(
                    getLongValue(byteCode, offset)
                );
                break;
        case OpCode::LD_B_1:
            rGeneralB = memory.getByte(
                    getLongValue(byteCode, offset)
                );
                break;
        case OpCode::LD_B_BIT:
            rGeneralB = memory.getBit(
                    getLongValue(byteCode, offset)
                );
                break;


        case OpCode::LD_RESULT_8:
            rResult = memory.getLong(
                getLongValue(byteCode, offset)
            );
            break;
        case OpCode::LD_RESULT_4:
            rResult = memory.getInt(
                getLongValue(byteCode, offset)
            );
            break;    
        case OpCode::LD_RESULT_1:
            rResult = memory.getByte(
                getLongValue(byteCode, offset)
            );
            break;    
        case OpCode::LD_RESULT_BIT:
            rResult = memory.getBit(
                getLongValue(byteCode, offset)
            );
            break;       


        case OpCode::LD_ZERO_FLAG:
            rZeroFlag = memory.getBit(
                getLongValue(byteCode, offset)
            );
            break;         


        case OpCode::MEM_MOV_8:
        {
            const Address addr1 = getLongValue(byteCode, offset);

            const Address addr2 = getLongValue(byteCode, offset);

            memory.set(addr1, memory.getLong(addr2));

            break;
        }
        case OpCode::MEM_MOV_4:
        {
            const Address addr1 = getLongValue(byteCode, offset);

            const Address addr2 = getLongValue(byteCode, offset);

            memory.set(addr1, memory.getInt(addr2));

            break;
        }
        case OpCode::MEM_MOV_1:
        {
            const Address addr1 = getLongValue(byteCode, offset);

            const Address addr2 = getLongValue(byteCode, offset);

            memory.set(addr1, memory.getByte(addr2));

            break;
        }
        case OpCode::MEM_MOV_BIT:
        {
            const Address addr1 = getLongValue(byteCode, offset);

            const Address addr2 = getLongValue(byteCode, offset);

            memory.set(addr1, memory.getBit(addr2));

            break;
        }


        case OpCode::REG_MOV_8:
        {
            const Address address = getLongValue(byteCode, offset);

            const Registers reg = (Registers) getByteValue(byteCode, offset);

            memory.set(address, *((long*) getRegister(reg)));
            
            break;
        }
        case OpCode::REG_MOV_4:
        {
            const Address address = getLongValue(byteCode, offset);

            const Registers reg = (Registers) getByteValue(byteCode, offset);

            memory.set(address, *((int*) getRegister(reg)));
            
            break;
        }
        case OpCode::REG_MOV_1:
        {
            const Address address = getLongValue(byteCode, offset);

            const Registers reg = (Registers) getByteValue(byteCode, offset);

            memory.set(address, *((Byte*) getRegister(reg)));
            
            break;
        }
        case OpCode::REG_MOV_BIT:
        {
            const Address address = getLongValue(byteCode, offset);

            const Registers reg = (Registers) getByteValue(byteCode, offset);

            memory.set(address, *((bool*) getRegister(reg)));
            
            break;
        }   


        case OpCode::REG_TO_REG:
        {
            const Registers regDest = (Registers) getByteValue(byteCode, offset);

            const Registers regSrc = (Registers) getByteValue(byteCode, offset);

            if (isBitRegister(regSrc))
            {
                *(long*) getRegister(regDest) = *(bool*) getRegister(regSrc);
            }
            else
            {
                *(long*) getRegister(regDest) = *(long*) getRegister(regSrc);
            }

            break;
        }


        case OpCode::MEM_SET_8:
        {
            const Address address = getLongValue(byteCode, offset);

            const long value = getLongValue(byteCode, offset);

            memory.set(address, value);

            break;
        }
        case OpCode::MEM_SET_4:
        {
            const Address address = getLongValue(byteCode, offset);

            const int value = getIntValue(byteCode, offset);

            memory.set(address, value);

            break;
        }
        case OpCode::MEM_SET_1:
        {
            const Address address = getLongValue(byteCode, offset);

            const Byte value = getByteValue(byteCode, offset);

            memory.set(address, value);

            break;
        }
        case OpCode::MEM_SET_BIT:
        {
            const Address address = getLongValue(byteCode, offset);

            const bool value = (bool) getByteValue(byteCode, offset);

            memory.set(address, value);

            break;
        }


        case OpCode::JMP:
            // set offset to the instruction to jump to
            offset = *((long*) (byteCode + offset));
            break;


        case OpCode::IF_JUMP:
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
        
        
        case OpCode::IF_NOT_JUMP:
            if (!rZeroFlag)
            {
                offset = *((long*) (byteCode + offset));
                break;
            }

            offset += sizeof(long);

            break;
            

        case OpCode::PUSH_CONST:
            memory.set(
                rStackPointer,
                getLongValue(byteCode, offset)
            );

            rStackPointer += sizeof(long);

            break;


        case OpCode::PUSH_REG:
        {
            const long value = *(long*) getRegister(
                (Registers) getByteValue(byteCode, offset)
            );

            memory.set(rStackPointer, value);

            rStackPointer += sizeof(long);

            break;
        }


        case OpCode::PUSH_BYTES:
            rStackPointer += getLongValue(byteCode, offset);
            break;


        case OpCode::POP:
            rStackPointer -= getLongValue(byteCode, offset);
            break;


        case OpCode::PRINT:
            std::cout << rGeneralA;
            break;


        } // switch ((OpCode) byte)

    } // while (executing)


    return exitCode;
}

