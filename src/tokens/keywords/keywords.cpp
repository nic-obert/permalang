#include "keywords.hh"
#include "errors.hh"


using namespace keywords;


const std::unordered_map<std::string, OpCodes> keywords::keywordsMap
({
    {"if",      OpCodes::FLOW_IF},
    {"else",    OpCodes::FLOW_ELSE},
    {"while",   OpCodes::FLOW_WHILE},
    {"for",     OpCodes::FLOW_FOR},
    {"int",     OpCodes::DECLARATION_INT},
    {"bool",    OpCodes::DECLARATION_BOOL},
    {"float",   OpCodes::DECLARATION_FLOAT},
    {"string",  OpCodes::DECLARATION_STRING},
    {"double",  OpCodes::DECLARATION_DOUBLE},
    {"long",    OpCodes::DECLARATION_LONG},
    {"byte",    OpCodes::DECLARATION_BYTE},
    {"void",    OpCodes::DECLARATION_VOID},
    {"system",  OpCodes::SYSTEM},
    {"sysload", OpCodes::SYSTEM_LOAD},
});


// switch statements maps

const char* keywords::keywordName(OpCodes keyword)
{
    switch (keyword)
    {
    case OpCodes::FLOW_IF:
        return "if";
    
    case OpCodes::FLOW_ELSE:
        return "else";
    
    case OpCodes::FLOW_FOR:
        return "for";
    
    case OpCodes::FLOW_WHILE:
        return "while";
    
    case OpCodes::DECLARATION_INT:
        return "int";
    
    case OpCodes::DECLARATION_LONG:
        return "long";
    
    case OpCodes::DECLARATION_BOOL:
        return "bool";

    case OpCodes::DECLARATION_FLOAT:
        return "float";
    
    case OpCodes::DECLARATION_STRING:
        return "string";
    
    case OpCodes::DECLARATION_DOUBLE:
        return "double";
    
    case OpCodes::DECLARATION_BYTE:
        return "byte";
    
    case OpCodes::DECLARATION_VOID:
        return "void";
    
    case OpCodes::SYSTEM:
        return "system";
    
    case OpCodes::SYSTEM_LOAD:
        return "sysload";
    
    }
    
    std::string error("Undefined keyword: ");
    error += (char) keyword;
    errors::UnexpectedBehaviourError(std::move(error));
    return nullptr;
}


int keywords::keywordPriority(OpCodes keyword)
{
    switch (keyword)
    {
    case OpCodes::FLOW_IF:
        return IF_P;
    
    case OpCodes::FLOW_ELSE:
        return ELSE_P;
    
    case OpCodes::FLOW_FOR:
        return FOR_P;
    
    case OpCodes::FLOW_WHILE:
        return WHILE_P;
    
    case OpCodes::SYSTEM:
    case OpCodes::SYSTEM_LOAD:
        return SYSTEM_P;

    }

    if (isDeclarationOp(keyword))
    {
        return DECLARATION_P;
    }
    
    return 0;
}


OpCodes keywords::isKeyword(std::string const& word)
{
    auto keyword = keywordsMap.find(word);
    if (keyword == keywordsMap.end())
    {
        return OpCodes::NO_OP;
    }
    return keyword->second;
}


bool keywords::hasValue(std::string const& word, Value& value, Tokens::TokenType& type)
{
    auto val = keywordValues.find(word);
    if (val == keywordValues.end())
    {
        return false;
    }
    value = val->second.value;
    type = val->second.type;
    return true;
}


static const Tokens::TokenType declarationTypes[] =
{
    Tokens::TokenType::INT,
    Tokens::TokenType::FLOAT,
    Tokens::TokenType::STRING,
    Tokens::TokenType::BOOL,
    Tokens::TokenType::DOUBLE,
    Tokens::TokenType::LONG,
    Tokens::TokenType::NONE,
    Tokens::TokenType::BYTE
};


Tokens::TokenType keywords::declarationType(OpCodes keyword)
{
    // operation used to find the declaration index in the lookup table above
    return declarationTypes[(unsigned char) keyword - (unsigned char) OpCodes::DECLARATION_INT];
}

