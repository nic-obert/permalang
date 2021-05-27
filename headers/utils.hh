#pragma once

#include "pch.hh"


// a Value can represent anything since it can be
// either a literal or a pointer
typedef unsigned long Value;


// casts anything to a Value
#define toValue(x) ((Value) x)


namespace file_utils
{

    // loads a file in the string
    // returns true if successful, false if not
    bool loadFile(const char* path, std::string& output);

};


namespace string_utils
{

    size_t indexOfChar(std::string& string, size_t beginning, char c);

    size_t indexOfNotChar(std::string& string, size_t beginning, char c);

    void byteToString(unsigned char number, std::string& output);

};


namespace globals
{

    extern const char* INCLUDE_PATH;

    extern bool doOptimize;

};

