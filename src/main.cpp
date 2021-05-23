#include "token.hh"
#include "syntax_tree.hh"

#include "timerpp.hh"


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
    using namespace pvm;
    using namespace timerpp;

    if (argc < 2) {
        std::cerr << "No file specified" << std::endl;
        exit(EXIT_FAILURE);
    }

    const char* filename = argv[1];

    std::string file;
    loadFile(filename, file);

    Timer timer = Timer();

    timer.start();
    TokenList tokens = TokenList(file);
    timer.stop();

    std::cout << "Token List took: " << timer.millis() << std::endl;
    std::cout << tokens << '\n' << std::endl;

    timer.start();
    SyntaxTree syntaxTree = SyntaxTree(tokens);
    ByteCode byteCode = syntaxTree.parseToByteCode();
    timer.stop();

    std::cout << "Syntax Tree took: " << timer.millis() << std::endl;

    // std::cout << syntaxTree << '\n' << std::endl;

    std::cout << byteCode.byteCode << '\n' << std::endl;

    timer.start();
    Pvm pvm = Pvm(1024);
    Byte exitCode = pvm.execute(byteCode.byteCode);
    timer.stop();

    std::cout << "Exit code: " << exitCode << std::endl;
    std::cout << "Execution took: " << timer.millis() << std::endl;
    
}

