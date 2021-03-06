#pragma once

#include <string>

#include "token.cpp"
#include "priorities.hh"


#define isDigit(x) (47 < x && x < 58)
#define toDigit(x) (x - 48)


TokenList* tokenize(std::string script) 
{
    TokenList* tokens = new TokenList();

    Token* token;
    TokenType type = NONE;

    char c;
    uint i = 0;
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
                tokens->add(token);
                token = nullptr;
                type = NONE;                

                break;
            

            case TEXT:

                break;


            case STRING:
                
                // terminate string
                if (c == '"')
                {
                    type = NONE;
                    tokens->add(token);
                    token = nullptr;
                    continue;
                }

                // add character to string
                *((std::string*) token->value) += c;
                continue;
            

            case ARITHMETIC_OPERATOR:

                // no compound operator

                tokens->add(token);
                token = nullptr;
                type = NONE;

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
            type = ARITHMETIC_OPERATOR;
            token = new Token(ARITHMETIC_OPERATOR, SUM_P, '+');
            continue;
        }
        if (c == '-')
        {
            type = ARITHMETIC_OPERATOR;
            token = new Token(ARITHMETIC_OPERATOR, SUBTRACTION_P, '-');
            continue;
        }
        if (c == '*')
        {
            type = ARITHMETIC_OPERATOR;
            token = new Token(ARITHMETIC_OPERATOR, MULTIPLICATION_P, '*');
            continue;
        }
        if (c == '/')
        {
            type = ARITHMETIC_OPERATOR;
            token = new Token(ARITHMETIC_OPERATOR, DIVISION_P, '/');
            continue;
        }



    }

    return tokens;
}

