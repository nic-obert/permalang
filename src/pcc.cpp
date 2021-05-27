#include "token.hh"
#include "syntax_tree.hh"
#include "preprocessor.hh"

#include "pch.hh"

#include "argparser.hh"


int main(int argc, const char** argv) 
{

    if (argc < 2) {
        std::cerr << "No file specified" << std::endl;
        exit(EXIT_FAILURE);
    }


    const char* fileName;

    argparser::Parser parser = argparser::Parser();
    parser.addBoolImplicit("-O", &globals::doOptimize);
    parser.addStringPositional(&fileName);

    parser.parse(argc, argv);


    std::string file;
    file_utils::loadFile(fileName, file);


    timerpp::Timer timer = timerpp::Timer();


    timer.start();
    preprocessor::process(file);
    timer.stop();

    std::cout << "Preprocessor took: " << timer.millis() << " ms" << '\n' << std::endl ;


    timer.start();
    Tokens::TokenList tokens = Tokens::TokenList(file);
    timer.stop();

    std::cout << "Token List took: " << timer.millis() << " ms" << std::endl;
    std::cout << tokens << '\n' << std::endl;


    timer.start();
    syntax_tree::SyntaxTree syntaxTree = syntax_tree::SyntaxTree(tokens);
    pvm::ByteCode byteCode = syntaxTree.parseToByteCode();
    timer.stop();

    std::cout << "Syntax Tree took: " << timer.millis() << " ms" << std::endl;

    // std::cout << syntaxTree << '\n' << std::endl;

    std::cout << byteCode << '\n' << std::endl;


    timer.start();
    pvm::Pvm pvm = pvm::Pvm(1024);
    pvm::Byte exitCode = pvm.execute(byteCode.byteCode);
    timer.stop();


    std::string exitCodeStr;
    string_utils::byteToString(exitCode, exitCodeStr);
    
    std::cout << "Exit code: " << exitCodeStr << std::endl;
    std::cout << "Execution took: " << timer.millis() << " ms" << std::endl;
        
}

