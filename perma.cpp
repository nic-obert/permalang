#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "tokenizer.cpp"
#include "token.cpp"


std::string loadFile(const char* path)
{
    std::ifstream file(path);

    if (file.bad()) 
    {
        std::cerr << "Couldn't read file " << path << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string buffer;
    while (getline(file, buffer))

    file.close();

    return buffer;
}


int main(int argc, const char** argv) 
{

    if (argc < 2) {
        std::cerr << "No file specified" << std::endl;
        exit(EXIT_FAILURE);
    }
    const char* filename = argv[1];

    std::string file = loadFile(filename);

    TokenList* tokens = tokenize(file);

    tokens->print();

}

