#pragma once

#include "token.hh"
#include "symbol_table.hh"
#include "utils.hh"



namespace syntax_tree
{   

    class Statement
    {
    public:

        Statement* next = nullptr;
        Tokens::Token* root = nullptr;


        Statement(Tokens::Token* root, Statement* next);
        Statement(Tokens::Token* root);
        Statement();


        void satisfy(Tokens::Token* token);


        void remove(Tokens::Token* token, bool del = false);


        Tokens::Token* getLast() const;

    };


    class Statements
    {
    public:

        Statement* start = nullptr;
        Statement* end = nullptr;


        Statements(Statement* statement);
        Statements();

        // removes the last statement of the linked list
        // does not delete it
        void removeLast();

        void add(Statement* statement);
    
    };


    class SyntaxTree
    {
    public:
        
        Statements statements = nullptr;
        

        SyntaxTree(Tokens::TokenList* tokens);
        SyntaxTree(Statements& statements);
       

        Tokens::Token* getHighestPriority(Tokens::Token* root);


        void parse();

    };


};



std::ostream& operator<<(std::ostream& stream, syntax_tree::SyntaxTree const& tree);

std::ostream& operator<<(std::ostream& stream, syntax_tree::Statement const& statement);

