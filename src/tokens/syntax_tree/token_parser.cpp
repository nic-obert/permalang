#include "token.hh"
#include "keywords.hh"
#include "operators.hh"


#define RIGHT 0
#define LEFT 1

using namespace Tokens;



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
            operatorType = LITERAL;
            break;
        
        case INCREMENT:
        case DECREMENT:
            unarySatisfy(this, NUMBER, LEFT);
            operatorType = REFERENCE;
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
            operatorType = LITERAL;
            break;

        case NOT:
            unarySatisfy(this, BOOL, RIGHT);
            operatorType = LITERAL;
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
            operatorType = REFERENCE;
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
            operatorType = REFERENCE;
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

