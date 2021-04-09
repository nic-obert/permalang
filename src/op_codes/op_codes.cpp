#include "op_codes.hh"


std::ostream& operator<<(std::ostream& stream, OpCodes const& opCode)
{
    switch (opCode)
    {
    case LOGICAL_AND:
        return stream << "LOGICAL AND";
    case LOGICAL_OR:
        return stream << "LOGICAL OR";  
    case LOGICAL_EQ:
        return stream << "LOGICAL EQ";
    case LOGICAL_NOT:
        return stream << "LOGICAL NOT";
    case LOGICAL_GREATER:
        return stream << "LOGICAL GREATER";
    case LOGICAL_GREATER_EQ:
        return stream << "LOGICAL GRATER EQ";
    case LOGICAL_LESS:
        return stream << "LOGICAL LESS";
    case LOGICAL_LESS_EQ:
        return stream << "LOGICAL LESS EQUAL";
    case LOGICAL_NOT_EQ:
        return stream << "LOGICAL NOT EQ";

    case ARITHMETICAL_SUM:
        return stream << "ARITHMETICAL SUM";
    case ARITHMETICAL_SUB:
        return stream << "ARITHMETICAL SUB";
    case ARITHMETICAL_MUL:
        return stream << "ARITHMETICAL MUL";
    case ARITHMETICAL_DIV:
        return stream << "ARITHMETICAL DIV";
    case ARITHMETICAL_POW:
        return stream << "ARITHMETICAL POW";
    case ARITHMETICAL_INC:
        return stream << "ARITHMETICAL INC";
    case ARITHMETICAL_DEC:
        return stream << "ARITHMETICAL DEC";
    case ARITHMETICAL_MOD:
        return stream << "ARITHMETICAL MOD";

    case ASSIGNMENT_ASSIGN:
        return stream << "ASSIGNMENT ASSIGN";
    case ASSIGNMENT_ADD:
        return stream << "ASSIGNMENT ADD";
    case ASSIGNMENT_SUB:
        return stream << "ASSIGNMENT SUB";
    case ASSIGNMENT_MUL:
        return stream << "ASSIGNMENT MUL";
    case ASSIGNMENT_DIV:
        return stream << "ASSIGNMENT DIV";
    case ASSIGNMENT_POW:
        return stream << "ASSIGNMENT POW";

    case DECLARATION_INT:
        return stream << "DECLARATION INT";
    case DECLARATION_FLOAT:
        return stream << "DECLARATION FLOAT";
    case DECLARATION_STRING:
        return stream << "DECLARATION STRING";
    case DECLARATION_BOOL:
        return stream << "DECLARATION BOOL";

    case REFERENCE:
        return stream << "REFERENCE";
    
    case LITERAL:
        return stream << "LITERAL";

    case FLOW_IF:
        return stream << "FLOW IF";
    case FLOW_ELSE:
        return stream << "FLOW ELSE";
    case FLOW_WHILE:
        return stream << "FLOW WHILE";
    case FLOW_FOR:
        return stream << "FLOW FOR";

    }

    return stream << "UNDEFINED OPCODE";
}


OpType operatorType(OpCodes op)
{
    switch (op)
    {
    case LOGICAL_AND:
    case LOGICAL_NOT_EQ:
    case LOGICAL_EQ:
    case LOGICAL_GREATER:
    case LOGICAL_GREATER_EQ:
    case LOGICAL_LESS:
    case LOGICAL_LESS_EQ:
    case ARITHMETICAL_SUM:
    case ARITHMETICAL_MUL:
    case ARITHMETICAL_DIV:
    case ARITHMETICAL_SUB:
    case ARITHMETICAL_MOD:
    case ARITHMETICAL_POW:
    case ASSIGNMENT_ADD:
    case ASSIGNMENT_DIV:
    case ASSIGNMENT_ASSIGN:
    case ASSIGNMENT_MUL:
    case ASSIGNMENT_POW:
    case ASSIGNMENT_SUB:
        return OpType::BINARY;
    
    case LOGICAL_NOT:
    case ARITHMETICAL_DEC:
    case ARITHMETICAL_INC:
    case DECLARATION_BOOL:
    case DECLARATION_FLOAT:
    case DECLARATION_INT:
    case DECLARATION_STRING:
        return OpType::UNARY;
    
    default:
        return OpType::STANDALONE;
    
    }
}


