#include "op_codes.hh"
#include "token.hh"
#include "keywords.hh"
#include "utils.hh"


#define AddToken tokens->add(token); token = nullptr;


Tokens::TokenList* Tokens::tokenize(std::string& script) 
{
    using namespace Tokens;

    TokenList* tokens = new TokenList();

    Token* token = nullptr;


    char c;
    for (uint i = 0; (c = script[i]) != 0; i++)
    {

        if (token != nullptr)
        {
            switch (token->opCode)
            {
            case OpCodes::LITERAL:
            {
                switch (token->type)
                {
                    case INT: 
                    {
                        if (isDigit(c))
                        {
                            if (token == nullptr)
                            {
                                token = new Token(INT, LITERAL_P, OpCodes::LITERAL, toDigit(c));
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
                    } // INT

                    case STRING: 
                    {
                
                        // terminate string
                        if (c == '"')
                        {
                            AddToken;
                            continue;
                        }

                        // add character to string
                        *((std::string*) token->value) += c;
                        continue;
                    } // STRING

                } // switch (token->type)

                break;
            } // LITERAL

            case OpCodes::NO_OP:
            {

                switch (token->type)
                {
                    case TEXT: 
                    {
                        if (isText(c) || isDigit(c))
                        {
                            // add character to string
                            *((std::string*) token->value) += c;
                            continue;
                        }

                        // end of text token
                        // check if text is a keyword
                        OpCodes opCode = Keywords::isKeyword(*(std::string*) token->value);
                        if (opCode == OpCodes::NO_OP)
                        {
                            // if word is not a keyword it's a reference
                            token->opCode = OpCodes::REFERENCE;
                            AddToken;
                        } else {
                            // if word is a keyword instead
                            token = new Token(KEYWORD, Keywords::keywordPriority(opCode), opCode);
                            AddToken;
                        }

                        break;
                    } // TEXT

                } // switch (token->type)

                break;
            } // NO_OP
            
            // arithmetical operators

            case OpCodes::ARITHMETICAL_SUM:
            {
                if (c == '=')
                {
                    token = new Token(NONE, ASSIGNMENT_P, OpCodes::ASSIGNMENT_ADD);
                    AddToken;
                    continue;
                } else if (c == '+')
                {
                    token = new Token(NONE, INCREMEN_P, OpCodes::ARITHMETICAL_INC);
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            }
            
            case OpCodes::ARITHMETICAL_SUB:
            {
                if (c == '=')
                {
                    token = new Token(NONE, ASSIGNMENT_P, OpCodes::ARITHMETICAL_SUB);
                    AddToken;
                    continue;
                } else if (c == '-')
                {
                    token = new Token(NONE, DECREMENT_P, OpCodes::ARITHMETICAL_DEC);
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            }
            
            case OpCodes::ARITHMETICAL_MUL:
            {
                if (c == '=')
                {
                    token = new Token(NONE, ASSIGNMENT_P, OpCodes::ASSIGNMENT_MUL);
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            }

            case OpCodes::ARITHMETICAL_DIV:
            {
                if (c == '=')
                {
                    token = new Token(NONE, ASSIGNMENT_P, OpCodes::ASSIGNMENT_DIV);
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            }

            case OpCodes::ARITHMETICAL_POW:
            {
                if (c == '=')
                {
                    token = new Token(NONE, ASSIGNMENT_P, OpCodes::ASSIGNMENT_POW);
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            }

            // assignment operators    
            
            case OpCodes::ASSIGNMENT_ASSIGN: {
                
                if (c == '=')
                {
                    token = new Token(NONE, EQUALITY_P, OpCodes::LOGICAL_EQ);
                    AddToken;
                    continue;
                }
                
                AddToken;
                break;
            }

            } // switch (token->opCodes)

        } // if (token != nullptr)


        if (isDigit(c))
        {
            token = new Token(INT, LITERAL_P, OpCodes::LITERAL, toDigit(c));
            continue;
        }


        if (c == '"')
        {
            token = new Token(TokenType::STRING, LITERAL_P, OpCodes::LITERAL, (Value) (new std::string("")));
            continue;
        }

        if (c == '+')
        {
            token = new Token(NONE, SUM_P, OpCodes::ARITHMETICAL_SUB);
            continue;
        }
        if (c == '-')
        {
            token = new Token(NONE, SUBTRACTION_P, OpCodes::ARITHMETICAL_SUB);
            continue;
        }
        if (c == '*')
        {
            token = new Token(NONE, MULTIPLICATION_P, OpCodes::ARITHMETICAL_MUL);
            continue;
        }
        if (c == '/')
        {
            token = new Token(NONE, DIVISION_P, OpCodes::ARITHMETICAL_DIV);
            continue;
        }
        if (c == '^')
        {
            token = new Token(NONE, DIVISION_P, OpCodes::ARITHMETICAL_POW);
            continue;
        }


        if (c == '=')
        {
            token = new Token(NONE, ASSIGNMENT_P, OpCodes::ASSIGNMENT_ASSIGN);
            continue;
        }


        if (isText(c))
        {
            token = new Token(TEXT, LITERAL_P, OpCodes::NO_OP, (Value) (new std::string {c}));
            continue;
        }


        if (c == ';')
        {
            token = new Token(ENDS, 0, OpCodes::NO_OP);
            AddToken;
            continue;
        }


    }

    return tokens;
}

