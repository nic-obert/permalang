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

        Tokens::Token* root;

        Statement* next;


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


    // tree representation of source code
    class SyntaxTree
    {
    private:
    
        // tac representation of the tree
        tac::Tac tacRepr;

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



std::ostream& operator<<(std::ostream& stream, const syntax_tree::SyntaxTree& tree);

std::ostream& operator<<(std::ostream& stream, const syntax_tree::Statement& statement);

