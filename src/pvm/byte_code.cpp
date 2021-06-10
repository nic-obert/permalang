#include "pvm.hh"
#include "errors.hh"


using namespace pvm;


static inline long getLong(const Byte* bytes, size_t& i)
{
    const long value = *((long*) (bytes + i));
    i += sizeof(long);
    return value;
}


static inline int getInt(const Byte* bytes, size_t& i)
{
    const int value = *((int*) (bytes + i));
    i += sizeof(int);
    return value;
}


static inline Byte getByte(const Byte* bytes, size_t& i)
{
    return bytes[i++];
}


ByteCode::ByteCode(Byte* byteCode, size_t size)
: byteCode(byteCode), size(size)
{
    
}


ByteCode::ByteCode()
: byteCode(nullptr), size(0)
{
    
}


std::ostream& operator<<(std::ostream& stream, const ByteCode& byteCode)
{
    const Byte* bytes = byteCode.byteCode;

    stream << "ByteCode (" << byteCode.size << "): {\n";

    for (size_t line = 1, i = 0; i != byteCode.size; line++)
    {

        OpCode opCode = (OpCode) bytes[i];

        stream << '#' << line << "\t@" << i << '\t' << opCode << ": ";

        // increment only after printing
        i++;

        // switch byte and increment index
        switch (opCode)
        {
        case OpCode::ADD:
            stream << '\n';
            continue;
        
        case OpCode::SUB:
            stream << '\n';
            continue;
        
        case OpCode::MUL:
            stream << '\n';
            continue;

        case OpCode::DIV:
            stream << '\n';
            continue;
        
        case OpCode::CMP:
            stream << '\n';
            continue;
        
        case OpCode::CMP_REVERSE:
            stream << '\n';
            continue;
        
        case OpCode::EXIT:
        {
            stream << (unsigned int) getByte(bytes, i) << '\n';            
            continue;
        }
        
        case OpCode::JMP:
            stream << "@[" << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::IF_JUMP:
            stream << "@[" << getLong(bytes, i) << "]\n";                
            continue;
        
        case OpCode::IF_NOT_JUMP:
            stream << "@[" << getLong(bytes, i) << "]\n";
            continue;

        case OpCode::PUSH_CONST:
            stream << getLong(bytes, i) << '\n';
            continue;
        
        case OpCode::PUSH_REG:
            stream << (Registers) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::PUSH_BYTES:
            stream << getLong(bytes, i) << '\n';
            continue;
        
        case OpCode::POP:
            stream << getLong(bytes, i) << '\n';
            continue;

        case OpCode::LD_A_8:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_A_4:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_A_1:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_A_BIT:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;

        case OpCode::LD_B_8:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_B_4:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_B_1:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_B_BIT:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_RESULT_8:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_RESULT_4:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_RESULT_1:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_RESULT_BIT:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::LD_ZERO_FLAG:
            stream << '[' << getLong(bytes, i) << "]\n";
            continue;

        case OpCode::LD_CONST_A_8:
            stream << getLong(bytes, i) << '\n';
            continue;
        
        case OpCode::LD_CONST_A_4:
            stream << getInt(bytes, i) << '\n';
            continue;
        
        case OpCode::LD_CONST_A_1:
        case OpCode::LD_CONST_A_BIT:
            stream << (unsigned int) getByte(bytes, i) << '\n';
            continue;

        case OpCode::LD_CONST_B_8:
            stream << getLong(bytes, i) << '\n';
            continue;
        
        case OpCode::LD_CONST_B_4:
            stream << getInt(bytes, i) << '\n';
            continue;
        
        case OpCode::LD_CONST_B_1:
        case OpCode::LD_CONST_B_BIT:
            stream << (unsigned int) getByte(bytes, i) << '\n';
            continue;

        case OpCode::LD_CONST_RESULT_8:
            stream << getLong(bytes, i) << '\n';
            continue;
        
        case OpCode::LD_CONST_RESULT_4:
            stream << getInt(bytes, i) << '\n';
            continue;
        
        case OpCode::LD_CONST_RESULT_1:
        case OpCode::LD_CONST_RESULT_BIT:
            stream << (unsigned int) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::MEM_MOV_8:
            stream << '[' << getLong(bytes, i) << "], ["
                << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::MEM_MOV_4:
            stream << '[' << getLong(bytes, i) << "], ["
                << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::MEM_MOV_1:
            stream << '[' << getLong(bytes, i) << "], ["
                << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::MEM_MOV_BIT:
            stream << '[' << getLong(bytes, i) << "], ["
                << getLong(bytes, i) << "]\n";
            continue;
        
        case OpCode::REG_MOV_8:
            stream << '[' << getLong(bytes, i) << "], "
                << (Registers) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::REG_MOV_4:
            stream << '[' << getLong(bytes, i) << "], "
                << (Registers) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::REG_MOV_1:
            stream << '[' << getLong(bytes, i) << "], "
                << (Registers) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::REG_MOV_BIT:
            stream << '[' << getLong(bytes, i) << "], "
                << (Registers) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::REG_TO_REG:
            stream << (Registers) getByte(bytes, i) << ", "
                << (Registers) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::MEM_SET_8:
            stream << '[' << getLong(bytes, i) << "], "
                << getLong(bytes, i) << '\n';
            continue;
        
        case OpCode::MEM_SET_4:
            stream << '[' << getLong(bytes, i) << "], "
                << getInt(bytes, i) << '\n';
            continue;
        
        case OpCode::MEM_SET_1:
            stream << '[' << getLong(bytes, i) << "], "
                << (unsigned int) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::MEM_SET_BIT:
            stream << '[' << getLong(bytes, i) << "], "
                << (unsigned int) getByte(bytes, i) << '\n';
            continue;
        
        case OpCode::CALL:
            stream << "unhandled";
            continue;
        
        case OpCode::PRINT:
            stream << ":\n";
            continue;

        } // switch ((OpCode) byteCode[i])

        // if flow reaches this line the while loop is terminated

        if ((OpCode) bytes[i-1] != OpCode::EXIT)
        {
            std::string msg("Unhandled OpCode in byte code printing: ");

            std::string strOpCode;
            string_utils::byteToString(bytes[i-1], strOpCode);

            msg += strOpCode;

            errors::UnexpectedBehaviourError(msg);  
        }

        break;

    } // while (true)

    return stream << '}';
}


