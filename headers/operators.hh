#pragma once

#include "token.hh"


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


    void satisfy(Tokens::Token* token);


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


    void satisfy(Tokens::Token* token);


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


    void satisfy(Tokens::Token* token);


    const char* assignmentOperatorName(AssignmentOperators op);

};
