#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "token.hh"
#include "syntax_tree.hh"
#include "symbol_table.hh"
#include "tac.hh"


void loadFile(const char* path, std::string& output)
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

    output = buffer.str();
}


int main(int argc, const char** argv) 
{
    using namespace Tokens;
    using namespace syntax_tree;
    using namespace tac;

    if (argc < 2) {
        std::cerr << "No file specified" << std::endl;
        exit(EXIT_FAILURE);
    }
    const char* filename = argv[1];

    std::string file;
    loadFile(filename, file);

    TokenList* tokens = tokenize(file);

    std::cout << *tokens << "\n" << std::endl;

    SyntaxTree syntaxTree = SyntaxTree(tokens);
    syntaxTree.parse();

    std::cout << syntaxTree << "\n" << std::endl;

    Tac tac = Tac();
    tac.parseTree(syntaxTree, Tac::NO_LABEL);

    std::cout << tac << "\n" << std::endl;

}

