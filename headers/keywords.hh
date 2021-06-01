#pragma once

#include "priorities.hh"
#include "token.hh"


namespace keywords
{

    // returns a keyword's name given its opCode
    const char* keywordName(OpCodes keyword);


    // maps string to a keyword
    extern const std::unordered_map<std::string, OpCodes> keywordsMap;


    // a constant structure holding a keyword's predefined value
    typedef struct KeywordValue
    {
        const Value value;
        const Tokens::TokenType type;

        KeywordValue(Value value, Tokens::TokenType type);

    } KeywordValue;


    // maps a keyword to a value
    extern const std::unordered_map<std::string, KeywordValue> keywordValues;


    // whether the given keyword has a predefiend value or not
    bool hasValue(std::string const& word, Value& value, Tokens::TokenType& type);


    // returns the priority of a given keyword using a switch statement
    int keywordPriority(OpCodes keyword);


    OpCodes isKeyword(std::string const& word);


    Tokens::TokenType declarationType(OpCodes keyword);

};


