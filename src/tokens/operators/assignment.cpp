
#include "token.hh"
#include "operators.hh"



const char* operators::assignment::assignmentOperatorName(AssignmentOperators op)
{
    switch (op)
    {
    case ASSIGNMENT:
        return "=";
    
    case ADD:
        return "+=";

    case SUBTRACT:
        return "-=";

    case MULTIPLY:
        return "*=";

    case DIVIDE:
        return "/=";
    
    case ELEVATE:
        return "^=";
    
    }
}



