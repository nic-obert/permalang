#pragma once

#include "pvm.hh"
#include "token.hh"


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


    class SyntaxTree
    {
    private:

        pvm::ByteList byteList;


        void parseStatement(Statement* statement);

        // satisfy the requirements of a Token
        // add the generated byteCode to the tree's byteCode
        // generate SymbolTable entries for the evaluated symbols
        void satisfyToken(Statement* statement, Tokens::Token* token);

        void generateByteCode(bool doPopScope);

        size_t byteCodeFor(Tokens::Token* token, Tokens::Token** operands, bool doStoreResult);

        void parseTokenOperator(Tokens::Token* token);

        // parse the tree and generate byte code for it recursively
        // allows to choose whether to pop the declared symbols or not
        // should not be accessible to the public 
        void parseToByteCode(bool doPopScope);


    public:
        
        Statements statements;
        
        SyntaxTree();
        SyntaxTree(Tokens::TokenList& tokens);
        SyntaxTree(Statements&& statements);
       
        Tokens::Token* getHighestPriority(Tokens::Token* root);

        pvm::ByteCode parseToByteCode();

    };


};



std::ostream& operator<<(std::ostream& stream, const syntax_tree::SyntaxTree& tree);

std::ostream& operator<<(std::ostream& stream, const syntax_tree::Statement& statement);

