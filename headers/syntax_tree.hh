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


        void satisfy();


        void print() const;

    };


    class SyntaxTree
    {
    public:

        SyntaxNode* root;


        SyntaxTree(Tokens::TokenList* tokens);
       

        SyntaxNode* getHighestPriority();


        void parse();

    };


};
