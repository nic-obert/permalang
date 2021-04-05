#include "op_codes.hh"
#include "token.hh"
#include "operators.hh"



const char* operators::assignment::assignmentOperatorName(OpCodes op)
{
    switch (op)
    {
    case ASSIGNMENT_ASSIGN:
        return "=";
    
    case ASSIGNMENT_ADD:
        return "+=";

    case ASSIGNMENT_SUB:
        return "-=";

    case ASSIGNMENT_MUL:
        return "*=";

    case ASSIGNMENT_DIV:
        return "/=";
    
    case ASSIGNMENT_POW:
        return "^=";
    
    }

    return "UNDEFINED ASSIGNMENT OPERATOR";
}



