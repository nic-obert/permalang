#pragma once

#include "symbol_table.hh"
#include "tac.hh"


namespace syntax_tree
{   

    #define DO_POP_SCOPE true
    #define DONT_POP_SCOPE false


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


    /*
        Tree representation of source code
        a SyntaxTree is a doubly-linked list of
        statements, each structured in a tree-like fashion

        The SyntaxTree works closely along with the SymbolTable
        to generate intermediate three address code
    */
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

        // generate Tac for the tree's code block
        void generateTac(bool doPopScope);

        // parse the tree and generate TAC for it recursively
        // allows to choose whether to pop the declared symbols or not
        // should not be accessible to the public 
        void parse(bool doPopScope);


    public:
        
        // list of statements
        Statements statements;
        

        SyntaxTree(Tokens::TokenList& tokens);
        SyntaxTree(Statements&& statements);
       
        // get the next token to be evaluated
        Tokens::Token* getHighestPriority(Tokens::Token* root);

        // parse the tree and generate TAC for it recursively
        // internally calls the private method `void parse(bool)`
        void parse();


        const tac::Tac& getTac() const;


    };


};



std::ostream& operator<<(std::ostream& stream, const syntax_tree::SyntaxTree& tree);

std::ostream& operator<<(std::ostream& stream, const syntax_tree::Statement& statement);

