#include "token.hh"
#include "op_codes.hh"
#include "priorities.hh"
#include "keywords.hh"
#include "errors.hh"


using namespace Tokens;


#define AddToken add(token); token = nullptr;



void TokenList::add(Token* token) 
{   
    if (first == nullptr)
    {
        first = token;
        last = first;
        return;
    }
    last->next = token;
    token->prev = last;
    last = token;
}


void TokenList::remove(Token* token)
{
    Token::removeToken(token);

    delete token;
}


std::ostream& operator<<(std::ostream& stream, const TokenList& list)
{
    stream << "Token List: {\n";
    if (list.first == nullptr)
        stream << "\tEmpty token line\n";

    for (Tokens::Token* token = list.first; token != nullptr; token = token->next)
    {
        stream << "\t" << *token << "\n";
    }

    return stream << "}";
}


TokenList::TokenList(std::string& script) 
{
    Token* token = nullptr;

    size_t currentPriority = 0;


    char c;
    for (size_t i = 0; (c = script[i]) != 0; i++)
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
                    token->priority = AND_P + currentPriority;
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
                    case TokenType::INT: 
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

                    case TokenType::STRING: 
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
                    case TokenType::TEXT: 
                    {
                        if (isText(c) || isDigit(c))
                        {
                            // add character to string
                            *((std::string*) token->value) += c;
                            continue;
                        }

                        // end of text token
                        // check if text is a keyword
                        OpCodes opCode = keywords::isKeyword(*(std::string*) token->value);
                        if (opCode == OpCodes::NO_OP)
                        {   
                            // check if keyword has a predefined value

                            // temporary variables
                            Value _value; 
                            TokenType _type;

                            if (keywords::hasValue(*(std::string*) token->value, _value, _type))
                            {
                                token->type = _type;
                                token->priority = LITERAL_P;
                                token->opCode = OpCodes::LITERAL;

                                // delete the string value since it won't be used anymore
                                delete (std::string*) token->value;

                                token->value = _value;
                                AddToken;
                                break;
                            }

                            // if word is not a keyword it's a reference
                            token->opCode = OpCodes::REFERENCE;
                            AddToken;
                        } else {
                            // if word is a keyword instead
                            token->type = TokenType::KEYWORD;
                            token->priority = keywords::keywordPriority(opCode) + currentPriority;
                            token->opCode = opCode;

                            // delete the string value since it won't be used anymore
                            delete (std::string*) token->value;

                            AddToken;
                        }

                        break;
                    } // case TEXT

                    default:
                    {
                        
                        if (token->value == '|' && c == '|')
                        {
                            token->priority = OR_P + currentPriority;
                            token->opCode = OpCodes::LOGICAL_OR;
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
                    token->type = TokenType::NONE;
                    token->priority = ASSIGNMENT_P + currentPriority;
                    token->opCode = OpCodes::ASSIGNMENT_ADD;
                    AddToken;
                    continue;
                } else if (c == '+')
                {
                    token->type = TokenType::NONE;
                    token->priority = INCREMEN_P + currentPriority;
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
                    token->type = TokenType::NONE;
                    token->priority = ASSIGNMENT_P + currentPriority;
                    token->opCode = OpCodes::ARITHMETICAL_SUB;
                    AddToken;
                    continue;
                } else if (c == '-')
                {
                    token->type = TokenType::NONE;
                    token->priority = DECREMENT_P + currentPriority;
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
                    token->type = TokenType::NONE;
                    token->priority = ASSIGNMENT_P + currentPriority;
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
                    token->type = TokenType::NONE;
                    token->priority = ASSIGNMENT_P + currentPriority;
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
                    token->type = TokenType::NONE;
                    token->priority = ASSIGNMENT_P + currentPriority;
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
                    token->type = TokenType::NONE;
                    token->priority = EQUALITY_P + currentPriority;
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
                    token->type = TokenType::NONE;
                    token->priority = INEQUALITY_P + currentPriority;
                    token->opCode = OpCodes::LOGICAL_NOT_EQ;
                    AddToken;
                    continue;
                }
                
                AddToken;
                break;
            } // case LOGICAL_NOT



            } // switch (token->opCodes)

        } // if (token != nullptr)


        switch (c)
        {
        case '"':
        {
            token = new Token(TokenType::STRING, LITERAL_P, OpCodes::LITERAL, toValue(new std::string("")));
            continue;
        }

        case '+':
        {
            token = new Token(TokenType::NONE, SUM_P + currentPriority, OpCodes::ARITHMETICAL_SUM);
            continue;
        }
        
        case '-':
        {
            token = new Token(TokenType::NONE, SUBTRACTION_P + currentPriority, OpCodes::ARITHMETICAL_SUB);
            continue;
        }

        case '*':
        {
            token = new Token(TokenType::NONE, MULTIPLICATION_P + currentPriority, OpCodes::ARITHMETICAL_MUL);
            continue;
        }

        case '/':
        {
            token = new Token(TokenType::NONE, DIVISION_P + currentPriority, OpCodes::ARITHMETICAL_DIV);
            continue;
        }
        
        case '^':
        {
            token = new Token(TokenType::NONE, POWER_P + currentPriority, OpCodes::ARITHMETICAL_POW);
            continue;
        }
        
        case '=':
        {
            token = new Token(TokenType::NONE, ASSIGNMENT_P + currentPriority, OpCodes::ASSIGNMENT_ASSIGN);
            continue;
        }

        case '&':
        {
            token = new Token(TokenType::NONE, ADDRESS_OF_P + currentPriority, OpCodes::ADDRESS_OF);
            continue;
        }
        
        case '|':
        {
            token = new Token(TokenType::NONE, 0, OpCodes::NO_OP, '|');
            continue;
        }
        
        case ';':
        {
            token = new Token(TokenType::ENDS, LITERAL_P, OpCodes::NO_OP);
            AddToken;
            continue;
        }
        
        case '!':
        {
            token = new Token(TokenType::NONE, NOT_P + currentPriority, OpCodes::LOGICAL_NOT);
            continue;
        }
        
        case '{':
        {   
            // increment token priority to evaluate stuff in scope first
            currentPriority += PARENTHESIS_P;

            token = new Token(TokenType::SCOPE, SCOPE_P + currentPriority, OpCodes::PUSH_SCOPE);
            AddToken;
            continue;
        }
        
        case '}':
        {   
            token = new Token(TokenType::SCOPE, SCOPE_P + currentPriority, OpCodes::POP_SCOPE);

            currentPriority -= PARENTHESIS_P;

            AddToken;
            continue;
        }
        
        case '(':
        {
            // increment token priority to evaluate stuff in parenthesis first
            currentPriority += PARENTHESIS_P;

            // function call
            if (last->type == TokenType::TEXT)
            {
                token = new Token(TokenType::NONE, currentPriority, OpCodes::CALL);
                continue;
            }

            // ordinary parenthesis
            token = new Token(TokenType::PARENTHESIS, currentPriority, OpCodes::PARENTHESIS, '(');
            
            AddToken;
            continue;
        }

        case ')':
        {
            // decrement token priority
            currentPriority -= PARENTHESIS_P;

            token = new Token(TokenType::PARENTHESIS, 0, OpCodes::PARENTHESIS, ')');
            
            AddToken;
            continue;
        }

        case ' ':
        case '\n':
        case '\r':
        case '\t':
        {
            continue;
        }

        default:
        {
            if (isDigit(c))
            {
                token = new Token(TokenType::INT, LITERAL_P, OpCodes::LITERAL, toDigit(c));
                continue;
            }

            if (isText(c))
            {
                token = new Token(TokenType::TEXT, LITERAL_P, OpCodes::NO_OP, toValue(new std::string {c}));
                continue;
            }


            // character isn't handled
            // extract the line of the invalid character

            // use C strings for convenience
            const char* cString = script.c_str();

            // find the start of the string
            size_t tmpIndex = i - 1;
            for (; cString[tmpIndex] != '\n'; tmpIndex--);

            size_t length = i - tmpIndex;
            const char* start = cString + tmpIndex;

            // find the end of the string
            tmpIndex = i + 1;
            for (; cString[tmpIndex] != '\n'; tmpIndex++);

            length = tmpIndex - i;

            errors::InvalidCharacterError(std::string(start, length), cString[i]);
    

        } // default

        } // switch (c)    

    } // for (uint i = 0; (c = script[i]) != 0; i++)

    // add a closing end of statement token
    add(new Token(TokenType::ENDS, LITERAL_P, OpCodes::NO_OP));

}

