
#include "token.hh"
#include "operators.hh"



void operators::arithmetical::satisfy(Tokens::Token* token)
{

}


const char* operators::arithmetical::arithmeticalOperatorName(ArithmeticalOperators op)
{
    switch (op)
    {
    case SUM:
        return "+";

    case SUBTRACTION:
        return "-";
    
    case MULTIPLICATION:
        return "*";

    case DIVISION:
        return "/";

    case POWER:
        return "^";
    
    case INCREMENT:
        return "++";

    case DECREMENT:
        return "--";
    
    }
}

