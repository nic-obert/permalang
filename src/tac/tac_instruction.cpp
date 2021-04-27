#include "pch.hh"

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
        case TacOp::SUM:
        case TacOp::SUB:
        case TacOp::MUL:
        case TacOp::DIV:
        case TacOp::EQ:
            return stream << instruction.addr1 
                << " " << TacOp::ASSIGN << " " << " "
                << instruction.addr2 << " " 
                << instruction.operation << " " 
                << instruction.addr3;
        
        case TacOp::ASSIGN:
            return stream << instruction.addr1
                << " " << TacOp::ASSIGN << " " 
                << instruction.addr2;
        
        case TacOp::JUMP:
            return stream << TacOp::JUMP << " " 
                << instruction.addr1;
        
        case TacOp::IF:
            return stream << TacOp::IF << " "
                << instruction.addr1 << " "
                << TacOp::JUMP << " " 
                << instruction.addr2;
        
        case TacOp::LABEL:
            return stream << "@["
                << (void*) &instruction << "]:";

        default:
            return stream << instruction.operation;
    }
}


std::ostream& operator<<(std::ostream& stream, TacInstruction const* instruction)
{
    return stream << *instruction;
}
