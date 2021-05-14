#include "pvm.hh"


#define longArray *((long*) (byteCode + i))



using namespace pvm;


ByteCode::ByteCode(Byte* byteCode, size_t size)
: byteCode(byteCode), size(size)
{
    
}


ByteCode::ByteCode()
: byteCode(nullptr), size(0)
{
    
}


std::ostream& operator<<(std::ostream& stream, const Byte* byteCode)
{
    stream << "ByteCode: {\n";

    for (size_t line = 1, i = 0; true; line++)
    {

        stream << '\t' << line << '\t';

        // switch byte and increment index
        switch ((OpCode) byteCode[i++])
        {
        case OpCode::ADD:
            stream << "add\n";
            continue;
        
        case OpCode::SUB:
            stream << "sub\n";
            continue;
        
        case OpCode::MUL:
            stream << "mul\n";
            continue;

        case OpCode::DIV:
            stream << "div\n";
            continue;
        
        case OpCode::CMP:
            stream << "cmp\n";
            continue;
        
        case OpCode::EXIT:
            stream << "exit\n";
            // breaking the switch statement means breaking the while loop
            break;
        
        case OpCode::JMP:
            stream << "jmp @["
                << longArray
                << "]\n";
                
            i += sizeof(long);
            continue;
        
        case OpCode::IF_JUMP:
            stream << "ifjmp @["
                << longArray
                << "]\n";
                
            i += sizeof(long);
            continue;

        case OpCode::PUSH:
            stream << "push ";
            
            stream << longArray << '\n';
            i += sizeof(long);
            continue;
        
        case OpCode::POP:
            stream << "pop ";

            stream << longArray << '\n';
            i += sizeof(long);
            continue;

        case OpCode::LDA:
            stream << "lda ["
                << longArray
                << "]\n";

            i += sizeof(long);
            continue;

        case OpCode::LDB:
            stream << "ldb ["
                << longArray
                << "]\n";

            i += sizeof(long);
            continue;

        case OpCode::LDCA:
            stream << "ldca "
                << longArray
                << '\n';

            i += sizeof(long);
            continue;
        
        case OpCode::LDCB:
            stream << "ldcb "
                << longArray
                << '\n';

            i += sizeof(long);
            continue;
        
        case OpCode::MEM_MOV:
            stream << "memmov ["
                << longArray
                << "], [";
            
            i += sizeof(long);

            stream << longArray
                << "]\n";

            i += sizeof(long);
            continue;
        
        case OpCode::REG_MOV:
            stream << "regmov "
                << (Registers) byteCode[i]
                << ", [";
            
            i ++;

            stream << longArray
                << "]\n";

            i += sizeof(long);
            continue;
        
        case OpCode::REG_MOV_BIT:
            stream << "regmovbit "
                << (Registers) byteCode[i]
                << ", [";
            
            i ++;

            stream << longArray
                << "]\n";

            i += sizeof(long);
            continue;
        
        case OpCode::MEM_SET:
            stream << "memset ["
                << longArray
                << "], ";
            
            i += sizeof(long);

            stream << longArray
                << '\n';

            i += sizeof(long);
            continue;

        } // switch ((OpCode) byteCode[i])

        // if flow reaches this line the while loop is terminated
        break;

    } // while (true)

    return stream << '}';
}


std::ostream& operator<<(std::ostream& stream, const Byte byte)
{
    return stream << (unsigned char) byte;
}

