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
    
    "LITERAL",

    "REFERENCE",
    
    "ADDRESS OF",

    "PUSH SCOPE",
    "POP SCOPE",

    "CALL",

    "PARENTHESIS",

    "FLOW IF",
    "FLOW ELSE",
    "FLOW FOR",
    "FLOW WHILE",
    
    "NO OP"
};


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode)
{
    return stream << opCodeRepr[(unsigned char) opCode];
}


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
    case OpCodes::PARENTHESIS:
    case OpCodes::PUSH_SCOPE:
        return OpType::UNARY;
    
    default:
        return OpType::STANDALONE;
    
    }
}


