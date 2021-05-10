#pragma once

#include "symbol_table.hh"
#include "tac.hh"
#include "token.hh"


namespace syntax_tree
{   

    // linked list of Token organized in a tree-like fashion
    class Statement
    {
    public:

        Statement* next;
        Tokens::Token* root;


        Statement(Tokens::Token* root, Statement* next);
        Statement(Tokens::Token* root);
        Statement();


        void remove(Tokens::Token* token, bool del = false);


        Tokens::Token* getLast() const;

    };


    // linked list of Statement
    class Statements
    {
    public:

        Statement* start;
        Statement* end;


        Statements(Statement* statement);
        Statements();

        // removes the last statement of the linked list
        // does not delete it
        void removeLast();

        void add(Statement* statement);
    
    };


    class SyntaxTree
    {
    private:
        // tac representation of the tree
        tac::Tac tac;

        // compile a CodeBlock to TAC
        // extend the tree's TAC
        void compileBlock(Tokens::Token* root);

        // parse a Statement in the tree
        void parseStatement(Statement* statement);

        // satisfy the requirements of a Token
        // add the generated TAC to the tree's TAC
        // generate SymbolTable entries for the evaluated symbols
        void satisfyToken(Statement* statement, Tokens::Token* token);


    public:
        
        // list of statements
        Statements statements;
        

        SyntaxTree(Tokens::TokenList& tokens);
        SyntaxTree(Statements&& statements);
       
        // get the next token to be evaluated
        Tokens::Token* getHighestPriority(Tokens::Token* root);

        // parse the tree and generate TAC for it recursively
        void parse();


        const tac::Tac& getTac() const;


    };


};



std::ostream& operator<<(std::ostream& stream, syntax_tree::SyntaxTree const& tree);

std::ostream& operator<<(std::ostream& stream, syntax_tree::Statement const& statement);

