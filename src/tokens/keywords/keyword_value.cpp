#include "keywords.hh"


using namespace keywords;


const std::unordered_map<std::string, KeywordValue> keywords::keywordValues
({
    {"true",    KeywordValue(1, Tokens::TokenType::BOOL)},
    {"false",   KeywordValue(0, Tokens::TokenType::BOOL)},
});


KeywordValue::KeywordValue(Value value, Tokens::TokenType type)
: value(value), type(type)
{

}

