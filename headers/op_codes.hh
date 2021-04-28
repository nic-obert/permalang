#pragma once

#include "pch.hh"


#define isLogicalOp(opCode) (OpCodes::LOGICAL_ < opCode && opCode < OpCodes::ARITHMETICAL_)

#define isArithmeticalOp(opCode) (OpCodes::ARITHMETICAL_ < opCode && opCode < OpCodes::ASSIGNMENT_)

#define isAssignmentOp(opCode) (OpCodes::ASSIGNMENT_ < opCode && opCode < OpCodes::DECLARATION_)

#define isDeclarationOp(opCode) (OpCodes::DECLARATION_ < opCode && opCode < OpCodes::LITERAL_)

#define isFlowOp(opCode) (OpCodes::FLOW_ < opCode && opCode < OpCodes::NO_OP)

#define isValue(opCode) (OpCodes::LITERAL == opCode || OpCodes::REFERENCE == opCode)

#define isOperator(opCode) (!isValue(opCode) && opCode != OpCodes::NO_OP)

#define isScope(opCode) (opCode == OpCodes::PUSH_SCOPE || opCode == OpCodes::POP_SCOPE)


typedef enum class OpType
{
    STANDALONE, // 0
    UNARY,      // 1
    BINARY      // 2
} OpType;


typedef enum class OpCodes
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

    ADDRESS_OF,

    PUSH_SCOPE,
    POP_SCOPE,

    CALL,

    PARENTHESIS,

    FLOW_,

    FLOW_IF,
    FLOW_ELSE,
    FLOW_FOR,
    FLOW_WHILE,

    NO_OP

} OpCodes;


OpType operatorType(OpCodes op);


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode);

