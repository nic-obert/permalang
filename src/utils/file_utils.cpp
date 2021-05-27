#include "utils.hh"


using namespace file_utils;


bool file_utils::loadFile(const char* path, std::string& output)
{
    std::ifstream file(path);

    if (file.bad()) 
    {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    output = buffer.str();

    return true;
}

