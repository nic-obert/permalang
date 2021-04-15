#include "op_codes.hh"
#include "token.hh"
#include "operators.hh"



const char* operators::assignment::assignmentOperatorName(OpCodes op)
{
    switch (op)
    {
    case OpCodes::ASSIGNMENT_ASSIGN:
        return "=";
    
    case OpCodes::ASSIGNMENT_ADD:
        return "+=";

    case OpCodes::ASSIGNMENT_SUB:
        return "-=";

    case OpCodes::ASSIGNMENT_MUL:
        return "*=";

    case OpCodes::ASSIGNMENT_DIV:
        return "/=";
    
    case OpCodes::ASSIGNMENT_POW:
        return "^=";
    
    }

    return "UNDEFINED ASSIGNMENT OPERATOR";
}



