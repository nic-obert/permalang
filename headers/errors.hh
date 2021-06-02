#pragma once

#include "pch.hh"

#include "symbol_table.hh"
#include "token.hh"
#include "op_codes.hh"


namespace errors
{
    
    void UnexpectedBehaviourError(const std::string& message);


    void SyntaxError(const std::string& message);


    void TypeError(const Tokens::Token& caller, Tokens::TokenType expected, const Tokens::Token& provided, const char* side);
    void TypeError(const Tokens::Token& caller, OpCodes expected, const Tokens::Token& provided, const char* side);


    void SymbolRedeclarationError(const symbol_table::Symbol& symbol);
    

    void UndefinedSymbolError(const std::string& name);


    void InvalidCharacterError(const std::string& line, char character);


    void ExpectedTokenError(const Tokens::Token& caller, Tokens::TokenType expected, const char* side);
    void ExpectedTokenError(const Tokens::Token& caller, OpCodes expected, const char* side);


    void ZeroDivisionError(const Tokens::Token& caller, const Tokens::Token& op1, const Tokens::Token& op2);


    void InvalidPreprocessorError(const char* name);


    void InvalidIncludeEnclosureError(char c);


    void FileReadError(const char* file);


    void FileWriteError(const char* file);


    void MissingClosingParenthesisError(const Tokens::Token& caller, const std::string& message);

};

