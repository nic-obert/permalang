#include "tac.hh"

#include <iostream>


using namespace tac;


std::ostream& operator<<(std::ostream& stream, TacOp const& op)
{
    switch (op)
    {
    case IF:
        return stream << "if";
    case JUMP:
        return stream << "jump";
    case LABEL:
        return stream << "label";
    case ASSIGN:
        return stream << '=';
    case SUM:
        return stream << '+';
    case SUB:
        return stream << '-';
    case MUL:
        return stream << '*';
    case DIV:
        return stream << '/';
    case EQ:
        return stream << "==";
    case TacOp::NO_OP:
        return stream << "NO OP";
    
    }
}


