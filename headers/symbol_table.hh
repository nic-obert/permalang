#pragma once

#include <unordered_map>

#include "token.hh"


namespace symbol_table
{

    typedef struct Scope
    {
        unsigned int scope;
        unsigned int index;
        Scope* prev = nullptr;

        Scope(unsigned int scope, unsigned int index);

    } Scope;


    class ScopeStack
    {
    private:

        Scope* last;

    public:

        ScopeStack();


        void push(Scope* scope);


        Scope* pop();

    };


    // Singleton
    class SymbolTable
    {
    private:

        static std::unordered_map<Tokens::Token*, Value>* table;

        static ScopeStack* scopes;

        SymbolTable() = delete;

    public:

        static void assign(Tokens::Token* token, Value value);


        static void declare(Tokens::Token* token);


        static void pushScope();


        static void popScope();


        static Value get(Tokens::Token* token);

    };

};
