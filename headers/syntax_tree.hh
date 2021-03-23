#pragma once

#include "token.hh"
#include "utils.hh"



namespace syntax_tree
{   


    typedef enum SyntaxType
    {
        REFERENCE,
        LITERAL,
        OPERATOR_BINARY,
        OPERATOR_UNARY
    } SyntaxType;


    class SyntaxNode
    {
    public:

        Tokens::Token* token;

        SyntaxNode* prev = nullptr;
        SyntaxNode* next = nullptr;
        SyntaxNode* parent = nullptr;

        Value value;

        SyntaxType type;


        SyntaxNode(Tokens::Token* token);
        SyntaxNode(Tokens::Token* token, SyntaxNode* prev);


        void satisfy();


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


    };


};



std::ostream& operator<<(std::ostream& stream, syntax_tree::SyntaxTree const& tree);

std::ostream& operator<<(std::ostream& stream, syntax_tree::SyntaxNode const& node);

std::ostream& operator<<(std::ostream& stream, syntax_tree::Statement const& statement);

std::ostream& operator<<(std::ostream& stream, syntax_tree::SyntaxType const& type);


