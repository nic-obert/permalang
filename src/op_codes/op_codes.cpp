#include "op_codes.hh"


// lookup table for OpCodes
static const char* const opCodeRepr[] = 
{
    "LOGICAL AND",
    "LOGICAL OR",
    "LOGICAL EQ",
    "LOGICAL NOT EQ",
    "LOGICAL GREATER",
    "LOGICAL LESS",
    "LOGICAL LESS EQ",
    "LOGICAL GREATER EQ",
    "LOGICAL NOT",

    "ARITHMETICAL SUM",
    "ARITHMETICAL SUB",
    "ARITHMETICAL MUL",
    "ARITHMETICAL DIV",
    "ARITHMETICAL POW",
    "ARITHMETICAL MOD",
    "ARITHMETICAL INC",
    "ARITHMETICAL DEC",

    "ASSIGNEMNT ASSIGN",
    "ASSIGNMENT ADD",
    "ASSIGNMENT SUB",
    "ASSIGNMENT MUL",
    "ASSIGNMENT DIV",
    "ASSIGNEMNT POW",

    "DECLARATION INT",
    "DECLARATION FLOAT",
    "DECLARATION STRING",
    "DECLARATION BOOL",
    "DECLARATION DOUBLE",
    "DECLARATION LONG",
    "DECLARATION VOID",
    "DECLARATION BYTE",
    
    "LITERAL",

    "REFERENCE",
    
    "ADDRESS OF",

    "PUSH SCOPE",
    "POP SCOPE",

    "CALL",
    "FUNC_DECLARATION",
    "FUNC_BODY",

    "OPEN PARENTHESIS",
    "CLOSE PARENTHESIS"

    "FLOW IF",
    "FLOW ELSE",
    "FLOW FOR",
    "FLOW WHILE",
    
    "NO OP",

    "SYSTEM",
    "SYSTEM LOAD",

    "BREAK",
    "CONTINUE"
};


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode)
{
    return stream << opCodeRepr[(unsigned char) opCode];
}


// operator type in terms of operand number
// (STANDALONE, UNARY, BINARY)
OpType operatorType(OpCodes op)
{
    switch (op)
    {
    case OpCodes::LOGICAL_AND:
    case OpCodes::LOGICAL_NOT_EQ:
    case OpCodes::LOGICAL_EQ:
    case OpCodes::LOGICAL_GREATER:
    case OpCodes::LOGICAL_GREATER_EQ:
    case OpCodes::LOGICAL_LESS:
    case OpCodes::LOGICAL_LESS_EQ:
    case OpCodes::ARITHMETICAL_SUM:
    case OpCodes::ARITHMETICAL_MUL:
    case OpCodes::ARITHMETICAL_DIV:
    case OpCodes::ARITHMETICAL_SUB:
    case OpCodes::ARITHMETICAL_MOD:
    case OpCodes::ARITHMETICAL_POW:
    case OpCodes::ASSIGNMENT_ADD:
    case OpCodes::ASSIGNMENT_DIV:
    case OpCodes::ASSIGNMENT_ASSIGN:
    case OpCodes::ASSIGNMENT_MUL:
    case OpCodes::ASSIGNMENT_POW:
    case OpCodes::ASSIGNMENT_SUB:
    case OpCodes::FLOW_IF:
        return OpType::BINARY;
    
    case OpCodes::LOGICAL_NOT:
    case OpCodes::ARITHMETICAL_DEC:
    case OpCodes::ARITHMETICAL_INC:
    case OpCodes::DECLARATION_BOOL:
    case OpCodes::DECLARATION_FLOAT:
    case OpCodes::DECLARATION_INT:
    case OpCodes::DECLARATION_STRING:
    case OpCodes::DECLARATION_DOUBLE:
    case OpCodes::DECLARATION_LONG:
    case OpCodes::DECLARATION_BYTE:
    case OpCodes::DECLARATION_VOID:
    case OpCodes::OPEN_PARENTHESIS:
    case OpCodes::PUSH_SCOPE:
    case OpCodes::SYSTEM:
    case OpCodes::SYSTEM_LOAD:
    case OpCodes::FUNC_BODY:
        return OpType::UNARY;
    
    default:
        return OpType::STANDALONE;
    
    }
}


