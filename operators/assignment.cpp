#pragma once

namespace operators
{
    namespace assignment
    {
        typedef enum AssignmentOperators
        {
            ASSIGNMENT,
            ADD,
            SUBTRACT,
            MULTIPLY,
            DIVIDE,
            ELEVATE
        } AssignmentOperators;


        const char* assignmentOperatorName(AssignmentOperators op)
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

    };
};

