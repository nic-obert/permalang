#pragma once

#include "pch.hh"

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
        Symbol();

    } Symbol;


    // an unordered map mapping a string (symbol name) to a Symbol
    typedef std::unordered_map<std::string, Symbol*> Table;


    // keeps track of the symbols in scope
    typedef struct Scope
    {
        // outer scope
        // outer table is optional since it can be undefined
        // for independent scopes
        std::optional<Table> outer;
        // local scope
        Table local;

        // number of bytes to push to the stack when evaluating the scope
        size_t localSymbolsSize;

        // previous Scope in the stack
        // will be initialized when added to the stack
        Scope* prev;

        // initialize a completely new Scope
        // that doesn't inherit from outer scopes
        Scope();

        // initialize a new Scope that inherits from
        // the previous Scope
        Scope(const Table& _local, const std::optional<Table>& _outer);

        // scope destructor, deletes symbols in local scope
        ~Scope();

    } Scope;


    // Singleton
    class SymbolTable
    {
    private:

        // references the first scope in the stack
        static Scope* globalScope;

        static Scope* scopeStack;

        SymbolTable() = delete;

    public:

        static void assign(std::string* identifier, Symbol* symbol);


        static void declare(std::string* identifier, Symbol* symbol);


        static void pushScope(bool inherits);


        static void popScope();

        
        static Symbol* get(std::string* identifier);


        // returns the last Scope on the scopeStack
        static const Scope* getScope();

        
        // initializes the SymbolTable
        // pushes the global scope
        static void init();


        // pops the global scope
        static void clear();

    };

};
