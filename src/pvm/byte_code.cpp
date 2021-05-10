#include "pvm.hh"


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

    size_t i = 0;
    size_t line = 0;

    while (true)
    {
        line ++;

        stream << '\t' << line << '\t';

        switch ((OpCode) byteCode[i])
        {
        case OpCode::ADD:
            stream << "add\n";
            i ++;
            continue;
        
        case OpCode::SUB:
            stream << "sub\n";
            i ++;
            continue;
        
        case OpCode::MUL:
            stream << "mul\n";
            i ++;
            continue;

        case OpCode::DIV:
            stream << "div\n";
            i ++;
            continue;
        
        case OpCode::CMP:
            stream << "cmp\n";
            i ++;
            continue;
        
        case OpCode::EXIT:
            stream << "exit\n";
            // breaking the switch statement means breaking the while loop
            break;
        
        case OpCode::JMP:
            i ++;
            stream << "jmp @["
                << *((long*) (byteCode + i))
                << "]\n";
                
            i += sizeof(long);
            continue;
        
        case OpCode::IF_JUMP:
            i ++;
            stream << "ifjmp @["
                << *((long*) (byteCode + i))
                << "]\n";
                
            i += sizeof(long);
            continue;

        case OpCode::PUSH:
            stream << "push\n";
            i ++;
            continue;
        
        case OpCode::POP:
            stream << "pop\n";
            i ++;
            continue;

        case OpCode::LDA:
            i ++;
            stream << "lda ["
                << *((long*) (byteCode + i))
                << "]\n";

            i += sizeof(long);
            continue;

        case OpCode::LDB:
            i ++;
            stream << "ldb ["
                << *((long*) (byteCode + i))
                << "]\n";

            i += sizeof(long);
            continue;

        case OpCode::LDCA:
            i ++;
            stream << "ldca "
                << *((long*) (byteCode + i))
                << '\n';

            i += sizeof(long);
            continue;
        
        case OpCode::LDCB:
            i ++;
            stream << "ldcb "
                << *((long*) (byteCode + i))
                << '\n';

            i += sizeof(long);
            continue;
        
        case OpCode::MEM_MOV:
            i ++;
            stream << "memmov ["
                << *((long*) (byteCode + i))
                << "], [";
            
            i += sizeof(long);

            stream << *((long*) (byteCode + i))
                << "]\n";

            i += sizeof(long);
            continue;
        
        case OpCode::REG_MOV:
            i ++;
            stream << "memmov "
                << (Registers) byteCode[i]
                << ", [";
            
            i ++;

            stream << *((long*) (byteCode + i))
                << "]\n";

            i += sizeof(long);
            continue;
        
        case OpCode::REG_MOV_BIT:
            i ++;
            stream << "memmovbit "
                << (Registers) byteCode[i]
                << ", [";
            
            i ++;

            stream << *((long*) (byteCode + i))
                << "]\n";

            i += sizeof(long);
            continue;
        
        case OpCode::MEM_SET:
            i ++;
            stream << "memset ["
                << *((long*) (byteCode + i))
                << "], ";
            
            i += sizeof(long);

            stream << *((long*) (byteCode + i))
                << '\n';

            i += sizeof(long);
            continue;

        } // switch ((OpCode) byteCode[i])

        // if flow reaches this line the while loop is terminated
        break;

    } // while (true)

    return stream << '}';
}

