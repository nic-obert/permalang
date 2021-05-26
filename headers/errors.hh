#pragma once

#include "pch.hh"

#include "symbol_table.hh"
#include "token.hh"
#include "op_codes.hh"


namespace errors
{
    
    void UnexpectedBehaviourError(std::string&& message);

    void SyntaxError(std::string&& message);

    void TypeError(Tokens::Token& caller, Tokens::TokenType expected, const Tokens::Token& provided, const char* side);
    void TypeError(Tokens::Token& caller, OpCodes expected, const Tokens::Token& provided, const char* side);

    void SymbolRedeclarationError(const symbol_table::Symbol& symbol);
    
    void UndefinedSymbolError(std::string&& name);

    void InvalidCharacterError(std::string&& line, char character);

    void ExpectedTokenError(Tokens::Token& caller, Tokens::TokenType expected, const char* side);
    void ExpectedTokenError(Tokens::Token& caller, OpCodes expected, const char* side);

    void ZeroDivisionError(Tokens::Token& caller, Tokens::Token& op1, Tokens::Token& op2);

    void InvalidPreprocessorError(const char* name);

    void InvalidIncludeEnclosureError(char c);

};

