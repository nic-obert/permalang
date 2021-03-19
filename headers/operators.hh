#pragma once

#include "token.hh"
#include "syntax_tree.hh"


namespace operators{};


namespace operators::arithmetical
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


    const char* arithmeticalOperatorName(ArithmeticalOperators op);

};


namespace operators::logical
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


    const char* logicalOperatorName(LogicalOperators op);
    
};


namespace operators::assignment
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


    const char* assignmentOperatorName(AssignmentOperators op);

};
