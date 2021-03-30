
#include "token.hh"
#include "operators.hh"
#include "keywords.hh"


#define AddToken tokens->add(token); token = nullptr; type = NONE;


Tokens::TokenList* Tokens::tokenize(std::string script) 
{
    using namespace Tokens;
    using namespace operators::arithmetical;
    using namespace operators::logical;
    using namespace operators::assignment;

    TokenList* tokens = new TokenList();

    Token* token;
    TokenType type = NONE;

    char c;
    for (uint i = 0; (c = script[i]) != 0; i++)
    {

        switch (type)
        {
            case INT: {
                if (isDigit(c))
                {
                    if (token == nullptr)
                    {
                        token = new Token(INT, LITERAL_P, toDigit(c));
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
            }
            

            case TEXT: {

                if (isText(c) || isDigit(c))
                {
                    // add character to string
                    *((std::string*) token->value) += c;
                    continue;
                }

                // end of text token
                // check if text is a keyword
                Keywords::Keywords word = Keywords::isKeyword(*(std::string*) token->value);
                if (word == Keywords::Keywords::__NOKEY)
                {
                    AddToken;
                } else {
                    // if word is a keyword instead
                    token = new Token(KEYWORD, Keywords::keywordPriority(word), word);
                    AddToken;
                }

                break;
            }


            case STRING: {
                
                // terminate string
                if (c == '"')
                {
                    AddToken;
                    continue;
                }

                // add character to string
                *((std::string*) token->value) += c;
                continue;
            }


            case ARITHMETIC_OP: {

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
            }
            

            case ASSIGNMENT_OP: {
                
                if (c == '=')
                {
                    token = new Token(LOGICAL_OP, EQUALITY_P, LogicalOperators::EQUALITY);
                    AddToken;
                    continue;
                }
                
                AddToken;
                break;
            }

        }


        if (isDigit(c))
        {
            token = new Token(INT, LITERAL_P, toDigit(c));
            type = INT;

            continue;
        }


        if (c == '"')
        {
            type = TokenType::STRING;
            token = new Token(TokenType::STRING, LITERAL_P, (ulong) (new std::string("")));
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


        if (isText(c))
        {
            type = TEXT;
            token = new Token(TEXT, LITERAL_P, (ulong) (new std::string {c}));
            continue;
        }


        if (c == ';')
        {
            type = NONE;
            token = new Token(ENDS, 0, 0);
            AddToken;
            continue;
        }



    }

    return tokens;
}

