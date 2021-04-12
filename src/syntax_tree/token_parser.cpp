#include "token.hh"
#include "syntax_tree.hh"
#include "symbol_table.hh"
#include "op_codes.hh"
#include "utils.hh"

#include <string>


#define RIGHT 0
#define LEFT 1
#define DELETE true

using namespace Tokens;


// returns the TokenType of a given token, throws exception if token is not declared
TokenType tokenTypeOf(const Token* token)
{
    if (token->opCode == OpCodes::REFERENCE)
    {
        return symbol_table::SymbolTable::get((std::string*) token->value)->type;
    }
    
    return token->type;
}


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

    if (tokenTypeOf(token->prev) != leftType)
    {
        std::cerr << "Token " << *token << " requires token of type " << leftType << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
    }
    else if (tokenTypeOf(token->next) != rightType)
    {
        std::cerr << "Token " << *token << " requires token of type " << rightType << " to the right, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }

    // pointer to array of token pointers
    token->value = toValue((new Token*[2] {token->prev, token->next}));

    token->type = tokenTypeOf(token->prev);

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

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
        if (tokenTypeOf(token->prev) != type)
        {
            std::cerr << "Token " << *token << " requires token of type " << token << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
        }

        token->value = toValue(new Token*[1] {token->prev});
        token->type = tokenTypeOf(token->prev);

        statement->remove(token->prev);

    } 
    else 
    {
        if (token->next == nullptr)
        {
            std::cerr << "Missing token of type " << type << " to the right of " << *token << std::endl;
            exit(1);
        }
        if (tokenTypeOf(token->next) != type)
        {
            std::cerr << "Token " << *token << " requires token of type " << type << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
        }

        token->value = toValue(new Token*[1] {token->next});
        token->type = tokenTypeOf(token->next);

        statement->remove(token->next);

    }

}



void declarationSatisfy(Token* token, TokenType type, syntax_tree::Statement* statement)
{

    if (token->next == nullptr)
    {
        std::cerr << "Missing token of operator type " << REFERENCE << " to the right of " << *token << std::endl;
        exit(1);
    }
    if (token->next->opCode != REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << REFERENCE << " to the right, but " << *token->next << " was provided" << std::endl;
        exit(1);
    }

    token->value = token->next->value;
    token->type = type;

    statement->remove(token->next, DELETE);

    token->opCode = REFERENCE;

    symbol_table::SymbolTable::declare(
        (std::string*) token->value,
        new symbol_table::Symbol(0, token->type)
    );

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

    if (token->prev->opCode != REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << REFERENCE << " to the left, but " << *token->prev << " was provided" << std::endl;
            exit(1);
    }
    else if (tokenTypeOf(token->next) != tokenTypeOf(token->prev))
    {
        std::cerr << "Token " << *token << " requires token of type " << tokenTypeOf(token->prev) << " to the right, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }


    token->value = toValue((new Token*[2] {token->prev, token->next}));

    // update symbol table
    symbol_table::SymbolTable::assign(
        (std::string*) token->prev->value,
        new symbol_table::Symbol(token->next->value, token->prev->type)
    );

    token->type = tokenTypeOf(token->prev);

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

}


void incDecSatisfy(Token* token, syntax_tree::Statement* statement)
{
    if (token->prev == nullptr)
    {
        std::cerr << "Missing token of operator type " << REFERENCE << " to the left of " << *token << std::endl;
        exit(1);
    }
    if (token->prev->opCode != REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << REFERENCE << " to the left, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }


    token->value = toValue(new Token*[1] {token->prev});
    token->type = tokenTypeOf(token->prev);

    statement->remove(token->prev);

}



void syntax_tree::Statement::satisfy(Token* token)
{
    // set token's priority to 0 right away so that it doesn't get evaluated multiple times
    token->priority = 0;

    switch (token->opCode)
    {
    case ARITHMETICAL_SUM:
    case ARITHMETICAL_SUB:
    case ARITHMETICAL_MUL:
    case ARITHMETICAL_DIV:
    case ARITHMETICAL_POW:
    {
        binarySatisfy(token, INT, INT, this);
        break;
    }

    case ARITHMETICAL_INC:
    case ARITHMETICAL_DEC:
    {
        incDecSatisfy(token, this);
        break;
    }

    case LOGICAL_EQ:
    case LOGICAL_NOT_EQ:
    case LOGICAL_AND:
    case LOGICAL_OR:
    case LOGICAL_LESS:
    case LOGICAL_LESS_EQ:
    case LOGICAL_GREATER:
    case LOGICAL_GREATER_EQ:
    {
        binarySatisfy(token, BOOL, BOOL, this);
        break;
    }
    
    case LOGICAL_NOT:
    {
        unarySatisfy(token, BOOL, RIGHT, this);
        break;
    }


    case ASSIGNMENT_ASSIGN:
    case ASSIGNMENT_ADD:
    case ASSIGNMENT_SUB:
    case ASSIGNMENT_DIV:
    case ASSIGNMENT_POW:
    case ASSIGNMENT_MUL:
    {
        assignSatisfy(token, this);
        break;
    }


    case DECLARATION_INT:
        declarationSatisfy(token, TokenType::INT, this);
        break;
    case DECLARATION_STRING:
        declarationSatisfy(token, TokenType::STRING, this);
        break;
    case DECLARATION_FLOAT:
        declarationSatisfy(token, TokenType::FLOAT, this);
        break;
    case DECLARATION_BOOL:
        declarationSatisfy(token, TokenType::BOOL, this);
        break;
    
    case FLOW_IF:
    case FLOW_ELSE:
    case FLOW_WHILE:
    case FLOW_FOR:
    {
        std::cerr << "not implemented" << std::endl;
        exit(1);
        break;
    }

    } // switch (token->opCode)

}

