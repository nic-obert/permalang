#pragma once

#include <string>

#include "token.cpp"
#include "priorities.hh"
#include "operators/operators.hh"


#define isDigit(x) (47 < x && x < 58)
#define toDigit(x) (x - 48)
#define AddToken tokens->add(token); token = nullptr; type = NONE;


TokenList* tokenize(std::string script) 
{
    TokenList* tokens = new TokenList();

    Token* token;
    TokenType type = NONE;

    char c;
    for (uint i = 0; (c = script[i]) != 0; i++)
    {

        switch (type)
        {
            case NUMBER:
                if (isDigit(c))
                {
                    if (token == nullptr)
                    {
                        token = new Token(NUMBER, LITERAL_P, toDigit(c));
                    } else {
                        token->value *= 10;
                        token->value += toDigit(c);
                    }

                    continue;
                } else if (c == '.')
                {
                    //decimal

                    continue;
                }
                
                // char is not a digit
                AddToken;               

                break;
            

            case TEXT:

                break;


            case STRING:
                
                // terminate string
                if (c == '"')
                {
                    AddToken;
                    continue;
                }

                // add character to string
                *((std::string*) token->value) += c;
                continue;
            

            case ARITHMETIC_OP:

                switch (token->value)
                {
                case ArithmeticalOperators::SUM:
                    if (c == '=')
                    {
                        token = new Token(ASSIGNMENT_OP, ASSIGNMENT_P, AssignmentOperators::ADD);
                        AddToken;
                        continue;
                    } else if (c == '+')
                    {
                        token = new Token(ARITHMETIC_OP, INCREMEN_P, ArithmeticalOperators::INCREMENT);
                        AddToken;
                        continue;
                    }
                    break;
                
                case ArithmeticalOperators::SUBTRACTION:
                    if (c == '=')
                    {
                        token = new Token(ASSIGNMENT_OP, ASSIGNMENT_P, AssignmentOperators::SUBTRACT);
                        AddToken;
                        continue;
                    } else if (c == '-')
                    {
                        token = new Token(ARITHMETIC_OP, DECREMENT_P, ArithmeticalOperators::DECREMENT);
                        AddToken;
                        continue;
                    }
                    break;
                
                case ArithmeticalOperators::MULTIPLICATION:
                    if (c == '=')
                    {
                        token = new Token(ASSIGNMENT_OP, ASSIGNMENT_P, AssignmentOperators::MULTIPLY);
                        AddToken;
                        continue;
                    }
                    break;

                case ArithmeticalOperators::DIVISION:
                    if (c == '=')
                    {
                        token = new Token(ASSIGNMENT_OP, ASSIGNMENT_P, AssignmentOperators::DIVIDE);
                        AddToken;
                        continue;
                    }
                    break;

                case ArithmeticalOperators::POWER:
                    if (c == '=')
                    {
                        token = new Token(ASSIGNMENT_OP, ASSIGNMENT_P, AssignmentOperators::ELEVATE);
                        AddToken;
                        continue;
                    }
                    break;
                
                }

                // no compound operator

                AddToken;
                break;
            

            case ASSIGNMENT_OP:

                if (c == '=')
                {
                    token = new Token(LOGICAL_OP, EQUALITY_P, LogicalOperators::EQUALITY);
                    AddToken;
                    continue;
                }
                
                AddToken;
                break;

        }


        if (isDigit(c))
        {
            token = new Token(NUMBER, LITERAL_P, toDigit(c));
            type = NUMBER;

            continue;
        }


        if (c == '"')
        {
            type = STRING;
            token = new Token(STRING, LITERAL_P, (unsigned long) (new std::string("")));
            continue;
        }

        if (c == '+')
        {
            type = ARITHMETIC_OP;
            token = new Token(ARITHMETIC_OP, SUM_P, ArithmeticalOperators::SUM);
            continue;
        }
        if (c == '-')
        {
            type = ARITHMETIC_OP;
            token = new Token(ARITHMETIC_OP, SUBTRACTION_P, ArithmeticalOperators::SUBTRACTION);
            continue;
        }
        if (c == '*')
        {
            type = ARITHMETIC_OP;
            token = new Token(ARITHMETIC_OP, MULTIPLICATION_P, ArithmeticalOperators::MULTIPLICATION);
            continue;
        }
        if (c == '/')
        {
            type = ARITHMETIC_OP;
            token = new Token(ARITHMETIC_OP, DIVISION_P, ArithmeticalOperators::DIVISION);
            continue;
        }
        if (c == '^')
        {
            type = ARITHMETIC_OP;
            token = new Token(ARITHMETIC_OP, DIVISION_P, ArithmeticalOperators::POWER);
            continue;
        }


        if (c == '=')
        {
            type = ASSIGNMENT_OP;
            token = new Token(ASSIGNMENT_OP, ASSIGNMENT_P, AssignmentOperators::ASSIGNMENT);
            continue;
        }



    }

    return tokens;
}

