#include "token.hh"
#include "syntax_tree.hh"
#include "preprocessor.hh"
#include "errors.hh"

#include "pch.hh"

#include "argparser.hh"


int main(int argc, const char** argv) 
{

    const char* fileName;
    const char* outputName = nullptr;
    bool execute;

    argparser::Parser parser = argparser::Parser(4);
    parser.addBoolImplicit("-O", &globals::doOptimize);
    parser.addStringPositional(&fileName, true);
    parser.addBoolImplicit("-x", &execute);
    parser.addString("-o", &outputName);

    parser.parse(argc, argv);
    
    if (execute)
    {

        pvm::ByteCode byteCode = pvm::loadByteCode(fileName);
        
        pvm::Pvm pvm = pvm::Pvm(1024);
        pvm::Byte exitCode = pvm.execute(byteCode.byteCode);


        std::string exitCodeStr;
        string_utils::byteToString(exitCode, exitCodeStr);
        
        std::cout << "Exit code: " << exitCodeStr << std::endl;
        
    }
    else // compile
    {

        timerpp::Timer timer = timerpp::Timer();
    
        std::string file;
        timer.start();
        if (!file_utils::loadFile(fileName, file))
        {
            errors::FileReadError(fileName);
        }
        timer.stop();

        std::cout << "File loaded in: " << timer.millis() << " ms\n" << std::endl;
        

        timer.start();
        preprocessor::process(file);
        timer.stop();

        std::cout << "Preprocessor took: " << timer.millis() << " ms\n" << std::endl ;


        timer.start();
        Tokens::TokenList tokens = Tokens::TokenList(file);
        timer.stop();

        std::cout << "Token List took: " << timer.millis() << " ms" << std::endl;
        std::cout << tokens << '\n' << std::endl;


        timer.start();
        syntax_tree::SyntaxTree syntaxTree = syntax_tree::SyntaxTree(tokens);
        const pvm::ByteCode byteCode = syntaxTree.parseToByteCode();
        timer.stop();

        std::cout << "Syntax Tree took: " << timer.millis() << " ms" << std::endl;

        std::cout << byteCode << '\n' << std::endl;

        
        if (outputName == nullptr)
        {
            // generate name for executable if not provided
            const size_t nameLength = strlen(fileName);
            char outputName[nameLength + 5];
            strncpy(outputName, fileName, nameLength);
            strcpy(outputName + nameLength, ".pfx");    

            // outputName is a local variable, not the outer one
            // this prevents copying of the string outputName to extend
            // its lifetime beyond this local scope
            pvm::generateExecutable(byteCode, outputName);
        }
        else
        {
            // here outputName is the outer variable
            pvm::generateExecutable(byteCode, outputName);
        }
        
        
    } // do compile

    
}

