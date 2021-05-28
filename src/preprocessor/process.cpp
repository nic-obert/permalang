#include "preprocessor.hh"

#include <string.h>
#include <unordered_map>

#include "errors.hh"


using namespace preprocessor;
using namespace string_utils;


static const auto preprocessorMap = std::unordered_map<std::string, Preps>
({
    {"define",  Preps::DEFINE},
    {"include", Preps::INCLUDE},
    {"ifdef",   Preps::IFDEF},
    {"ifndef",  Preps::IFNDEF}
});


static void preprocessorInclude(std::string& script, size_t& hashIndex)
{
    #define INCLUDE_LENGTH 8
    
    // search for the first character after spaces
    const size_t enclosureIndex = indexOfNotChar(script, hashIndex + INCLUDE_LENGTH, ' ');

    // content of the file to include
    std::string file;

    if (script[enclosureIndex] == '<')
    {
        // search in global include directory

    // get file path

        const size_t enclosureClose = indexOfChar(script, enclosureIndex + 1, '>');

        // allocate a char array with the size of the file name
        const size_t nameLength = enclosureClose - enclosureIndex - 1;

        const size_t includePathLength = strlen(globals::INCLUDE_PATH);

        // + 2 to make space for the null termination character and the / in the path name
        const size_t filePathLength = includePathLength + nameLength + 2;
        char filePath[filePathLength];

        strcpy(filePath, globals::INCLUDE_PATH);
        filePath[includePathLength] = '/';

        // extract file name from enclosure
        strncpy(filePath + includePathLength + 1, script.c_str() + enclosureIndex + 1, nameLength);
        filePath[filePathLength - 1] = '\0';

    // read file

        if (!file_utils::loadFile(filePath, file))
        {
            errors::FileReadError(filePath);
        }

    // remove the #include statement until the end of the line

        const size_t lineEnd = indexOfChar(script, enclosureClose, '\n');
        script.erase(hashIndex, lineEnd - hashIndex);

    }
    else if (script[enclosureIndex] == '"')
    {
        // search in relative path
    
    // get file path

        const size_t enclosureClose = indexOfChar(script, enclosureIndex, '"');

        const size_t filePathLength = enclosureClose - enclosureIndex - 1;
        char filePath[filePathLength + 1];

        strncpy(filePath, script.c_str() + enclosureIndex + 1, filePathLength);
        filePath[filePathLength] = '\0';

    // read file

        if (!file_utils::loadFile(filePath, file))
        {
            errors::FileReadError(filePath);
        }

    // remove the #include statement until the end of the line

        // + 1 to remove the newline character
        const size_t lineEnd = indexOfChar(script, enclosureClose, '\n') + 1;
        script.erase(hashIndex, lineEnd - hashIndex);

    }
    else
    {
        errors::InvalidIncludeEnclosureError(script[enclosureIndex]);
    }

    // include the loaded file

    script.insert(hashIndex, file);    

}


void preprocessor::process(std::string& script)
{
    for (size_t i = 0; script[i] != '\0'; i++)
    {   
        // search for a '#' after a newline
        if ((script[i] != '#' || script[i-1] != '\n')
            && i != 0)
        {
            continue;
        }

        // get the preprocessor name

        const size_t pIndex = i+1;
        const size_t spaceIndex = indexOfChar(script, pIndex, ' ');
        const size_t length = spaceIndex - pIndex;

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

    } // for (char in script)
    
}

