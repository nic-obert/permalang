#include "token.hh"
#include "syntax_tree.hh"
#include "symbol_table.hh"
#include "op_codes.hh"
#include "utils.hh"

#include <string>
#include <iostream>


#define RIGHT 0
#define LEFT 1
#define DELETE true

using namespace Tokens;


/*
 - returns the TokenType of a given token
 - returns the TokenType of it's value if token is a variable
 - throws exception if token is not declared
*/
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
        std::cerr << "Token " << *token << " requires token of type " << rightType << " to the right, but " << *token->next << " was provided" << std::endl;
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
            std::cerr << "Token " << *token << " requires token of type " << type << " to the right, but " << *token->next << " was provided" << std::endl;
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
        std::cerr << "Missing token of operator type " << OpCodes::REFERENCE << " to the right of " << *token << std::endl;
        exit(1);
    }
    if (token->next->opCode != OpCodes::REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << OpCodes::REFERENCE << " to the right, but " << *token->next << " was provided" << std::endl;
        exit(1);
    }

    token->value = token->next->value;
    token->type = type;

    statement->remove(token->next, DELETE);

    token->opCode = OpCodes::REFERENCE;

    symbol_table::SymbolTable::declare(
        (std::string*) token->value,
        new symbol_table::Symbol(0, token->type)
    );

}


