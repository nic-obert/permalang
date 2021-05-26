#pragma once

#include <iostream>


namespace preprocessor
{

    typedef enum class Preps
    {
        INCLUDE,
        DEFINE,
        IFDEF,
        IFNDEF

    } Preps;


    // applies preprocessor rules to the provided script
    // changes the string
    void process(std::string& script);

};

