#pragma once

#include "pch.hh"


#define isLogicalOp(opCode) (OpCodes::LOGICAL_AND < opCode && opCode < OpCodes::ARITHMETICAL_SUM)

#define isArithmeticalOp(opCode) (OpCodes::LOGICAL_NOT < opCode && opCode < OpCodes::ASSIGNMENT_ASSIGN)

#define isAssignmentOp(opCode) (OpCodes::ARITHMETICAL_DEC < opCode && opCode < OpCodes::DECLARATION_INT)

#define isDeclarationOp(opCode) (OpCodes::ASSIGNMENT_POW < opCode && opCode < OpCodes::LITERAL)

#define isFlowOp(opCode) (OpCodes::PARENTHESIS < opCode && opCode < OpCodes::NO_OP)

#define isValue(opCode) (OpCodes::LITERAL == opCode || OpCodes::REFERENCE == opCode)

#define isOperator(opCode) (!isValue(opCode) && opCode != OpCodes::NO_OP)

#define isScope(opCode) (opCode == OpCodes::PUSH_SCOPE || opCode == OpCodes::POP_SCOPE)


// operator type, compatible with integers since it represents
// the number of operands an operator requires:
// - STANDALONE = 0
// - UNARY = 1
// - BINARY = 2
typedef enum class OpType
{
    STANDALONE = 0, // 0, requires zero operands
    UNARY = 1,      // 1, requires one operand
    BINARY = 2      // 2, requires two operands
} OpType;


// enum values must be constants for lookup table
typedef enum class OpCodes
{
    LOGICAL_AND         = 0,
    LOGICAL_OR          = 1,
    LOGICAL_EQ          = 2,
    LOGICAL_NOT_EQ      = 3,
    LOGICAL_GREATER     = 4,
    LOGICAL_LESS        = 5,
    LOGICAL_LESS_EQ     = 6,
    LOGICAL_GREATER_EQ  = 7,
    LOGICAL_NOT         = 8,

    ARITHMETICAL_SUM    = 9,
    ARITHMETICAL_SUB    = 10,
    ARITHMETICAL_MUL    = 11,
    ARITHMETICAL_DIV    = 12,
    ARITHMETICAL_POW    = 13,
    ARITHMETICAL_MOD    = 14,
    ARITHMETICAL_INC    = 15,
    ARITHMETICAL_DEC    = 16,

    ASSIGNMENT_ASSIGN   = 17,
    ASSIGNMENT_ADD      = 18,
    ASSIGNMENT_SUB      = 19,
    ASSIGNMENT_MUL      = 20,
    ASSIGNMENT_DIV      = 21,
    ASSIGNMENT_POW      = 22,

    DECLARATION_INT     = 23,
    DECLARATION_FLOAT   = 24,
    DECLARATION_STRING  = 25,
    DECLARATION_BOOL    = 26,

    LITERAL             = 27,

    REFERENCE           = 28,

    ADDRESS_OF          = 29,

    PUSH_SCOPE          = 30,
    POP_SCOPE           = 31,

    CALL                = 32,

    PARENTHESIS         = 33,

    FLOW_IF             = 34,
    FLOW_ELSE           = 35,
    FLOW_FOR            = 36,
    FLOW_WHILE          = 37,

    NO_OP               = 38

} OpCodes;


OpType operatorType(OpCodes op);


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode);

