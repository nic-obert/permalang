#pragma once

#include "pch.hh"

#include "priorities.hh"
#include "token.hh"
#include "op_codes.hh"


namespace Keywords
{

    const char* keywordName(OpCodes keyword);


    const std::unordered_map<std::string, OpCodes> keywordsMap ({
            {"if",      OpCodes::FLOW_IF},
            {"else",    OpCodes::FLOW_ELSE},
            {"int",     OpCodes::DECLARATION_INT},
            {"bool",    OpCodes::DECLARATION_BOOL},
            {"float",   OpCodes::DECLARATION_FLOAT},
            {"string",  OpCodes::DECLARATION_STRING},
    });


    typedef struct KeywordValue
    {
        const Value value;
        const Tokens::TokenType type;

        KeywordValue(Value value, Tokens::TokenType type);

    } KeywordValue;


    const std::unordered_map<std::string, KeywordValue> keywordsValues ({
        {"true",    KeywordValue(1, Tokens::TokenType::BOOL)},
        {"false",   KeywordValue(0, Tokens::TokenType::BOOL)},
    });


    bool hasValue(std::string const& word, Value& value, Tokens::TokenType& type);


    int keywordPriority(OpCodes keyword);


    OpCodes isKeyword(std::string const& word);

};


