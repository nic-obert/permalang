#include "tac.hh"

#include <iostream>


using namespace tac;


std::ostream& operator<<(std::ostream& stream, TacOp const& op)
{
    switch (op)
    {
    case TacOp::IF:
        return stream << "if";
    case TacOp::JUMP:
        return stream << "jump";
    case TacOp::LABEL:
        return stream << "label";
    case TacOp::ASSIGN:
        return stream << '=';
    case TacOp::SUM:
        return stream << '+';
    case TacOp::SUB:
        return stream << '-';
    case TacOp::MUL:
        return stream << '*';
    case TacOp::DIV:
        return stream << '/';
    case TacOp::EQ:
        return stream << "==";
    case TacOp::NO_OP:
        return stream << "NO OP";
    
    }
}


