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


    const char* keywordName(Keywords keyword);


    const std::unordered_map<std::string, Keywords> keywordsMap = 
    {
        {"if", IF},
        {"else", ELSE},
        {"int", INT},
        {"bool", BOOL},
        {"float", FLOAT},
        {"string", STRING}
    };


    int keywordPriority(Keywords keyword);


    Keywords isKeyword(std::string word);

};


