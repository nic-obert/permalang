#pragma once

#include <unordered_map>
#include <string>

#include "priorities.hh"
#include "op_codes.hh"


namespace Keywords
{

    const char* keywordName(OpCodes keyword);


    const std::unordered_map<std::string, OpCodes> keywordsMap ({
            {"if", OpCodes::FLOW_IF},
            {"else", OpCodes::FLOW_ELSE},
            {"int", DECLARATION_INT},
            {"bool", DECLARATION_BOOL},
            {"float", DECLARATION_FLOAT},
            {"string", DECLARATION_STRING}
    });


    int keywordPriority(OpCodes keyword);


    OpCodes isKeyword(std::string word);

};


