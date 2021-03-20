#pragma once

#include "token.hh"
#include "utils.hh"



namespace syntax_tree
{   

    class SyntaxNode
    {
    public:

        Tokens::Token* token;

        SyntaxNode* prev;
        SyntaxNode* next;
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

        Statement* next;
        SyntaxNode* root;

        Statement(SyntaxNode* root, Statement* next);
        Statement(SyntaxNode* root);

    };


    class Statements
    {
    public:

        Statement* start;
        Statement* end;


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
        
        Statements statements;


        SyntaxTree(Tokens::TokenList* tokens);
       

        SyntaxNode* getHighestPriority(SyntaxNode* root);


        void parse();


        void print() const;

    };


};

