
#include <unordered_map>

#include "priorities.hh"
#include "keywords.hh"



const char* Keywords::keywordName(Keywords keyword)
{
    switch (keyword)
    {
    case IF:
        return "if";
    
    case ELSE:
        return "else";
            
    case INT:
        return "int";
    
    case BOOL:
        return "bool";
    
    case FLOAT:
        return "float";
            
    case STRING:
        return "string";

    }
}


Keywords::Keywords Keywords::isKeyword(std::string word)
{
    auto keyword = keywordsMap.find(word);
    if (keyword == keywordsMap.end())
        return __NOKEY;
    return keyword->second;
}


int Keywords::keywordPriority(Keywords keyword)
{
    switch (keyword)
    {
    case IF:
        return IF_P;
    
    case ELSE:
        return ELSE_P;
    
    case INT:
        return DECLARATION_P;
    
    case BOOL:
        return DECLARATION_P;

    case FLOAT:
        return DECLARATION_P;
    
    case STRING:
        return DECLARATION_P;
    
    }
}

