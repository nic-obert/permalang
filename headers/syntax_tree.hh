#pragma once

#include "token.hh"
#include "utils.hh"



namespace syntax_tree
{   

    class SyntaxNode
    {
    public:

        Tokens::Token* token;

        SyntaxNode* prev = nullptr;
        SyntaxNode* next = nullptr;
        SyntaxNode* parent = nullptr;

        Value value;


        SyntaxNode(Tokens::Token* token);
        SyntaxNode(Tokens::Token* token, SyntaxNode* prev);


        void satisfy();


        void print() const;

    };


    class Statement
    {
    public:

        Statement* next = nullptr;
        SyntaxNode* root = nullptr;

        Statement(SyntaxNode* root, Statement* next);
        Statement(SyntaxNode* root);

    };


    class Statements
    {
    public:

        Statement* start = nullptr;
        Statement* end = nullptr;


        Statements(SyntaxNode* item);
        Statements();


        void add(SyntaxNode* item);


        SyntaxNode* get(unsigned int index) const;


        SyntaxNode* getFirst() const;


        SyntaxNode* getLast() const;
    
    
    };


    class SyntaxTree
    {
    public:
        
        Statements statements = nullptr;


        SyntaxTree(Tokens::TokenList* tokens);
       

        SyntaxNode* getHighestPriority(SyntaxNode* root);


        void parse();


        void print() const;

    };


};

