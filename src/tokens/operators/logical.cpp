#include "op_codes.hh"
#include "token.hh"
#include "operators.hh"



const char* operators::logical::logicalOperatorName(OpCodes op)
{
    switch (op)
    {
    case LOGICAL_EQ:
        return "==";
    
    case LOGICAL_NOT_EQ:
        return "!=";
    
    case LOGICAL_AND:
        return "&&";
    
    case LOGICAL_OR:
        return "||";

    case LOGICAL_GREATER:
        return ">";
    
    case LOGICAL_LESS:
        return "<";

    case LOGICAL_GREATER_EQ:
        return ">=";
    
    case LOGICAL_LESS_EQ:
        return "<=";
    
    case LOGICAL_NOT:
        return "!";
    }

    return "UNDEFINED LOGICAL OPERATOR";
}

