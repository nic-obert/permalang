#include "operators.hh"


const char* operators::arithmetical::arithmeticalOperatorName(OpCodes op)
{
    switch (op)
    {
    case OpCodes::ARITHMETICAL_SUM:
        return "+";

    case OpCodes::ARITHMETICAL_SUB:
        return "-";
    
    case OpCodes::ARITHMETICAL_MUL:
        return "*";

    case OpCodes::ARITHMETICAL_DIV:
        return "/";

    case OpCodes::ARITHMETICAL_POW:
        return "^";
    
    case OpCodes::ARITHMETICAL_INC:
        return "++";

    case OpCodes::ARITHMETICAL_DEC:
        return "--";
    
    }

    return "UNDEFINED ARITHMETICAL OPERATOR";

}

