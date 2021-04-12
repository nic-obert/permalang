#include "tac.hh"


using namespace tac;


TacInstruction::TacInstruction(TacOp operation)
: operation(operation)
{

}


TacInstruction::TacInstruction(TacOp operation, TacValue* addr1)
: operation(operation), addr1(addr1)
{

}


TacInstruction::TacInstruction(TacOp operation, TacValue* addr1, TacValue* addr2)
: operation(operation), addr1(addr1), addr2(addr2)
{
    
}


TacInstruction::TacInstruction(TacOp operation, TacValue* addr1, TacValue* addr2, TacValue* addr3)
: operation(operation), addr1(addr1), addr2(addr2), addr3(addr3)
{
    
}



std::ostream& operator<<(std::ostream& stream, TacInstruction const& instruction)
{
    switch (instruction.operation)
    {
        case SUM:
        case SUB:
        case MUL:
        case DIV:
        case EQ:
            return stream << instruction.addr1 
                << " " << TacOp::ASSIGN << " " << " "
                << instruction.addr2 << " " 
                << instruction.operation << " " 
                << instruction.addr3;
        
        case ASSIGN:
            return stream << instruction.addr1
                << " " << TacOp::ASSIGN << " " 
                << instruction.addr2;
        
        case JUMP:
            return stream << instruction.operation << " " 
                << instruction.addr1;
        
        case IF:
            return stream << instruction.operation << " "
                << instruction.addr1 << " "
                << TacOp::JUMP << " " 
                << instruction.addr2;

        default:
            return stream << instruction.operation;
    }
}


std::ostream& operator<<(std::ostream& stream, TacInstruction const* instruction)
{
    return stream << *instruction;
}
