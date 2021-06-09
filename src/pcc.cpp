#include "token.hh"
#include "syntax_tree.hh"
#include "preprocessor.hh"
#include "errors.hh"

#include "pch.hh"

#include "argparser.hh"


typedef struct Options
{
    const char* fileName = nullptr;
    const char* outputName = nullptr;
    bool execute;
    bool verbose;

} Options;


static void initParser(argparser::Parser* parser, Options& options)
{
    *parser = argparser::Parser(
        5,
        "Permalang Compiler Collection\n"
        "For anything email nchlsuba@gmail.com"
    );

    parser->addBoolImplicit(
        "-O", &globals::doOptimize, false,
        "apply optimizations to compiled byte code");

    parser->addStringPositional(
        &options.fileName, true,
        "name of the file to load");

    parser->addBoolImplicit(
        "-x", &options.execute, false,
        "execute the specified file instead of compiling");

    parser->addString(
        "-o", &options.outputName, false,
        "compilation output file name");

    parser->addBoolImplicit(
        "-v", &options.verbose, false,
        "verbose compilation");

}


int main(int argc, const char** argv) 
{

    Options options;
    argparser::Parser parser;

    initParser(&parser, options);

    parser.parse(argc, argv);
    

    if (options.execute)
    {

        pvm::ByteCode byteCode = pvm::loadByteCode(options.fileName);
        
        pvm::Pvm pvm = pvm::Pvm(1024);
        pvm::Byte exitCode = pvm.execute(byteCode.byteCode);

        std::cout << "Exit code: " << (unsigned int) exitCode << std::endl;
        
    }
    else // compile
    {

        timerpp::Timer timer;

if (options.verbose)
    timer = timerpp::Timer();

        std::string file;

if (options.verbose)
    timer.start();

        if (!file_utils::loadFile(options.fileName, file))
        {
            errors::FileReadError(options.fileName);
        }

if (options.verbose)
{
    timer.stop();

    std::cout << "File loaded in: " << timer.millis() << " ms\n" << std::endl;


    timer.start();
}
        preprocessor::process(file);

if (options.verbose)
{
    timer.stop();

    std::cout << "Preprocessor took: " << timer.millis() << " ms\n" << std::endl ;


    timer.start();
}
        Tokens::TokenList tokens = Tokens::TokenList(file);

if (options.verbose)
{    
    timer.stop();

    std::cout << "Token List took: " << timer.millis() << " ms" << std::endl;
    std::cout << tokens << '\n' << std::endl;


    timer.start();
}
        syntax_tree::SyntaxTree syntaxTree = syntax_tree::SyntaxTree(tokens);
        const pvm::ByteCode byteCode = syntaxTree.parseToByteCode();

if (options.verbose)
{ 
    timer.stop();

    std::cout << "Syntax Tree took: " << timer.millis() << " ms" << std::endl;

    std::cout << byteCode << '\n' << std::endl;
}
        
        if (options.outputName == nullptr)
        {
            // generate name for executable if not provided
            const size_t nameLength = strlen(options.fileName);
            char outputName[nameLength + 5];
            strncpy(outputName, options.fileName, nameLength);
            strcpy(outputName + nameLength, ".pfx");    

            // outputName is a local variable, not the outer one
            // this prevents copying of the string outputName to extend
            // its lifetime beyond this local scope
            pvm::generateExecutable(byteCode, outputName);
        }
        else
        {
            // here outputName is the outer variable
            pvm::generateExecutable(byteCode, options.outputName);
        }
        
        
    } // do compile

    
}

