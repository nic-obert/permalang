#include "pvm.hh"


#define longArray *((long*) (bytes + i))



using namespace pvm;


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

    for (size_t line = 1, i = 0; true; line++)
    {

        stream << '\t' << line << '\t';

        // switch byte and increment index
        switch ((OpCode) bytes[i++])
        {
        case OpCode::ADD:
            stream << "add:\n";
            continue;
        
        case OpCode::SUB:
            stream << "sub:\n";
            continue;
        
        case OpCode::MUL:
            stream << "mul:\n";
            continue;

        case OpCode::DIV:
            stream << "div:\n";
            continue;
        
        case OpCode::CMP:
            stream << "cmp:\n";
            continue;
        
        case OpCode::CMP_REVERSE:
            stream << "cmp reverse:\n";
            continue;
        
        case OpCode::EXIT:
        {
            const char* exitCode = charToASCII(bytes[i]);
            stream << "exit: " << exitCode << '\n';
            free((void*) exitCode);
            i ++;
            // breaking the switch statement means breaking the while loop
            break;
        }
        
        case OpCode::JMP:
            stream << "jmp: @["
                << longArray
                << "]\n";
                
            i += sizeof(long);
            continue;
        
        case OpCode::IF_JUMP:
            stream << "if jmp: @["
                << longArray
                << "]\n";
                
            i += sizeof(long);
            continue;

        case OpCode::PUSH_CONST:
            stream << "push const: ";
            
            stream << longArray << '\n';
            i += sizeof(long);
            continue;
        
        case OpCode::PUSH_REG:
            stream << "push reg: ";

            stream << (Registers) bytes[i] << '\n';
            i++;
            continue;
        
        case OpCode::PUSH_BYTES:
            stream << "push bytes: ";
            
            stream << longArray << '\n';
            i += sizeof(long);
            continue;
        
        case OpCode::POP:
            stream << "pop: ";

            stream << longArray << '\n';
            i += sizeof(long);
            continue;

        case OpCode::LD_A:
            stream << "ld a: ["
                << longArray
                << "]\n";

            i += sizeof(long);
            continue;

        case OpCode::LD_B:
            stream << "ld b: ["
                << longArray
                << "]\n";

            i += sizeof(long);
            continue;

        case OpCode::LD_CONST_A:
            stream << "ld const a: "
                << longArray
                << '\n';

            i += sizeof(long);
            continue;
        
        case OpCode::LD_CONST_B:
            stream << "ld const b: "
                << longArray
                << '\n';

            i += sizeof(long);
            continue;
        
        case OpCode::MEM_MOV:
            stream << "mem mov: ["
                << longArray
                << "], [";
            
            i += sizeof(long);

            stream << longArray
                << "]\n";

            i += sizeof(long);
            continue;
        
        case OpCode::REG_MOV:
            stream << "reg mov: ["
                << longArray
                << "], ";
            
            i += sizeof(long);

            stream << (Registers) bytes[i] << "\n";

            i ++;
            continue;
        
        case OpCode::REG_MOV_BIT:
            stream << "reg mov bit: ["
                << longArray
                << "], ";
            
            i += sizeof(long);

            stream << (Registers) bytes[i] << "\n";

            i ++;
            continue;
        
        case OpCode::REG_TO_REG:
            stream << "reg to reg: "
                << (Registers) bytes[i] << ", ";

            i ++;

            stream << (Registers) bytes[i] << "\n";

            i++;
            continue;
        
        case OpCode::MEM_SET:
            stream << "mem set: ["
                << longArray
                << "], ";
            
            i += sizeof(long);

            stream << longArray
                << '\n';

            i += sizeof(long);
            continue;
        
        case OpCode::CALL:
            stream << "call (unhandled)\n";
            continue;
        
        case OpCode::PRINT:
            stream << "print:\n";
            continue;

        } // switch ((OpCode) byteCode[i])

        // if flow reaches this line the while loop is terminated
        break;

    } // while (true)

    return stream << '}';
}

