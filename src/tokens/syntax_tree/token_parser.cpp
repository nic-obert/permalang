#include "token.hh"
#include "keywords.hh"
#include "operators.hh"


#define RIGHT 0
#define LEFT 1

using namespace Tokens;


OperatorType syntaxTypeOfToken(Token* token)
{
    switch (token->type)
    {
    // operators
    case Tokens::KEYWORD: 
    {
        switch ((Keywords::Keywords) token->value)
        {
        case Keywords::Keywords::IF:
            return OperatorType::BINARY;

        case Keywords::Keywords::ELSE:
        case Keywords::Keywords::BOOL:
        case Keywords::Keywords::FLOAT:
        case Keywords::Keywords::INT:
        case Keywords::Keywords::STRING:
            return OperatorType::UNARY;
        }
    }

    case Tokens::ARITHMETIC_OP: 
    {
        using namespace operators::arithmetical;

        switch ((ArithmeticalOperators) token->value)
        {
        case ArithmeticalOperators::SUM:
        case ArithmeticalOperators::DIVISION:
        case ArithmeticalOperators::MULTIPLICATION:
        case ArithmeticalOperators::SUBTRACTION:
        case ArithmeticalOperators::POWER:
            return OperatorType::UNARY;
        
        case ArithmeticalOperators::INCREMENT:
        case ArithmeticalOperators::DECREMENT:
            return OperatorType::UNARY;
        }
    }

    case Tokens::LOGICAL_OP: 
    {
        using namespace operators::logical;

        switch ((LogicalOperators) token->value)
        {
        case LogicalOperators::AND:
        case LogicalOperators::OR:
        case LogicalOperators::EQUALITY:
        case LogicalOperators::GREATER_EQUAL:
        case LogicalOperators::INEQUALITY:
        case LogicalOperators::LESS_EQUAL:
        case LogicalOperators::GREATER_THAN:
        case LogicalOperators::LESS_THAN:
            return OperatorType::BINARY;
        
        case LogicalOperators::NOT:
            return OperatorType::UNARY;
        }
    }

    case Tokens::ASSIGNMENT_OP:
    {
        return OperatorType::BINARY;
    }
    
    // literals
    case Tokens::BOOL:
    case Tokens::NUMBER:
    case Tokens::STRING:
        return OperatorType::LITERAL;
    
    // references
    case Tokens::TEXT:
        return OperatorType::REFERENCE;
        
    }
}



void binarySatisfy(Token* token, TokenType leftType, TokenType rightType)
{
    using namespace Tokens;

    if (token->prev == nullptr)
    {
        std::cerr << "Missing token of type " << leftType << " to the left of " << *token << std::endl;
        exit(1);
    }    
    else if (token->next == nullptr)
    {
        std::cerr << "Missing token of type " << rightType << " to the right of " << *token << std::endl;
        exit(1);
    }

    if (token->prev->type != leftType)
    {
        std::cerr << "Token " << *token << " requires token of type " << leftType << " to the left, but " << token->prev->type << " was provided" << std::endl;
            exit(1);
    }
    else if (token->next->type != rightType)
    {
        std::cerr << "Token " << *token << " requires token of type " << rightType << " to the right, but " << token->prev->type << " was provided" << std::endl;
        exit(1);
    }


    token->value = (Value) new Token*[2] {token->prev, token->next};

    token->prev->parent = token;
    token->next->parent = token;

    // remove nodes to the left and right if node
    token->prev = token->prev->prev;
    token->next = token->next->next;

    if (token->prev != nullptr)
    {
        token->prev->next = token;
    }
    if (token->next != nullptr)
    {
        token->next->prev = token;
    }


}


void unarySatisfy(Token* token, TokenType type, char side)
{

    using namespace Tokens;

    if (side == LEFT)
    {
        if (token->prev == nullptr)
        {
            std::cerr << "Missing token of type " << type << " to the left of " << *token << std::endl;
            exit(1);
        }
        if (token->prev->type != type)
        {
            std::cerr << "Token " << *token << " requires token of type " << token << " to the left, but " << token->prev->type << " was provided" << std::endl;
            exit(1);
        }

        token->value = (Value) token->prev;

        token->prev->parent = token;

        token->prev = token->prev->prev;
        
        if (token->prev != nullptr)
        {
            token->prev->next = token;
        }

    } 
    else 
    {
        if (token->next == nullptr)
        {
            std::cerr << "Missing token of type " << type << " to the right of " << *token << std::endl;
            exit(1);
        }
        if (token->next->type != type)
        {
            std::cerr << "Token " << *token << " requires token of type " << type << " to the left, but " << token->prev->type << " was provided" << std::endl;
            exit(1);
        }

        token->value = (Value) token->next;

        token->next->parent = token;

        token->next = token->next->next;

        if (token->next != nullptr)
        {
            token->next->prev = token;
        }

    }

}


void Token::satisfy()
{
    // set token's priority to 0 right away so that it doesn't get evaluated multiple times
    priority = 0;

    switch (type)
    {
    case ARITHMETIC_OP:
    {
        using namespace operators::arithmetical;

        switch ((ArithmeticalOperators) value)
        {
        case POWER:
        case DIVISION:
        case MULTIPLICATION:
        case SUBTRACTION:
        case SUM:
            binarySatisfy(this, NUMBER, NUMBER);
            break;
        
        case INCREMENT:
        case DECREMENT:
            unarySatisfy(this, NUMBER, LEFT);
            break;
        
        }
        break;
    }
    
    case LOGICAL_OP:
    {
        using namespace operators::logical;

        switch ((LogicalOperators) value)
        {
        case EQUALITY:
        case INEQUALITY:
        case AND:
        case OR:
        case GREATER_THAN:
        case GREATER_EQUAL:
        case LESS_THAN:
        case LESS_EQUAL:
            binarySatisfy(this, BOOL, BOOL);
            break;

        case NOT:
            unarySatisfy(this, BOOL, RIGHT);
            break;
        }

        break;
    }

    case ASSIGNMENT_OP: 
    {
        using namespace operators::assignment;

        switch ((AssignmentOperators) value)
        {
        case ASSIGNMENT:
        case ADD:
        case SUBTRACT:
        case DIVIDE:
        case ELEVATE:
        case MULTIPLY:
            binarySatisfy(this, TEXT, NUMBER);
            break;
        }

        break;
    }

    case KEYWORD:
    {
        using namespace Keywords;

        switch ((Keywords::Keywords) value)
        {
        case Keywords::Keywords::INT:
        case Keywords::Keywords::STRING:
        case Keywords::Keywords::FLOAT:
            unarySatisfy(this, TEXT, RIGHT);
            break;
        
        case Keywords::Keywords::IF:
        case Keywords::Keywords::ELSE:
            std::cerr << "not implemented" << std::endl;
            exit(1);
            break;
        }

        break;
    }

    }

}

