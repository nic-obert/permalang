#pragma once


#include <iostream>


#define isLogicalOp(x) (OpCodes::LOGICAL_ < x && x < OpCodes::ARITHMETICAL_)

#define isArithmeticalOp(x) (OpCodes::ARITHMETICAL_ < x && x < OpCodes::ASSIGNMENT_)

#define isAssignmentOp(x) (OpCodes::ASSIGNMENT_ < x && x < OpCodes::DECLARATION_)

#define isDeclarationOp(x) (OpCodes::DECLARATION_ < x && x < OpCodes::LITERAL_)

#define isFlowOp(x) (OpCodes::FLOW_ < x && x < OpCodes::NO_OP)

#define isValue(x) (OpCodes::LITERAL == x || OpCodes::REFERENCE == x)

#define isOperator(x) (!isValue(x) && x != OpCodes::NO_OP)


typedef enum OpType
{
    STANDALONE, // 0
    UNARY,      // 1
    BINARY      // 2
} OpType;


typedef enum OpCodes
{
    LOGICAL_,

    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_EQ,
    LOGICAL_NOT_EQ,
    LOGICAL_GREATER,
    LOGICAL_LESS,
    LOGICAL_LESS_EQ,
    LOGICAL_GREATER_EQ,
    LOGICAL_NOT,

    ARITHMETICAL_,

    ARITHMETICAL_SUM,
    ARITHMETICAL_SUB,
    ARITHMETICAL_MUL,
    ARITHMETICAL_DIV,
    ARITHMETICAL_POW,
    ARITHMETICAL_MOD,
    ARITHMETICAL_INC,
    ARITHMETICAL_DEC,

    ASSIGNMENT_,

    ASSIGNMENT_ASSIGN,
    ASSIGNMENT_ADD,
    ASSIGNMENT_SUB,
    ASSIGNMENT_MUL,
    ASSIGNMENT_DIV,
    ASSIGNMENT_POW,

    DECLARATION_,

    DECLARATION_INT,
    DECLARATION_FLOAT,
    DECLARATION_STRING,
    DECLARATION_BOOL,

    LITERAL,

    REFERENCE,

    FLOW_,

    FLOW_IF,
    FLOW_ELSE,
    FLOW_FOR,
    FLOW_WHILE,

    NO_OP

} OpCodes;


OpType operatorType(OpCodes op);


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode);

