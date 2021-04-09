#include "tac.hh"


using namespace tac;


TacInstruction::TacInstruction(TacOp operation)
: operation(operation)
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

