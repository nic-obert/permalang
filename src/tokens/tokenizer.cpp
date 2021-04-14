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

            case OpCodes::ADDRESS_OF:
            {
                if (c == '&')
                {
                    token->opCode = OpCodes::LOGICAL_AND;
                    token->priority = AND_P;
                    AddToken;
                    continue;
                }

                AddToken;
                break;

            } // case ADDRESS_OF


            case OpCodes::LITERAL:
            {
                switch (token->type)
                {
                    case INT: 
                    {
                        if (isDigit(c))
                        {
                            token->value *= 10;
                            token->value += toDigit(c);                  
                            continue;
                        } 
                        else if (c == '.')
                        {
                            //decimal

                            continue;
                        }
                        
                        // char is not a digit
                        AddToken;               

                        break;
                    } // case INT

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
                    } // case STRING

                } // switch (token->type)

                break;
            } // case LITERAL


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
                            // check if keyword has a predefined value

                            // temporary variables
                            Value _value; 
                            TokenType _type;

                            if (Keywords::hasValue(*(std::string*) token->value, _value, _type))
                            {
                                token->type = _type;
                                token->priority = LITERAL_P;
                                token->opCode = OpCodes::LITERAL;
                                token->value = _value;
                                AddToken;
                                break;
                            }

                            // if word is not a keyword it's a reference
                            token->opCode = OpCodes::REFERENCE;
                            AddToken;
                        } else {
                            // if word is a keyword instead
                            token->type = KEYWORD;
                            token->priority = Keywords::keywordPriority(opCode);
                            token->opCode = opCode;
                            AddToken;
                        }

                        break;
                    } // case TEXT

                    default:
                    {
                        
                        if (token->value == '|' && c == '|')
                        {
                            token->priority = OR_P;
                            token->opCode = LOGICAL_OR;
                            AddToken;
                            continue;
                        }

                    } // default

                } // switch (token->type)

                break;
            } // case NO_OP
            
// ARITHMETICAL OPERATORS

            case OpCodes::ARITHMETICAL_SUM:
            {
                if (c == '=')
                {
                    token->type = NONE;
                    token->priority = ASSIGNMENT_P;
                    token->opCode = OpCodes::ASSIGNMENT_ADD;
                    AddToken;
                    continue;
                } else if (c == '+')
                {
                    token->type = NONE;
                    token->priority = INCREMEN_P;
                    token->opCode = OpCodes::ARITHMETICAL_INC;
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            } // case ARITHMETICAL_SUM
            
            case OpCodes::ARITHMETICAL_SUB:
            {
                if (c == '=')
                {
                    token->type = NONE;
                    token->priority = ASSIGNMENT_P;
                    token->opCode = OpCodes::ARITHMETICAL_SUB;
                    AddToken;
                    continue;
                } else if (c == '-')
                {
                    token->type = NONE;
                    token->priority = DECREMENT_P;
                    token->opCode = OpCodes::ARITHMETICAL_DEC;
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            } // case ARITHMETICAL_SUB
            
            case OpCodes::ARITHMETICAL_MUL:
            {
                if (c == '=')
                {
                    token->type = NONE;
                    token->priority = ASSIGNMENT_P;
                    token->opCode = OpCodes::ASSIGNMENT_MUL;
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            } // case ARITHMETICAL_MUL

            case OpCodes::ARITHMETICAL_DIV:
            {
                if (c == '=')
                {
                    token->type = NONE;
                    token->priority = ASSIGNMENT_P;
                    token->opCode = OpCodes::ASSIGNMENT_DIV;
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            } // case ARITHMETICAL_DIV

            case OpCodes::ARITHMETICAL_POW:
            {
                if (c == '=')
                {
                    token->type = NONE;
                    token->priority = ASSIGNMENT_P;
                    token->opCode = OpCodes::ASSIGNMENT_POW;
                    AddToken;
                    continue;
                }
                AddToken;
                break;
            }

// ASSIGNMENT OPERATORS   
            
            case OpCodes::ASSIGNMENT_ASSIGN:
            {
                
                if (c == '=')
                {
                    token->type = NONE;
                    token->priority = EQUALITY_P;
                    token->opCode = OpCodes::LOGICAL_EQ;
                    AddToken;
                    continue;
                }
                
                AddToken;
                break;
            } // case ASSIGNMENT_ASSIGN

// LOGICAL OPERATORS

            case OpCodes::LOGICAL_NOT:
            {
                // !=
                if (c == '=')
                {
                    token->type = NONE;
                    token->priority = INEQUALITY_P;
                    token->opCode = OpCodes::LOGICAL_NOT_EQ;
                    AddToken;
                    continue;
                }
                
                AddToken;
                break;
            } // case LOGICAL_NOT



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
            token = new Token(NONE, SUM_P, OpCodes::ARITHMETICAL_SUM);
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

        if (c == '&')
        {
            token = new Token(NONE, ADDRESS_OF_P, OpCodes::ADDRESS_OF);
            continue;
        }

        if (c == '|')
        {
            token = new Token(NONE, 0, OpCodes::NO_OP, '|');
            continue;
        }


        if (isText(c))
        {
            token = new Token(TEXT, LITERAL_P, OpCodes::NO_OP, (Value) (new std::string {c}));
            continue;
        }


        if (c == ';')
        {
            token = new Token(ENDS, LITERAL_P, OpCodes::NO_OP);
            AddToken;
            continue;
        }

        if (c == '!')
        {
            token = new Token(NONE, NOT_P, OpCodes::LOGICAL_NOT);
            continue;
        }


    }

    return tokens;
}

