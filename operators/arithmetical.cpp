#pragma once

namespace operators 
{
    namespace arithmetical
    {
        typedef enum ArithmeticalOperators
        {
            SUM,
            SUBTRACTION,
            MULTIPLICATION,
            DIVISION,
            POWER,
            INCREMENT,
            DECREMENT
        } ArithmeticalOperators;


        const char* arithmeticalOperatorName(ArithmeticalOperators op)
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

    };
};
