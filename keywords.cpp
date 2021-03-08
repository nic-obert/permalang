#pragma once

#include <unordered_map>

#include "priorities.hh"

namespace Keywords
{
    typedef enum Keywords
    {
        __NOKEY,
        IF,
        ELSE,
        INT,
        BOOL,
        FLOAT,
        STRING
    } Keywords;


    const char* keywordName(Keywords keyword)
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


    const std::unordered_map<std::string, Keywords> keywordsMap = 
    {
        {"if", IF},
        {"else", ELSE},
        {"int", INT},
        {"bool", BOOL},
        {"float", FLOAT},
        {"string", STRING}
    };


    int keywordPriority(Keywords keyword)
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


    Keywords isKeyword(std::string word)
    {
        auto keyword = keywordsMap.find(word);
        if (keyword == keywordsMap.end())
            return __NOKEY;
        return keyword->second;
    }

};
