#pragma once

#include <unordered_map>
#include <string>

#include "token.hh"


namespace symbol_table
{

    typedef struct Symbol
    {
        Value value;
        Tokens::TokenType type;

        Symbol(Value value, Tokens::TokenType type);

    } Symbol;


    typedef struct Scope
    {
        unsigned int scope;
        size_t index;
        Scope* prev = nullptr;

        Scope(unsigned int scope, size_t index);

    } Scope;


    class ScopeStack
    {
    private:

        Scope* last;

    public:

        ScopeStack();


        void push(size_t index);


        Scope* pop();

    };


    // Singleton
    class SymbolTable
    {
    private:

        static std::unordered_map<std::string, Symbol*> table;

        static ScopeStack scopes;

        SymbolTable() = delete;

    public:

        static void assign(std::string* identifier, Symbol* symbol);


        static void declare(std::string* identifier, Symbol* symbol);


        static void pushScope();


        static void popScope();

        
        static Symbol* get(std::string* identifier);

    };

};
