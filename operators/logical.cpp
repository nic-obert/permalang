#pragma once

namespace operators
{
    namespace logical
    {
        typedef enum LogicalOperators
        {
            EQUALITY,
            INEQUALITY,
            AND,
            OR,
            GREATER_THAN,
            LESS_THAN,
            GREATER_EQUAL,
            LESS_EQUAL
        } LogicalOperator;


        const char* logicalOperatorName(LogicalOperators op)
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

    };
};

