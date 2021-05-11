#include "token.hh"
#include "syntax_tree.hh"
#include "tac.hh"


void loadFile(const char* path, std::string& output)
{
    std::ifstream file(path);

    if (file.bad()) 
    {
        std::cerr << "Couldn't read file: " << path << std::endl;
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
    using namespace pvm;

    if (argc < 2) {
        std::cerr << "No file specified" << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];

    std::string file;
    loadFile(filename, file);

    TokenList tokens = TokenList(file);

    std::cout << tokens << '\n' << std::endl;

    SyntaxTree syntaxTree = SyntaxTree(tokens);
    syntaxTree.parse();

    // std::cout << syntaxTree << '\n' << std::endl;

    const Tac& tac = syntaxTree.getTac();
    
    std::cout << tac << '\n' << std::endl;

    ByteCode byteCode = tac.toByteCode();

    std::cout << byteCode.byteCode << '\n' << std::endl;

    Pvm pvm = Pvm(1024);
    Byte exitCode = pvm.execute(byteCode.byteCode);

    std::cout << "Exit code: " << exitCode << std::endl;
    
}

