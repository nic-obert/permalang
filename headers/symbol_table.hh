#pragma once

#include <unordered_map>
#include <string>
#include <optional>

#include "token.hh"


namespace symbol_table
{
    // do inherit from outer scope
    #define DO_INHERIT true
    // do not inherit from outer scope
    #define DONT_INHERIT false


    // holds a Value along with its TokenType
    typedef struct Symbol
    {
        Value value;
        Tokens::TokenType type;

        Symbol(Value value, Tokens::TokenType type);

    } Symbol;


    // an unordered map mapping a string (symbol name) to a Symbol
    typedef std::unordered_map<std::string, Symbol*> Table;


    // keeps track of the symbols in scope
    typedef struct Scope
    {
        // outer table is optional since it can be undefined
        std::optional<Table> outer;
        Table local;

        Scope* prev = nullptr;

        // initialize a completely new Scope
        // that doesn't inherit from outer scopes
        Scope();

        // initialize a new Scope that inherits from
        // the previous Scope
        Scope(Table& _local, std::optional<Table>& _outer);

        // scope destructor, deletes symbols in local scope
        ~Scope();

    } Scope;


    // Singleton
    class SymbolTable
    {
    private:

        static Table globalScope;

        static Scope* scopeStack;

        SymbolTable() = delete;

    public:

        static void assign(std::string* identifier, Symbol* symbol);


        static void declare(std::string* identifier, Symbol* symbol);


        static void pushScope(bool inherits);


        static void popScope();

        
        static Symbol* get(std::string* identifier);

    };

};
