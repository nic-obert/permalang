#include "preprocessor.hh"

#include <string.h>
#include <unordered_map>

#include "errors.hh"


using namespace preprocessor;


static inline size_t indexOfChar(std::string& string, size_t beginning, char c)
{
    for (; string[beginning] != c; beginning++);
    return beginning;
}


static inline size_t indexOfNotChar(std::string& string, size_t beginning, char c)
{
    for (; string[beginning] == c; beginning++);
    return beginning;
}


static const std::unordered_map<std::string, Preps> preprocessorMap = std::unordered_map<std::string, Preps>
({
    {"define",  Preps::DEFINE},
    {"include", Preps::INCLUDE},
    {"ifdef",   Preps::IFDEF},
    {"ifndef",  Preps::IFNDEF}
});


static void preprocessorInclude(std::string& script, size_t& index)
{
    #define INCLUDE_LENGTH 8
    
    // search for the first character after spaces
    size_t enclosureIndex = indexOfNotChar(script, index + INCLUDE_LENGTH, ' ');

    if (script[enclosureIndex] == '<')
    {
        // search in global include directory
    }
    else if (script[enclosureIndex] == '"')
    {
        // search in relative path
    }
    else
    {
        errors::InvalidIncludeEnclosureError(script[enclosureIndex]);
    }
}


void preprocessor::process(std::string& script)
{
    for (size_t i = 0; script[i] != '\0'; i++)
    {   
        // search for a '#' after a newline
        if (script[i] != '#' || script[i-1] != '\n')
        {
            continue;
        }

        // get the preprocessor name

        size_t pIndex = i+1;
        size_t spaceIndex = indexOfChar(script, pIndex, ' ');
        size_t length = spaceIndex - pIndex;

        char pName[length + 1];
        strncpy(pName, script.c_str() + pIndex, length);
        pName[length] = '\0';

        auto it = preprocessorMap.find(pName);
        if (it == preprocessorMap.end())
        {
            errors::InvalidPreprocessorError(pName);
        }

        switch (it->second)
        {
        case Preps::INCLUDE:
            preprocessorInclude(script, i);
            break;
        
        } // switch(Preps)

    }
}

