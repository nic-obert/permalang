#include "pch.hh"


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode)
{
    switch (opCode)
    {
    case OpCodes::LOGICAL_AND:
        return stream << "LOGICAL AND";
    case OpCodes::LOGICAL_OR:
        return stream << "LOGICAL OR";  
    case OpCodes::LOGICAL_EQ:
        return stream << "LOGICAL EQ";
    case OpCodes::LOGICAL_NOT:
        return stream << "LOGICAL NOT";
    case OpCodes::LOGICAL_GREATER:
        return stream << "LOGICAL GREATER";
    case OpCodes::LOGICAL_GREATER_EQ:
        return stream << "LOGICAL GRATER EQ";
    case OpCodes::LOGICAL_LESS:
        return stream << "LOGICAL LESS";
    case OpCodes::LOGICAL_LESS_EQ:
        return stream << "LOGICAL LESS EQUAL";
    case OpCodes::LOGICAL_NOT_EQ:
        return stream << "LOGICAL NOT EQ";

    case OpCodes::ARITHMETICAL_SUM:
        return stream << "ARITHMETICAL SUM";
    case OpCodes::ARITHMETICAL_SUB:
        return stream << "ARITHMETICAL SUB";
    case OpCodes::ARITHMETICAL_MUL:
        return stream << "ARITHMETICAL MUL";
    case OpCodes::ARITHMETICAL_DIV:
        return stream << "ARITHMETICAL DIV";
    case OpCodes::ARITHMETICAL_POW:
        return stream << "ARITHMETICAL POW";
    case OpCodes::ARITHMETICAL_INC:
        return stream << "ARITHMETICAL INC";
    case OpCodes::ARITHMETICAL_DEC:
        return stream << "ARITHMETICAL DEC";
    case OpCodes::ARITHMETICAL_MOD:
        return stream << "ARITHMETICAL MOD";

    case OpCodes::ASSIGNMENT_ASSIGN:
        return stream << "ASSIGNMENT ASSIGN";
    case OpCodes::ASSIGNMENT_ADD:
        return stream << "ASSIGNMENT ADD";
    case OpCodes::ASSIGNMENT_SUB:
        return stream << "ASSIGNMENT SUB";
    case OpCodes::ASSIGNMENT_MUL:
        return stream << "ASSIGNMENT MUL";
    case OpCodes::ASSIGNMENT_DIV:
        return stream << "ASSIGNMENT DIV";
    case OpCodes::ASSIGNMENT_POW:
        return stream << "ASSIGNMENT POW";

    case OpCodes::DECLARATION_INT:
        return stream << "DECLARATION INT";
    case OpCodes::DECLARATION_FLOAT:
        return stream << "DECLARATION FLOAT";
    case OpCodes::DECLARATION_STRING:
        return stream << "DECLARATION STRING";
    case OpCodes::DECLARATION_BOOL:
        return stream << "DECLARATION BOOL";

    case OpCodes::REFERENCE:
        return stream << "REFERENCE";
    
    case OpCodes::LITERAL:
        return stream << "LITERAL";
    
    case OpCodes::PUSH_SCOPE:
        return stream << "PUSH SCOPE";
    case OpCodes::POP_SCOPE:
        return stream << "POP SCOPE";

    case OpCodes::PARENTHESIS:
        return stream << "PARENTHESIS";

    case OpCodes::FLOW_IF:
        return stream << "FLOW IF";
    case OpCodes::FLOW_ELSE:
        return stream << "FLOW ELSE";
    case OpCodes::FLOW_WHILE:
        return stream << "FLOW WHILE";
    case OpCodes::FLOW_FOR:
        return stream << "FLOW FOR";
    
    
    default:
        return stream << "UNDEFINED OPCODE";

    }    
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


