#pragma once

#include "token.hh"
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


        void remove(Tokens::Token* token);

    };


    class Statements
    {
    public:

        Statement* start = nullptr;
        Statement* end = nullptr;


        Statements(Statement* statement);
        Statements();


        void add(Statement* statement);


        Tokens::Token* getFirst() const;


        Tokens::Token* getLast() const;    
    
    };


    class SyntaxTree
    {
    public:
        
        Statements statements = nullptr;


        SyntaxTree(Tokens::TokenList* tokens);
       

        Tokens::Token* getHighestPriority(Tokens::Token* root);


        void parse();

    };


};



std::ostream& operator<<(std::ostream& stream, syntax_tree::SyntaxTree const& tree);

std::ostream& operator<<(std::ostream& stream, syntax_tree::Statement const& statement);

