#include "errors.hh"


void errors::UnexpectedBehaviourError(const std::string&& message)
{
    std::cerr << "[Unexpected Behaviour Error] " << message << std::endl;
    exit(EXIT_FAILURE);
}


void errors::SyntaxError(const std::string&& message)
{
    std::cerr << "[Syntax Error] " << message << std::endl;
    exit(EXIT_FAILURE);
}


void errors::TypeError(const Tokens::Token& caller, Tokens::TokenType expected, const Tokens::Token& provided, const char* side)
{
    std::cerr << "[Type Error] Expected type "
        << expected << " to the " << side << " of " << caller
        << ", but " << provided << " was provided" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::TypeError(const Tokens::Token& caller, OpCodes expected, const Tokens::Token& provided, const char* side)
{
    std::cerr << "[Type Error] Expected type "
        << expected << " to the " << side << " of " << caller
        << ", but " << provided << " was provided" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::SymbolRedeclarationError(const symbol_table::Symbol& symbol)
{
    std::cerr << "[Symbol Redeclaration Error] Symbol " << symbol
        << " has been redeclared in local scope" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::UndefinedSymbolError(const std::string&& name)
{
    std::cerr << "[Undefined Symbol Error] Symbol name " << name
        << " hasn't been declared in any reachable scope" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::InvalidCharacterError(const std::string&& line, char character)
{
    std::cerr << "[Invalid Character Error] Invalid character '" << character
        << "' in line:\n" << line << std::endl;
    exit(EXIT_FAILURE);
}


void errors::ExpectedTokenError(const Tokens::Token& caller, Tokens::TokenType expected, const char* side)
{
    std::cerr << "[Expected Token Error] Expected type " << expected
        << " to the " << side << " of " << caller << std::endl;
    exit(EXIT_FAILURE);
}


void errors::ExpectedTokenError(const Tokens::Token& caller, OpCodes expected, const char* side)
{
    std::cerr << "[Expected Token Error] Expected type " << expected
        << " to the " << side << " of " << caller << std::endl;
    exit(EXIT_FAILURE);
}


void errors::ZeroDivisionError(const Tokens::Token& caller, const Tokens::Token& op1, const Tokens::Token& op2)
{
    std::cerr << "[Zero Division Error] Zero division performed by operator "
        << caller << " on operands: " << op1 << ", " << op2 << std::endl;
    exit(EXIT_FAILURE);
}


void errors::InvalidPreprocessorError(const char* name)
{
    std::cerr << "[Invalid Preprocessor Error] \"" << name
        << "\" is not a preprocessor " << std::endl;
    exit(EXIT_FAILURE);
}


void errors::InvalidIncludeEnclosureError(char c)
{
    std::cerr << "[Invalid Include Enclosure Error] Character '" << c
        << "' is not a valid file name enclosure. Please use either '</>' or '\"/\"'" << std::endl;
    exit(EXIT_FAILURE);
}


void errors::FileReadError(const char* file)
{
    std::cerr << "[File Read Error] Could not read file \"" << file << '"' << std::endl;
    exit(EXIT_FAILURE);
}


void errors::FileWriteError(const char* file)
{
    std::cerr << "[File Write Error] Could not write to file \"" << file << '"' << std::endl;
    exit(EXIT_FAILURE);
}


void errors::MissingClosingParenthesisError(const Tokens::Token& caller, const std::string&& message)
{
    std::cerr << "[Missing Closing Parenthesis Error] Missing closing parenthesis required by "
        << caller << '\n' << message << std::endl;
    exit(EXIT_FAILURE);
}