void assignSatisfy(Token* token, syntax_tree::Statement* statement)
{

    if (token->prev == nullptr)
    {
        std::cerr << "Missing token of operator type " << OpCodes::REFERENCE << " to the left of " << *token << std::endl;
        exit(1);
    }    
    else if (token->next == nullptr)
    {
        std::cerr << "Missing token of operator type " << OpCodes::REFERENCE << " or " << OpCodes::LITERAL << " to the right of " << *token << std::endl;
        exit(1);
    }

    if (token->prev->opCode != OpCodes::REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << OpCodes::REFERENCE << " to the left, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }
    else if (tokenTypeOf(token->next) != tokenTypeOf(token->prev))
    {
        std::cerr << "Token " << *token << " requires token of type " << tokenTypeOf(token->prev) << " to the right, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }

    // set token's value to an array of its operands
    token->value = toValue((new Token*[2] {token->prev, token->next}));

    // update symbol table
    symbol_table::SymbolTable::assign(
        (std::string*) token->prev->value,
        new symbol_table::Symbol(token->next->value, token->prev->type)
    );

    // type of assignment operator is the type of the variable it has assigned a value to
    token->type = tokenTypeOf(token->prev);

    // remove tokens to the left and right
    statement->remove(token->prev);
    statement->remove(token->next);

}


// increment ++, decrement -- operators
void incDecSatisfy(Token* token, syntax_tree::Statement* statement)
{
    if (token->prev == nullptr)
    {
        std::cerr << "Missing token of operator type " << OpCodes::REFERENCE << " to the left of " << *token << std::endl;
        exit(1);
    }
    if (token->prev->opCode != OpCodes::REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << OpCodes::REFERENCE << " to the left, but " << *token->prev << " was provided" << std::endl;
        exit(1);
    }


    token->value = toValue(new Token*[1] { token->prev });
    token->type = tokenTypeOf(token->prev);

    statement->remove(token->prev);

}


void addressOfSatisfy(Token* token, syntax_tree::Statement* statement)
{
    if (token->next == nullptr)
    {
        std::cerr << "Missing token of operator type " << OpCodes::REFERENCE << " to the right of " << *token << std::endl;
        exit(1);
    }
    if (token->next->opCode != OpCodes::REFERENCE)
    {
        std::cerr << "Token " << *token << " requires token of operator type " << OpCodes::REFERENCE << " to the right, but " << *token->next << " was provided" << std::endl;
        exit(1);
    } 
    
    token->value = toValue(new Token*[1] { token->next });
    token->type = tokenTypeOf(token->next);

    statement->remove(token->next);
}



void syntax_tree::Statement::satisfy(Token* token)
{
    // set token's priority to 0 right away so that it doesn't get evaluated multiple times
    token->priority = 0;

    // satisfy operator's requirements based on it's operation code
    switch (token->opCode)
    {
    case OpCodes::ARITHMETICAL_SUM:
    case OpCodes::ARITHMETICAL_SUB:
    case OpCodes::ARITHMETICAL_MUL:
    case OpCodes::ARITHMETICAL_DIV:
    case OpCodes::ARITHMETICAL_POW:
    {
        binarySatisfy(token, TokenType::INT, TokenType::INT, this);
        break;
    }

    case OpCodes::ARITHMETICAL_INC:
    case OpCodes::ARITHMETICAL_DEC:
    {
        incDecSatisfy(token, this);
        break;
    }


    case OpCodes::LOGICAL_EQ:
    case OpCodes::LOGICAL_NOT_EQ:
    case OpCodes::LOGICAL_AND:
    case OpCodes::LOGICAL_OR:
    case OpCodes::LOGICAL_LESS:
    case OpCodes::LOGICAL_LESS_EQ:
    case OpCodes::LOGICAL_GREATER:
    case OpCodes::LOGICAL_GREATER_EQ:
    {
        binarySatisfy(token, TokenType::BOOL, TokenType::BOOL, this);
        break;
    }
    
    case OpCodes::LOGICAL_NOT:
    {
        unarySatisfy(token, TokenType::BOOL, RIGHT, this);
        break;
    }


    case OpCodes::ADDRESS_OF:
    {
        addressOfSatisfy(token, this);
        break;
    }


    case OpCodes::ASSIGNMENT_ASSIGN:
    case OpCodes::ASSIGNMENT_ADD:
    case OpCodes::ASSIGNMENT_SUB:
    case OpCodes::ASSIGNMENT_DIV:
    case OpCodes::ASSIGNMENT_POW:
    case OpCodes::ASSIGNMENT_MUL:
    {
        assignSatisfy(token, this);
        break;
    }


    case OpCodes::DECLARATION_INT:
        declarationSatisfy(token, TokenType::INT, this);
        break;
    case OpCodes::DECLARATION_STRING:
        declarationSatisfy(token, TokenType::STRING, this);
        break;
    case OpCodes::DECLARATION_FLOAT:
        declarationSatisfy(token, TokenType::FLOAT, this);
        break;
    case OpCodes::DECLARATION_BOOL:
        declarationSatisfy(token, TokenType::BOOL, this);
        break;

    
    case OpCodes::PUSH_SCOPE:
    {
        using namespace syntax_tree;

        // first push the scope to the SymbolTable
        symbol_table::SymbolTable::pushScope(DO_INHERIT);

        // transfer the part of the statement which is in the new scope
        // to another statement
        Statement* scopeStatement = new Statement(token->next);
        scopeStatement->next = next;
        
        // set previous token to null since this is the beginning of the statement
        scopeStatement->root->prev = nullptr;

        // create a new statement list for the new syntax tree
        Statements* scopeStatements = new Statements(scopeStatement);        

        // search for closing scope
        unsigned int depth = 1;
        for (Token* tok = scopeStatement->root; true; tok = tok->next)
        {
            // jump to next statement if current statement is finished
            if (tok == nullptr)
            {
                // search for closing scope in next statement
                scopeStatement = scopeStatement->next;

                // check if the statement actually exists
                if (scopeStatement == nullptr)
                {
                    std::cerr << "Missing closing scope because next statement is missing" << std::endl;
                    exit(1);
                }

                tok = scopeStatement->root;
            }

            // increase or decrease scope depth if another scope is found
            if (tok->opCode == OpCodes::PUSH_SCOPE)
            {
                depth ++;
            }
            else if (tok->opCode == OpCodes::POP_SCOPE)
            {
                depth --;

                // if depth is 0 --> found end of scope
                if (depth == 0)
                {
                    // this is the last statement of the scope
                    scopeStatements->end = scopeStatement;

                    // make the statement (the one before the scope) 
                    // continue from this token's next
                    if (tok->next != nullptr)
                    {
                        tok->next->prev = token;
                    }
                    token->next = tok->next;

                    // make the scopeStatement end right before the closing scope Token
                    if (tok->prev != nullptr)
                    {
                        tok->prev->next = nullptr;
                    }

                    // delete closing scope token since it won't be used anymore
                    delete tok;

                    // make the outer Statement linked list (Statements)
                    // continue from the next statement
                    next = scopeStatement->next;

                    // if statement's root is the newly deleted token --> delete it
                    if (scopeStatement->root == tok)
                    {
                        scopeStatements->removeLast();
                        delete scopeStatement;
                    }
                    // whereas if the statement isn't empty
                    else
                    {
                        // break the linked list on end of scope
                        scopeStatement->next = nullptr;
                    }
                    
                    break;
                }
            }
        }
        
        // create a new SyntaxTree for the scope
        SyntaxTree* scopeTree = new SyntaxTree(*scopeStatements);

        // parse the new SyntaxTree recursively
        scopeTree->parse();

        // pop the Scope once parsed its SyntaxTree
        symbol_table::SymbolTable::popScope();

        // set the token's value to the Scope's content
        token->value = toValue(scopeTree);

        break;
    }
    

    case OpCodes::PARENTHESIS:
    {
        // check if there's anything inside parenthesis
        Token* content = token->next;
        if (content == nullptr)
        {
            std::cerr << "Missing statement inside parenthesis" << std::endl;
            exit(1);
        }

        // check for closing parenthesis
        Token* closing = content->next;
        if (closing == nullptr || 
            (closing->opCode != OpCodes::PARENTHESIS && closing->value != ')'))
        {
            std::cerr << "Missing closing parenthesis" << std::endl;
            exit(1);
        }

        // parenthesis' value is it's content's
        token->value = toValue(new Token*[1] { content });

        // set parenthesis' type to it's content's
        token->type = tokenTypeOf(content);

        remove(content);
        remove(closing, DELETE);

        break;
    }


    case OpCodes::FLOW_IF:
    {
        Token* condition = token->next;

        // check if a boolean condition is provided
        if (condition == nullptr)
        {
            std::cerr << "Missing boolean condidion after if keyword" << std::endl;
            exit(1);
        }

        // check if condition is actually a boolean statement
        if (condition->type != TokenType::BOOL)
        {
            std::cerr << "Expected token of type " << TokenType::BOOL << " after if keyword, but " << condition << " was provided" << std::endl;
            exit(1);
        }

        Token* body = condition->next;

        // check if body exists
        if (body == nullptr)
        {
            std::cerr << "Missing if statement's body after condition" << std::endl;
            exit(1);
        }

        // set if token's value to an array of its boolean condition and its body
        token->value = toValue((new Token*[2] { condition, body }));

        // remove operands from the statement (this)
        remove(condition);
        remove(body);

        break;
    }


    case OpCodes::FLOW_ELSE:
    {
        Token* body = token->next;

        // check if body exists
        if (body == nullptr)
        {
            std::cerr << "Missing else statement's body" << std::endl;
            exit(1);
        }

        token->value = toValue(new Token*[1] { body });

        remove(body);

        break;
    }


    case OpCodes::FLOW_WHILE:
    case OpCodes::FLOW_FOR:
    {
        std::cerr << "not implemented" << std::endl;
        exit(1);
        break;
    }

    } // switch (token->opCode)

}

