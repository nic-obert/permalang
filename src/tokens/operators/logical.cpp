
#include "token.hh"
#include "operators.hh"



const char* operators::logical::logicalOperatorName(LogicalOperators op)
{
    switch (op)
    {
    case EQUALITY:
        return "==";
    
    case INEQUALITY:
        return "!=";
    
    case AND:
        return "&&";
    
    case OR:
        return "||";

    case GREATER_THAN:
        return ">";
    
    case LESS_THAN:
        return "<";

    case GREATER_EQUAL:
        return ">=";
    
    case LESS_EQUAL:
        return "<=";
        
    }
}