static const char* opCodeNames[] =
{
    "exit",
    "add",
    "sub",
    "mul",
    "div",
    "cmp",
    "cmp reverse",
    "ld const A 8",
    "ld const A 4",
    "ld const A 1",
    "ld const A bit",
    "ld const B 8",
    "ld const B 4",
    "ld const B 1",
    "ld const B bit",
    "ld const RESULT 8",
    "ld const RESULT 4",
    "ld const RESULT 1",
    "ld const RESULT bit",
    "ld A 8",
    "ld A 4",
    "ld A 1",
    "ld A bit",
    "ld B 8",
    "ld B 4",
    "ld B 1",
    "ld B bit",
    "ld RESULT 8",
    "ld RESULT 4",
    "ld RESULT 1",
    "ld RESULT bit",
    "ld ZERO FLAG",
    "mem mov 8",
    "mem mov 4",
    "mem mov 1",
    "mem mov bit",
    "reg mov 8",
    "reg mov 4",
    "reg mov 1",
    "reg mov bit",
    "reg to reg",
    "mem set 8",
    "mem set 4",
    "mem set 1",
    "mem set bit",
    "jmp",
    "if jump",
    "if not jump",
    "push const",
    "push reg",
    "push bytes",
    "pop",
    "call",
    "print",
    "no op"    
};


std::ostream& operator<<(std::ostream& stream, const OpCode opCode)
{
    return stream << opCodeNames[(unsigned char) opCode];
}

