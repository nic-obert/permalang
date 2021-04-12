
#include <unordered_map>

#include "priorities.hh"
#include "keywords.hh"
#include "op_codes.hh"


using namespace Keywords;


const char* Keywords::keywordName(OpCodes keyword)
{
    switch (keyword)
    {
    case FLOW_IF:
        return "if";
    
    case FLOW_ELSE:
        return "else";
            
    case DECLARATION_INT:
        return "int";
    
    case DECLARATION_BOOL:
        return "bool";
    
    case DECLARATION_FLOAT:
        return "float";
            
    case DECLARATION_STRING:
        return "string";

    }

    return "UNDEFINED KEYWORD";
}


OpCodes Keywords::isKeyword(std::string const& word)
{
    auto keyword = keywordsMap.find(word);
    if (keyword == keywordsMap.end())
    {
        return OpCodes::NO_OP;
    }
    return keyword->second;
}


int Keywords::keywordPriority(OpCodes keyword)
{
    switch (keyword)
    {
    case FLOW_IF:
        return IF_P;
    
    case FLOW_ELSE:
        return ELSE_P;
    
    case DECLARATION_INT:
        return DECLARATION_P;
    
    case DECLARATION_BOOL:
        return DECLARATION_P;

    case DECLARATION_FLOAT:
        return DECLARATION_P;
    
    case DECLARATION_STRING:
        return DECLARATION_P;
    
    }

    return 0;
}


bool Keywords::hasValue(std::string const& word, Value& value, Tokens::TokenType& type)
{
    auto val = keywordsValues.find(word);
    if (val == keywordsValues.end())
    {
        return false;
    }
    value = val->second.value;
    type = val->second.type;
    return true;
}


KeywordValue::KeywordValue(Value value, Tokens::TokenType type)
: value(value), type(type)
{

}

