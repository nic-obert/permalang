#include "token.hh"
#include "keywords.hh"
#include "operators.hh"
#include "syntax_tree.hh"


#define RIGHT 0
#define LEFT 1

using namespace Tokens;



void binarySatisfy(Token* token, TokenType leftType, TokenType rightType, syntax_tree::Statement* statement)
{

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
        std::cerr << "Token " << *token << " requires token of type " << leftType << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
    }
    else if (token->next->type != rightType)
    {
        std::cerr << "Token " << *token << " requires token of type " << rightType << " to the right, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }


    token->value = (Value) new Token*[2] {token->prev, token->next};

    token->prev->parent = token;
    token->next->parent = token;

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

    if (token->prev != nullptr)
    {
        token->prev->next = token;
    }
    if (token->next != nullptr)
    {
        token->next->prev = token;
    }


}


void unarySatisfy(Token* token, TokenType type, char side, syntax_tree::Statement* statement)
{

    if (side == LEFT)
    {
        if (token->prev == nullptr)
        {
            std::cerr << "Missing token of type " << type << " to the left of " << *token << std::endl;
            exit(1);
        }
        if (token->prev->type != type)
        {
            std::cerr << "Token " << *token << " requires token of type " << token << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
        }

        token->value = (Value) token->prev;

        token->prev->parent = token;

        statement->remove(token->prev);
        
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
            std::cerr << "Token " << *token << " requires token of type " << type << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
        }

        token->value = (Value) token->next;

        token->next->parent = token;

        statement->remove(token->next);

        if (token->next != nullptr)
        {
            token->next->prev = token;
        }

    }

}



void declarationSatisfy(Token* token, TokenType type, syntax_tree::Statement* statement)
{

    if (token->next == nullptr)
    {
        std::cerr << "Missing token of operator type " << REFERENCE << " to the right of " << *token << std::endl;
        exit(1);
    }
    if (token->next->operatorType != REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << REFERENCE << " to the right, but " << *token->next << " was provided" << std::endl;
        exit(1);
    }

    token->value = (Value) token->next;
    token->type = type;

    token->next->parent = token;

    statement->remove(token->next);

    if (token->next != nullptr)
    {
        token->next->prev = token;
    }

    token->operatorType = REFERENCE;

}


void assignSatisfy(Token* token, syntax_tree::Statement* statement)
{
    if (token->prev == nullptr)
    {
        std::cerr << "Missing token of operator type " << REFERENCE << " to the left of " << *token << std::endl;
        exit(1);
    }    
    else if (token->next == nullptr)
    {
        std::cerr << "Missing token of operator type " << REFERENCE << " or " << LITERAL << " to the right of " << *token << std::endl;
        exit(1);
    }

    if (token->prev->operatorType != REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << REFERENCE << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
    }
    else if (token->next->type != token->prev->type)
    {
        std::cerr << "Token " << *token << " requires token of type " << token->prev->type << " to the right, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }


    token->value = (Value) new Token*[2] {token->prev, token->next};
    token->type = token->prev->type;

    token->prev->parent = token;
    token->next->parent = token;

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

    if (token->prev != nullptr)
    {
        token->prev->next = token;
    }
    if (token->next != nullptr)
    {
        token->next->prev = token;
    }

    token->operatorType = REFERENCE;

}


void incDecSatisfy(Token* token, syntax_tree::Statement* statement)
{
    if (token->prev == nullptr)
    {
        std::cerr << "Missing token of operator type " << REFERENCE << " to the left of " << *token << std::endl;
        exit(1);
    }
    if (token->next->operatorType != REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << REFERENCE << " to the left, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }

    token->value = (Value) token->next;
    token->type = token->next->type;

    token->next->parent = token;

    statement->remove(token->next);

    if (token->next != nullptr)
    {
        token->next->prev = token;
    }

    token->operatorType = REFERENCE;
}



void syntax_tree::Statement::satisfy(Token* token)
{
    // set token's priority to 0 right away so that it doesn't get evaluated multiple times
    token->priority = 0;

    switch (token->type)
    {
    case ARITHMETIC_OP:
    {
        using namespace operators::arithmetical;

        switch ((ArithmeticalOperators) token->value)
        {
        case POWER:
        case DIVISION:
        case MULTIPLICATION:
        case SUBTRACTION:
        case SUM:
            binarySatisfy(token, INT, INT, this);
            token->operatorType = LITERAL;
            break;
        
        case INCREMENT:
        case DECREMENT:
            incDecSatisfy(token, this);
            break;
        
        }
        break;
    }
    
    case LOGICAL_OP:
    {
        using namespace operators::logical;

        switch ((LogicalOperators) token->value)
        {
        case EQUALITY:
        case INEQUALITY:
        case AND:
        case OR:
        case GREATER_THAN:
        case GREATER_EQUAL:
        case LESS_THAN:
        case LESS_EQUAL:
            binarySatisfy(token, BOOL, BOOL, this);
            token->operatorType = LITERAL;
            break;

        case NOT:
            unarySatisfy(token, BOOL, RIGHT, this);
            token->operatorType = LITERAL;
            break;
        }

        break;
    }

    case ASSIGNMENT_OP: 
    {
        using namespace operators::assignment;

        switch ((AssignmentOperators) token->value)
        {
        case ASSIGNMENT:
        case ADD:
        case SUBTRACT:
        case DIVIDE:
        case ELEVATE:
        case MULTIPLY:
            assignSatisfy(token, this);
            break;
        }

        break;
    }

    case KEYWORD:
    {

        switch ((Keywords::Keywords) token->value)
        {
        case Keywords::Keywords::INT:
            declarationSatisfy(token, TokenType::INT, this);
            break;
        case Keywords::Keywords::STRING:
            declarationSatisfy(token, TokenType::STRING, this);
            break;
        case Keywords::Keywords::FLOAT:
            declarationSatisfy(token, TokenType::FLOAT, this);
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

