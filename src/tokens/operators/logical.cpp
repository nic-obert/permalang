#include "op_codes.hh"
#include "token.hh"
#include "operators.hh"



const char* operators::logical::logicalOperatorName(OpCodes op)
{
    switch (op)
    {
    case OpCodes::LOGICAL_EQ:
        return "==";
    
    case OpCodes::LOGICAL_NOT_EQ:
        return "!=";
    
    case OpCodes::LOGICAL_AND:
        return "&&";
    
    case OpCodes::LOGICAL_OR:
        return "||";

    case OpCodes::LOGICAL_GREATER:
        return ">";
    
    case OpCodes::LOGICAL_LESS:
        return "<";

    case OpCodes::LOGICAL_GREATER_EQ:
        return ">=";
    
    case OpCodes::LOGICAL_LESS_EQ:
        return "<=";
    
    case OpCodes::LOGICAL_NOT:
        return "!";
    }

    return "UNDEFINED LOGICAL OPERATOR";
}

