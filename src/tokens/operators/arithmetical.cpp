#include "op_codes.hh"
#include "token.hh"
#include "operators.hh"
#include "syntax_tree.hh"



const char* operators::arithmetical::arithmeticalOperatorName(OpCodes op)
{
    switch (op)
    {
    case ARITHMETICAL_SUM:
        return "+";

    case ARITHMETICAL_SUB:
        return "-";
    
    case ARITHMETICAL_MUL:
        return "*";

    case ARITHMETICAL_DIV:
        return "/";

    case ARITHMETICAL_POW:
        return "^";
    
    case ARITHMETICAL_INC:
        return "++";

    case ARITHMETICAL_DEC:
        return "--";
    
    }

    return "UNDEFINED ARITHMETICAL OPERATOR";

}

