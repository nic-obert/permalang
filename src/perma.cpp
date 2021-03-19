#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "token.hh"


std::string loadFile(const char* path)
{
    std::ifstream file(path);

    if (file.bad()) 
    {
        std::cerr << "Couldn't read file " << path << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}


int main(int argc, const char** argv) 
{
    using namespace Tokens;

    if (argc < 2) {
        std::cerr << "No file specified" << std::endl;
        exit(EXIT_FAILURE);
    }
    const char* filename = argv[1];

    std::string file = loadFile(filename);

    TokenList* tokens = tokenize(file);

    tokens->print();

}

