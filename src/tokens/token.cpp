#include "token.hh"
#include "keywords.hh"


using namespace Tokens;


Token::Token(TokenType type, size_t priority, OpCodes opCode, Value value) 
: type(type), priority(priority), value(value), opCode(opCode)
{
    
}


Token::Token(TokenType type, size_t priority, OpCodes opCode) 
: type(type), priority(priority), value(0), opCode(opCode)
{
    
}


Token::~Token()
{
    if (type == TokenType::TEXT || opCode == OpCodes::REFERENCE)
    {
        // text or reference tokens have a string as their value
        delete (std::string*) value;
    }
  
}


std::ostream& operator<<(std::ostream& stream, Token const& token)
{   

    switch (token.opCode)
    {

    case OpCodes::LITERAL:
    {
        switch (token.type)
        {
        case TokenType::TEXT:
        case TokenType::STRING: 
        {
            stream << '<' << token.type << ": " << *((std::string*) token.value)
                << " (" << token.priority << ")>";
            return stream;
        }

        case TokenType::BOOL:
        {
            stream << '<' << token.type << ": " << ((token.value == 0) ? "false" : "true")
                << " (" << token.priority << ")>";
            return stream;
        }

        default:
            return stream << '<' << token.type << ": " << token.value << " ("
                << token.priority << ")>";

        } // switch (token.type)

        break;
    } // case LITERAL


    case OpCodes::CALL:
    {
        return stream << "<CALL: (" << token.priority << ")>";
    } // case CALL


    case OpCodes::FUNC_DECLARARION:
    {
        return stream << "<FUNCTION: ( (" << token.priority << ")>";
    }
    

    case OpCodes::REFERENCE:
    {

        switch (token.type)
        {
        case TokenType::INT:
        case TokenType::STRING:
        case TokenType::BOOL:
        case TokenType::FLOAT:
        case TokenType::DOUBLE:
        case TokenType::LONG:
        case TokenType::NUMERIC:
        case TokenType::BYTE:
            stream << '<' << token.type << "*: " << *(std::string*) token.value
                << " (" << token.priority << ")>";
            return stream;
        
        case TokenType::TEXT:
            stream << '<' << TokenType::NONE << "*: " << *(std::string*) token.value
                << " (" << token.priority << ")>";
            return stream;
        }

        break;
    } // case REFERENCE

    // arithmetical operators

    case OpCodes::ARITHMETICAL_SUM:
        return stream << "< + (" << token.priority << ")>";

    case OpCodes::ARITHMETICAL_SUB:
        return stream << "< - (" << token.priority << ")>";
    
    case OpCodes::ARITHMETICAL_MUL:
        return stream << "< * (" << token.priority << ")>";

    case OpCodes::ARITHMETICAL_DIV:
        return stream << "< / (" << token.priority << ")>";

    case OpCodes::ARITHMETICAL_POW:
        return stream << "< ^ (" << token.priority << ")>";
    
    case OpCodes::ARITHMETICAL_INC:
        return stream << "< ++ (" << token.priority << ")>";

    case OpCodes::ARITHMETICAL_DEC:
        return stream << "<-- (" << token.priority << ")>";

    // logical operators

    case OpCodes::LOGICAL_EQ:
        return stream << "< == (" << token.priority << ")>";
    
    case OpCodes::LOGICAL_NOT_EQ:
        return stream << "< != (" << token.priority << ")>";
    
    case OpCodes::LOGICAL_AND:
        return stream << "< && (" << token.priority << ")>";
    
    case OpCodes::LOGICAL_OR:
        return stream << "< || (" << token.priority << ")>";

    case OpCodes::LOGICAL_GREATER:
        return stream << "< > (" << token.priority << ")>";
    
    case OpCodes::LOGICAL_LESS:
        return stream << "< < (" << token.priority << ")>";

    case OpCodes::LOGICAL_GREATER_EQ:
        return stream << "< >= (" << token.priority << ")>";
    
    case OpCodes::LOGICAL_LESS_EQ:
        return stream << "< <= (" << token.priority << ")>";
    
    case OpCodes::LOGICAL_NOT:
        return stream << "< ! (" << token.priority << ")>";

    // assignement operators

    case OpCodes::ASSIGNMENT_ASSIGN:
        return stream << "< = (" << token.priority << ")>";
    
    case OpCodes::ASSIGNMENT_ADD:
        return stream << "< += (" << token.priority << ")>";

    case OpCodes::ASSIGNMENT_SUB:
        return stream << "< -= (" << token.priority << ")>";

    case OpCodes::ASSIGNMENT_MUL:
        return stream << "< *= (" << token.priority << ")>";

    case OpCodes::ASSIGNMENT_DIV:
        return stream << "< /= (" << token.priority << ")>";
    
    case OpCodes::ASSIGNMENT_POW:
        return stream << "< ^= (" << token.priority << ")>";


    case OpCodes::ADDRESS_OF:
        return stream << "<ADDRESS OF: & (" << token.priority << ")>";

    case OpCodes::PUSH_SCOPE:
    case OpCodes::FUNC_BODY:
        return stream << "< { (" << token.priority << ")>";

    case OpCodes::POP_SCOPE:
        return stream << "< } (" << token.priority << ")>";
    

    case OpCodes::OPEN_PARENTHESIS:
        return stream << "< ( (" << token.priority << ")>";

    case OpCodes::CLOSE_PARENTHESIS:
        return stream << "< ) (" << token.priority << ")>";


    default:
    {
        
        switch (token.type)
        {
        case TokenType::KEYWORD:
            return stream << '<' << TokenType::KEYWORD << ": "
                << keywords::keywordName(token.opCode) << " (" << token.priority << ")>";
    
        case TokenType::ENDS:
        case TokenType::COMMA:
            return stream;
        
        default:
            return stream << '<' << token.type << ": " << token.value << " (" << token.priority << ")>";

        } // switch (token.type)

    } // default

    } // switch (token.opCode)

    // if token hasn't been already handled, throw error
    std::cerr << "Undefined Token: <type=" << token.type << ", value=" << token.value
        << ", opCode=" << token.opCode << ", priority=" << token.priority << '>' << std::endl;
    exit(EXIT_FAILURE);
}


void Token::removeToken(Token* token)
{
    if (token->prev != nullptr)
    {
        token->prev->next = token->next;
    }

    if (token->next != nullptr)
    {
        token->next->prev = token->prev;
    }
}


void Tokens::copyRelevantData(Token* dest, const Token* src)
{
    dest->opCode = src->opCode;
    dest->type = src->type;
    dest->value = src->value;
}

