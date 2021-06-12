#pragma once

#include "pch.hh"


#define isLogicalOp(opCode) (OpCodes::LOGICAL_AND < opCode && opCode < OpCodes::ARITHMETICAL_SUM)

#define isArithmeticalOp(opCode) (OpCodes::LOGICAL_NOT < opCode && opCode < OpCodes::ASSIGNMENT_ASSIGN)

#define isAssignmentOp(opCode) (OpCodes::ARITHMETICAL_DEC < opCode && opCode < OpCodes::DECLARATION_INT)

#define isDeclarationOp(opCode) (OpCodes::ASSIGNMENT_POW < opCode && opCode < OpCodes::LITERAL)

#define isFlowOp(opCode) (OpCodes::CLOSE_PARENTHESIS < opCode && opCode < OpCodes::NO_OP)

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
    LOGICAL_AND,
    LOGICAL_OR,
    LOGICAL_EQ,
    LOGICAL_NOT_EQ,
    LOGICAL_GREATER,
    LOGICAL_LESS,
    LOGICAL_LESS_EQ,
    LOGICAL_GREATER_EQ,
    LOGICAL_NOT,

    ARITHMETICAL_SUM,
    ARITHMETICAL_SUB,
    ARITHMETICAL_MUL,
    ARITHMETICAL_DIV,
    ARITHMETICAL_POW,
    ARITHMETICAL_MOD,
    ARITHMETICAL_INC,
    ARITHMETICAL_DEC,

    ASSIGNMENT_ASSIGN,
    ASSIGNMENT_ADD,
    ASSIGNMENT_SUB,
    ASSIGNMENT_MUL,
    ASSIGNMENT_DIV,
    ASSIGNMENT_POW,

    DECLARATION_INT,
    DECLARATION_FLOAT,
    DECLARATION_STRING,
    DECLARATION_BOOL,
    DECLARATION_DOUBLE,
    DECLARATION_LONG,
    DECLARATION_VOID,
    DECLARATION_BYTE,

    LITERAL,

    REFERENCE,

    ADDRESS_OF,

    PUSH_SCOPE,
    POP_SCOPE,

    CALL,
    FUNC_DECLARARION,
    FUNC_BODY,

    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,

    FLOW_IF,
    FLOW_ELSE,
    FLOW_FOR,
    FLOW_WHILE,

    NO_OP,

    SYSTEM,
    SYSTEM_LOAD,

} OpCodes;


OpType operatorType(OpCodes op);


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode);

